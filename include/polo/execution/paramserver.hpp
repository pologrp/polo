#ifndef POLO_EXECUTION_PARAMSERVER_HPP_
#define POLO_EXECUTION_PARAMSERVER_HPP_

#include <algorithm>
#include <cstdint>
#include <exception>
#include <future>
#include <iterator>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "cereal/archives/portable_binary.hpp"
#include "cereal/types/vector.hpp"

#include "polo/communicator/address.hpp"
#include "polo/communicator/zmq.hpp"
#include "polo/utility/sampler.hpp"

namespace polo {
namespace execution {
namespace paramserver {
namespace detail {
template <class T>
void deserialize(communicator::zmq::message &msg, const std::size_t pid,
                 T &val) {
  std::istringstream iss(std::ios_base::binary);
  iss.str(msg.read(pid));
  cereal::PortableBinaryInputArchive ar(iss);
  ar(val);
}
template <class T> std::string serialize(const T &val) {
  std::ostringstream oss(std::ios_base::binary);
  {
    cereal::PortableBinaryOutputArchive ar(oss);
    ar(val);
  }
  return oss.str();
}
} // namespace detail

struct options {
  options() = default;

  void linger(const int time) noexcept { linger_ = time; }
  int linger() const noexcept { return linger_; }

  void master_timeout(const long timeout) noexcept { mtimeout_ = timeout; }
  long master_timeout() const noexcept { return mtimeout_; }

  void worker_timeout(const long timeout) noexcept { wtimeout_ = timeout; }
  long worker_timeout() const noexcept { return wtimeout_; }

  void scheduler_timeout(const long timeout) noexcept { stimeout_ = timeout; }
  long scheduler_timeout() const noexcept { return stimeout_; }

  void timeout(const long timeout) noexcept {
    master_timeout(timeout);
    worker_timeout(timeout);
    scheduler_timeout(timeout);
  }

  void num_masters(const std::int32_t num) noexcept { num_masters_ = num; }
  std::int32_t num_masters() const noexcept { return num_masters_; }

  void
  scheduler(std::tuple<std::string, std::uint16_t, std::uint16_t, std::uint16_t>
                scheduler_info) noexcept {
    scheduler(std::get<0>(scheduler_info), std::get<1>(scheduler_info),
              std::get<2>(scheduler_info), std::get<3>(scheduler_info));
  }
  void scheduler(std::string address, const std::uint16_t pub_port,
                 const std::uint16_t master_port,
                 const std::uint16_t worker_port) noexcept {
    saddress_ = std::move(address);
    spub_ = pub_port;
    smaster_ = master_port;
    sworker_ = worker_port;
  }
  std::tuple<std::string, std::uint16_t, std::uint16_t, std::uint16_t>
  scheduler() const noexcept {
    return std::make_tuple(saddress_, spub_, smaster_, sworker_);
  }

  void master(std::uint16_t worker_port) noexcept { mworker_ = worker_port; }
  std::uint16_t master() const noexcept { return mworker_; }

private:
  int linger_{1000};
  long mtimeout_{10000}, wtimeout_{-1}, stimeout_{-1};
  std::int32_t num_masters_{1};
  std::string saddress_{"localhost"};
  std::uint16_t spub_{40000}, smaster_{40001}, sworker_{40002};
  std::uint16_t mworker_{40000};
};

template <class value_t, class index_t> struct master {
  master() = default;

  master(const master &) = default;
  master &operator=(const master &) = default;
  master(master &&) = default;
  master &operator=(master &&) = default;

protected:
  void parameters(options opts) {
    linger = opts.linger();
    timeout = opts.master_timeout();
    auto scheduler = opts.scheduler();
    saddress = std::get<0>(scheduler);
    spub = std::get<1>(scheduler);
    smaster = std::get<2>(scheduler);
    mworker = opts.master();
  }

  template <class InputIt> std::vector<value_t> initialize(InputIt, InputIt) {
    subscription =
        communicator::zmq::socket{ctx, communicator::zmq::socket_type::sub};
    subscription.set(communicator::zmq::socket_opt::subscribe, 'M');
    subscription.set(communicator::zmq::socket_opt::subscribe, 'A');
    std::string address = "tcp://" + saddress + ":" + std::to_string(spub);
    subscription.connect(address.c_str());

    request =
        communicator::zmq::socket{ctx, communicator::zmq::socket_type::req};
    request.set(communicator::zmq::socket_opt::linger, linger);
    address = "tcp://" + saddress + ":" + std::to_string(smaster);
    request.connect(address.c_str());

    router =
        communicator::zmq::socket{ctx, communicator::zmq::socket_type::router};
    router.set(communicator::zmq::socket_opt::linger, linger);
    while (true) {
      try {
        address = "tcp://*:" + std::to_string(mworker);
        router.bind(address.c_str());
        break;
      } catch (...) {
        mworker++;
      }
    }

    communicator::ip myip = communicator::ip::getexternal();
    address = "tcp://" + myip.get() + ":" + std::to_string(mworker);

    communicator::zmq::message msg;
    msg.addpart(std::begin(address), std::end(address));
    msg.send(request);

    poll.additem(request, communicator::zmq::poll_event::pollin);

    if (poll.poll(timeout) == 0)
      throw std::runtime_error(address + ": No response from " + saddress +
                               ":" + std::to_string(smaster) + " for " +
                               std::to_string(timeout) + " ms.");

    msg.receive(request);
    if (msg.size() == 0)
      throw std::runtime_error(address + ": Empty message from " + saddress +
                               ":" + std::to_string(smaster) + ".");

    detail::deserialize(msg, 0, startind);
    detail::deserialize(msg, 1, x);
    g = std::vector<value_t>(x.size());

    poll.clear();
    poll.additem(subscription, communicator::zmq::poll_event::pollin);
    poll.additem(router, communicator::zmq::poll_event::pollin);

    return x;
  }

  template <class Algorithm, class Loss, class Terminator, class Logger>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger) {
    value_t fval;
    std::vector<index_t> indices;
    communicator::zmq::message msg;

    while (poll.poll(timeout) > 0) {
      if (poll[0].isready()) {
        msg.receive(subscription);
        if (msg.size() == 1 && msg.read<char>(0) == 'T')
          break;

        detail::deserialize(msg, 0, k);
      }

      if (poll[1].isready()) {
        msg.receive(router);

        std::size_t pid{0};
        while (msg.size(pid) != 0)
          pid++;
        pid++;

        if (msg.size(pid) == 0) {
          msg.send(router);
          continue;
        }

        index_t kworker;
        std::vector<value_t> values;
        const char tag = msg.read<char>(pid++);

        if (tag == 'x') {
          if (msg.size(pid) == 0)
            values = x;
          else {
            detail::deserialize(msg, pid, indices);
            for (const auto ind : indices)
              values.push_back(x[ind - startind]);
          }
          auto data = detail::serialize(values);
          msg[pid] = {std::begin(data), std::end(data)};
          msg.send(router);
        } else if (tag == 'g') {
          detail::deserialize(msg, pid++, kworker);
          detail::deserialize(msg, pid++, fval);
          if (msg.size(pid) == 0)
            detail::deserialize(msg, pid + 1, g);
          else {
            for (auto &val : g)
              val = 0;
            detail::deserialize(msg, pid++, indices);
            detail::deserialize(msg, pid, values);
            std::size_t valind{0};
            for (const auto ind : indices)
              g[ind - startind] = values[valind++];
          }
          alg->boost(std::begin(g), std::end(g), std::begin(g));
          alg->smooth(k, std::begin(x), std::end(x), std::begin(g),
                      std::begin(g));
          auto step =
              alg->step(k, fval, std::begin(x), std::end(x), std::begin(g));
          alg->prox(step, std::begin(x), std::end(x), std::begin(g),
                    std::begin(x));
          std::forward<Logger>(logger)(k, fval, std::begin(x), std::end(x),
                                       std::begin(g), std::end(g));
          msg.pop_back();
          msg.pop_back();
          msg.pop_back();
          msg.pop_back();
          msg.addpart();
          msg.send(router);
        }
      }
    }
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Space, class Sampler>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger, Space, Sampler &&, const index_t) {
    solve(alg, std::forward<Loss>(loss), std::forward<Terminator>(terminate),
          std::forward<Logger>(logger));
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Space1, class Sampler1, class Space2, class Sampler2>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger, Space1, Sampler1 &&, const index_t, Space2,
             Sampler2 &&, const index_t) {
    solve(alg, std::forward<Loss>(loss), std::forward<Terminator>(terminate),
          std::forward<Logger>(logger));
  }

  ~master() = default;

private:
  int linger;
  long timeout;
  std::string saddress;
  std::uint16_t spub, smaster, mworker;
  index_t startind, k;
  std::vector<value_t> x, g;
  communicator::zmq::context ctx;
  communicator::zmq::socket request{ctx, communicator::zmq::socket_type::req},
      router{ctx, communicator::zmq::socket_type::router},
      subscription{ctx, communicator::zmq::socket_type::sub};
  communicator::zmq::poller poll;
};

template <class value_t, class index_t> struct worker {
  worker() = default;

  worker(const worker &) = default;
  worker &operator=(const worker &) = default;
  worker(worker &&) = default;
  worker &operator=(worker &&) = default;

protected:
  void parameters(options opts) {
    linger = opts.linger();
    timeout = opts.worker_timeout();
    auto scheduler = opts.scheduler();
    saddress = std::get<0>(scheduler);
    spub = std::get<1>(scheduler);
    sworker = std::get<3>(scheduler);
  }

  template <class InputIt>
  std::vector<value_t> initialize(InputIt xbegin, InputIt xend) {
    x = std::vector<value_t>(xbegin, xend);
    g = std::vector<value_t>(x.size());

    subscription =
        communicator::zmq::socket{ctx, communicator::zmq::socket_type::sub};
    subscription.set(communicator::zmq::socket_opt::subscribe, 'W');
    subscription.set(communicator::zmq::socket_opt::subscribe, 'A');
    std::string address = "tcp://" + saddress + ":" + std::to_string(spub);
    subscription.connect(address.c_str());

    request =
        communicator::zmq::socket{ctx, communicator::zmq::socket_type::req};
    request.set(communicator::zmq::socket_opt::linger, linger);
    address = "tcp://" + saddress + ":" + std::to_string(sworker);
    request.connect(address.c_str());

    poll.additem(subscription, communicator::zmq::poll_event::pollin);
    poll.additem(request, communicator::zmq::poll_event::pollin);

    return x;
  }

  template <class Algorithm, class Loss, class Terminator, class Logger>
  void solve(Algorithm *, Loss &&loss, Terminator &&, Logger &&) {
    const value_t *xbegin{&x[0]};

    auto f = [&, xbegin]() { fval = std::forward<Loss>(loss)(xbegin, &g[0]); };

    kernel(f);
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Sampler>
  void solve(Algorithm *, Loss &&loss, Terminator &&, Logger &&,
             utility::sampler::detail::component_sampler_t, Sampler &&sampler,
             const index_t num_components) {
    const value_t *xbegin{&x[0]};
    std::vector<index_t> components(num_components);

    auto f = [&, xbegin, num_components]() {
      std::forward<Sampler>(sampler)(std::begin(components),
                                     std::end(components));
      std::sort(std::begin(components), std::end(components));
      fval = std::forward<Loss>(loss)(xbegin, &g[0], &components[0],
                                      &components[num_components]);
    };

    kernel(f);
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Sampler>
  void solve(Algorithm *, Loss &&loss, Terminator &&, Logger &&,
             utility::sampler::detail::coordinate_sampler_t, Sampler &&sampler,
             const index_t num_coordinates) {
    const value_t *xbegin{&x[0]};
    std::vector<index_t> coordinates(num_coordinates);
    std::vector<value_t> partial(num_coordinates);

    auto f = [&, xbegin, num_coordinates]() {
      std::forward<Sampler>(sampler)(std::begin(coordinates),
                                     std::end(coordinates));
      std::sort(std::begin(coordinates), std::end(coordinates));
      fval = std::forward<Loss>(loss)(xbegin, &partial[0], &coordinates[0],
                                      &coordinates[num_coordinates]);
      for (std::size_t idx = 0; idx < num_coordinates; idx++)
        g[coordinates[idx]] = partial[idx];
    };

    kernel(f, &coordinates);
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Sampler1, class Sampler2>
  void solve(Algorithm *, Loss &&loss, Terminator &&, Logger &&,
             utility::sampler::detail::component_sampler_t, Sampler1 &&sampler1,
             const index_t num_components,
             utility::sampler::detail::coordinate_sampler_t,
             Sampler2 &&sampler2, const index_t num_coordinates) {
    const value_t *xbegin{&x[0]};
    std::vector<index_t> components(num_components),
        coordinates(num_coordinates);
    std::vector<value_t> partial(num_coordinates);

    auto f = [&, xbegin, num_components, num_coordinates]() {
      std::forward<Sampler1>(sampler1)(std::begin(components),
                                       std::end(components));
      std::sort(std::begin(components), std::end(components));
      std::forward<Sampler2>(sampler2)(std::begin(coordinates),
                                       std::end(coordinates));
      std::sort(std::begin(coordinates), std::end(coordinates));
      fval = std::forward<Loss>(loss)(
          xbegin, &partial[0], &components[0], &components[num_components],
          &coordinates[0], &coordinates[num_coordinates]);
      for (std::size_t idx = 0; idx < num_coordinates; idx++)
        g[coordinates[idx]] = partial[idx];
    };

    kernel(f, &coordinates);
  }

  ~worker() = default;

private:
  void askfor(const char tag, const std::vector<index_t> *indices = nullptr) {
    std::vector<index_t> range(2);
    range[0] = indices == nullptr ? 0 : indices->front();
    range[1] = indices == nullptr ? x.size() - 1 : indices->back();
    auto data = detail::serialize(range);

    msg.clear();
    msg.addpart(tag);
    msg.addpart(std::begin(data), std::end(data));
    msg.send(request);
  }

  void ack() {
    msg.clear();
    msg.addpart('u');
    msg.send(request);
  }

  void ping() {
    msg.clear();
    msg.addpart();
    msg.send(request);
  }

  void receive_x() {
    index_t indstart{0}, indend;
    std::vector<std::future<void>> futures;

    detail::deserialize(msg, 1, k);
    auto xstart = std::begin(x);

    for (std::size_t pid = 2; pid < msg.numparts(); pid += 2) {
      detail::deserialize(msg, pid, indend);

      std::promise<void> promise;
      futures.push_back(promise.get_future());

      std::thread(
          [=](const std::string address, std::promise<void> promise) {
            communicator::zmq::message msg;
            msg.addpart('x');
            msg.addpart();

            communicator::zmq::socket req(ctx,
                                          communicator::zmq::socket_type::req);
            req.set(communicator::zmq::socket_opt::linger, linger);
            req.connect(address.c_str());

            msg.send(req);

            communicator::zmq::poller poll;
            poll.additem(req, communicator::zmq::poll_event::pollin);
            if (poll.poll(timeout) > 0) {
              msg.receive(req);
              std::vector<value_t> values;
              detail::deserialize(msg, 1, values);
              std::copy(std::begin(values), std::end(values), xstart);
              promise.set_value_at_thread_exit();
            } else
              promise.set_exception_at_thread_exit(std::make_exception_ptr(
                  std::runtime_error("Could not get data")));
          },
          msg.read(pid + 1), std::move(promise))
          .detach();

      xstart += indend - indstart;
      indstart = indend;
    }

    for (const auto &future : futures)
      future.wait();

    for (auto &future : futures)
      future.get();
  }

  void send_g(const std::vector<index_t> *indices = nullptr) {
    index_t indstart{0}, indend;
    std::vector<std::future<void>> futures;

    auto kdata = detail::serialize(k);
    auto fdata = detail::serialize(fval);

    auto gstart = std::begin(g);

    if (indices == nullptr) {
      for (std::size_t pid = 1; pid < msg.numparts(); pid += 2) {
        detail::deserialize(msg, pid, indend);
        const index_t ndata = indend - indstart;

        auto gdata =
            detail::serialize(std::vector<value_t>(gstart, gstart + ndata));

        std::promise<void> promise;
        futures.push_back(promise.get_future());

        std::thread(
            [=](const std::string address, const std::string kdata,
                const std::string fdata, const std::string gdata,
                std::promise<void> promise) {
              communicator::zmq::message msg;
              msg.addpart('g');
              msg.addpart(std::begin(kdata), std::end(kdata));
              msg.addpart(std::begin(fdata), std::end(fdata));
              msg.addpart();
              msg.addpart(std::begin(gdata), std::end(gdata));

              communicator::zmq::socket req(
                  ctx, communicator::zmq::socket_type::req);
              req.set(communicator::zmq::socket_opt::linger, linger);
              req.connect(address.c_str());

              msg.send(req);

              communicator::zmq::poller poll;
              poll.additem(req, communicator::zmq::poll_event::pollin);
              if (poll.poll(timeout) > 0) {
                msg.receive(req);
                if (msg.numparts() == 2 && msg.size(1) == 0)
                  promise.set_value_at_thread_exit();
              } else
                promise.set_exception_at_thread_exit(std::make_exception_ptr(
                    std::runtime_error("Could not send data")));
            },
            msg.read(pid + 1), kdata, fdata, std::move(gdata),
            std::move(promise))
            .detach();

        gstart += ndata;
      }
    } else {
      auto startit = std::begin(*indices);
      for (std::size_t pid = 1; pid < msg.numparts(); pid += 2) {
        detail::deserialize(msg, pid, indend);
        auto found =
            std::find_if(startit, std::end(*indices),
                         [=](const index_t val) { return val >= indend; });
        if (startit == found)
          continue;

        auto grange = detail::serialize(std::vector<index_t>(startit, found));

        std::vector<value_t> glocal;
        std::transform(startit, found, std::back_inserter(glocal),
                       [&](const index_t idx) { return g[idx]; });
        auto gdata = detail::serialize(glocal);

        std::promise<void> promise;
        futures.push_back(promise.get_future());

        std::thread(
            [=](const std::string address, const std::string kdata,
                const std::string fdata, const std::string grange,
                const std::string gdata, std::promise<void> promise) {
              communicator::zmq::message msg;
              msg.addpart('g');
              msg.addpart(std::begin(kdata), std::end(kdata));
              msg.addpart(std::begin(fdata), std::end(fdata));
              msg.addpart(std::begin(grange), std::end(grange));
              msg.addpart(std::begin(gdata), std::end(gdata));

              communicator::zmq::socket req(
                  ctx, communicator::zmq::socket_type::req);
              req.set(communicator::zmq::socket_opt::linger, linger);
              req.connect(address.c_str());

              msg.send(req);

              communicator::zmq::poller poll;
              poll.additem(req, communicator::zmq::poll_event::pollin);
              if (poll.poll(timeout) > 0) {
                msg.receive(req);
                if (msg.numparts() == 2 && msg.size(1) == 0)
                  promise.set_value_at_thread_exit();
              } else
                promise.set_exception_at_thread_exit(std::make_exception_ptr(
                    std::runtime_error("Could not send data")));
            },
            msg.read(pid + 1), kdata, fdata, std::move(grange),
            std::move(gdata), std::move(promise))
            .detach();
      }
    }

    for (const auto &future : futures)
      future.wait();

    for (auto &future : futures)
      future.get();
  }

  template <class Function>
  void kernel(Function &&f, const std::vector<index_t> *indices = nullptr) {
    askfor('x');

    while (poll.poll(timeout) > 0) {
      if (poll[0].isready()) {
        msg.receive(subscription);
        if (msg.size() == 1 && msg.read<char>(0) == 'T')
          break;
      }

      if (poll[1].isready()) {
        msg.receive(request);
        const char tag = msg.read<char>(0);

        if (msg.size() == 0 || tag == 'u') {
          askfor('x');
        } else if (tag == 'x') {
          try {
            receive_x();
            std::forward<Function>(f)();
            askfor('g', indices);
          } catch (...) {
            ping();
          }
        } else if (tag == 'g') {
          try {
            send_g(indices);
            ack();
          } catch (...) {
            ping();
          }
        }
      }
    }
  }

  int linger;
  long timeout;
  std::string saddress;
  std::uint16_t spub, sworker;
  index_t k;
  value_t fval;
  std::vector<value_t> x, g;
  communicator::zmq::context ctx;
  communicator::zmq::socket request{ctx, communicator::zmq::socket_type::req},
      subscription{ctx, communicator::zmq::socket_type::sub};
  communicator::zmq::poller poll;
  communicator::zmq::message msg;
};

template <class value_t, class index_t> struct scheduler {
  scheduler() = default;

  scheduler(const scheduler &) = default;
  scheduler &operator=(const scheduler &) = default;
  scheduler(scheduler &&) = default;
  scheduler &operator=(scheduler &&) = default;

protected:
  void parameters(options opts) {
    linger = opts.linger();
    timeout = opts.scheduler_timeout();
    nmasters = opts.num_masters();
    auto scheduler = opts.scheduler();
    ppub = std::get<1>(scheduler);
    pmaster = std::get<2>(scheduler);
    pworker = std::get<3>(scheduler);
  }

  template <class InputIt>
  std::vector<value_t> initialize(InputIt xbegin, InputIt xend) {
    const index_t d = std::distance(xbegin, xend);
    const index_t split = d / nmasters;
    const index_t remain = d % nmasters;

    index_t startind{0}, ndata;
    std::int32_t curmasters{0};

    std::vector<value_t> x(xbegin, xend);

    publisher =
        communicator::zmq::socket{ctx, communicator::zmq::socket_type::pub};
    std::string endpoint = "tcp://*:" + std::to_string(ppub);
    publisher.bind(endpoint.c_str());

    master =
        communicator::zmq::socket{ctx, communicator::zmq::socket_type::router};
    master.set(communicator::zmq::socket_opt::linger, linger);
    endpoint = "tcp://*:" + std::to_string(pmaster);
    master.bind(endpoint.c_str());

    worker =
        communicator::zmq::socket{ctx, communicator::zmq::socket_type::router};
    worker.set(communicator::zmq::socket_opt::linger, linger);
    endpoint = "tcp://*:" + std::to_string(pworker);
    worker.bind(endpoint.c_str());

    poll.additem(master, communicator::zmq::poll_event::pollin);
    communicator::zmq::message msg;

    while (curmasters < nmasters && poll.poll(timeout) > 0) {
      msg.receive(master);

      std::size_t pid{0};
      while (msg.size(pid) != 0)
        pid++;
      pid++;

      std::string address(msg.read(pid));
      msg.pop_back();

      ndata = curmasters < remain ? split + 1 : split;
      auto data = detail::serialize(startind);
      msg.addpart(std::begin(data), std::end(data));

      data = detail::serialize(std::vector<value_t>(xbegin, xbegin + ndata));
      msg.addpart(std::begin(data), std::end(data));

      msg.send(master);

      datadist.emplace_back(std::make_pair(startind, startind + ndata),
                            std::move(address));

      xbegin += ndata;
      startind += ndata;
      curmasters++;
    }
    if (curmasters != nmasters)
      throw std::runtime_error("Scheduler: Only " + std::to_string(curmasters) +
                               " masters joined the network.");

    poll.additem(worker, communicator::zmq::poll_event::pollin);

    return x;
  }

  template <class Algorithm, class Loss, class Terminator, class Logger>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger) {
    communicator::zmq::message msg;

    while (
        !std::forward<Terminator>(terminate)(k, 0, nullptr, nullptr, nullptr) &&
        poll.poll(timeout) > 0) {
      if (poll[1].isready()) {
        msg.receive(worker);

        std::size_t pid{0};
        while (msg.size(pid) != 0)
          pid++;
        pid++;

        if (msg.size(pid) == 0) {
          msg.send(worker);
          continue;
        }

        const char tag = msg.read<char>(pid++);

        if (tag == 'u') {
          k++;
          msg.send(worker);
          msg.clear();
          msg.addpart('M');
          auto data = detail::serialize(k);
          msg.addpart(std::begin(data), std::end(data));
          msg.send(publisher);
        } else if (tag == 'x' || tag == 'g') {
          std::vector<index_t> indices;
          detail::deserialize(msg, pid, indices);
          msg.pop_back();

          if (tag == 'x') {
            auto data = paramserver::detail::serialize(k);
            msg.addpart(std::begin(data), std::end(data));
          }

          for (const auto &pair : datadist) {
            if (pair.first.first <= indices[1] &&
                pair.first.second > indices[0]) {
              auto data = paramserver::detail::serialize(pair.first.second);
              msg.addpart(std::begin(data), std::end(data));
              msg.addpart(std::begin(pair.second), std::end(pair.second));
            }
          }

          msg.send(worker);
        }
      }

      if (poll[0].isready()) {
      }
    }

    msg.clear();
    msg.addpart('A');
    msg.addpart('T');
    msg.send(publisher);

    if (!std::forward<Terminator>(terminate)(k, 0, nullptr, nullptr, nullptr))
      throw std::runtime_error("Scheduler: No activity for " +
                               std::to_string(timeout) + " ms.");
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Space, class Sampler>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger, Space, Sampler &&, const index_t) {
    solve(alg, std::forward<Loss>(loss), std::forward<Terminator>(terminate),
          std::forward<Logger>(logger));
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Space1, class Sampler1, class Space2, class Sampler2>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger, Space1, Sampler1 &&, const index_t, Space2,
             Sampler2 &&, const index_t) {
    solve(alg, std::forward<Loss>(loss), std::forward<Terminator>(terminate),
          std::forward<Logger>(logger));
  }

  ~scheduler() = default;

private:
  int linger;
  long timeout;
  std::int32_t nmasters;
  std::uint16_t ppub, pmaster, pworker;
  index_t k{1};
  std::vector<std::pair<std::pair<index_t, index_t>, std::string>> datadist;
  communicator::zmq::context ctx;
  communicator::zmq::socket publisher{ctx, communicator::zmq::socket_type::pub},
      master{ctx, communicator::zmq::socket_type::router},
      worker{ctx, communicator::zmq::socket_type::router};
  communicator::zmq::poller poll;
};

#ifdef POLO_PARAMSERVER_MASTER
template <class value_t, class index_t>
using executor = master<value_t, index_t>;
#elif defined POLO_PARAMSERVER_WORKER
template <class value_t, class index_t>
using executor = worker<value_t, index_t>;
#else
template <class value_t, class index_t>
using executor = scheduler<value_t, index_t>;
#endif
} // namespace paramserver
} // namespace execution
} // namespace polo

#endif
