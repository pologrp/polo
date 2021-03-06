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
#include <type_traits>
#include <utility>
#include <vector>

#include "cereal/archives/portable_binary.hpp"
#include "cereal/types/vector.hpp"

#ifdef POLO_WITH_CURL
#include "polo/communicator/address.hpp"
#endif

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

  void master(std::string address, std::uint16_t worker_port) noexcept {
    maddress_ = std::move(address);
    mworker_ = worker_port;
  }
  void master(std::uint16_t worker_port) noexcept { mworker_ = worker_port; }
  std::pair<std::string, std::uint16_t> master() const noexcept {
    return {maddress_, mworker_};
  }

private:
  int linger_{1000};
  long mtimeout_{10000}, wtimeout_{-1}, stimeout_{-1};
  std::int32_t num_masters_{1};
  std::string saddress_{"localhost"}, maddress_;
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
    auto m = opts.master();
    maddress = m.first;
    mworker = m.second;
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

    if (maddress.empty()) {
#ifdef POLO_WITH_CURL
      communicator::ip myip = communicator::ip::getexternal();
      address = "tcp://" + myip.get() + ":" + std::to_string(mworker);
#endif
    } else
      address = "tcp://" + maddress + ":" + std::to_string(mworker);

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

    xb = x.data();
    xb_c = xb;
    xe_c = xb_c + x.size();
    gb = g.data();
    ge = gb + g.size();
    gb_c = gb;
    ge_c = ge;

    poll.clear();
    poll.additem(subscription, communicator::zmq::poll_event::pollin);
    poll.additem(router, communicator::zmq::poll_event::pollin);

    return x;
  }

  template <class Algorithm, class Loss, class Logger, class Terminator,
            class Encoder>
  void solve(Algorithm *alg, Loss &&loss, Logger &&logger,
             Terminator &&terminate, Encoder &&) {
    value_t fval;
    index_t wid, kworker;
    std::string data;
    communicator::zmq::message msg;

    typename std::decay<Encoder>::type::result_type enc;

    while (poll.poll(timeout) > 0) {
      if (poll[0].isready()) {
        msg.receive(subscription);
        if (msg.size(1) == 1 && msg.read<char>(1) == 'T')
          break;

        detail::deserialize(msg, 1, k);
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

        const char tag = msg.read<char>(pid++);

        if (tag == 'x') {
          data = detail::serialize(x);
          msg[pid] = {std::begin(data), std::end(data)};
          msg.send(router);
        } else if (tag == 'g') {
          detail::deserialize(msg, pid++, wid);
          detail::deserialize(msg, pid++, kworker);
          detail::deserialize(msg, pid++, fval);
          detail::deserialize(msg, pid, enc);
          enc(gb, ge, startind);
          alg->boost(wid, kworker, k, gb_c, ge_c, gb);
          alg->smooth(kworker, k, xb_c, xe_c, gb_c, gb);
          const value_t step = alg->step(kworker, k, fval, xb_c, xe_c, gb_c);
          alg->prox(step, xb_c, xe_c, gb_c, xb);
          std::forward<Logger>(logger)(k, fval, xb_c, xe_c, gb_c);
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

  template <class Algorithm, class Loss, class Space, class Sampler,
            class Logger, class Terminator, class Encoder>
  void solve(Algorithm *alg, Loss &&loss, Space, Sampler &&, const index_t,
             Logger &&logger, Terminator &&terminate, Encoder &&encoder) {
    solve(alg, std::forward<Loss>(loss), std::forward<Logger>(logger),
          std::forward<Terminator>(terminate), std::forward<Encoder>(encoder));
  }

  template <class Algorithm, class Loss, class Sampler1, class Sampler2,
            class Logger, class Terminator, class Encoder>
  void solve(Algorithm *alg, Loss &&loss,
             utility::sampler::detail::component_sampler_t, Sampler1 &&,
             const index_t, utility::sampler::detail::coordinate_sampler_t,
             Sampler2 &&, const index_t, Logger &&logger,
             Terminator &&terminate, Encoder &&encoder) {
    solve(alg, std::forward<Loss>(loss), std::forward<Logger>(logger),
          std::forward<Terminator>(terminate), std::forward<Encoder>(encoder));
  }

  ~master() = default;

  value_t getf() const { return 0; }
  std::vector<value_t> getx() const {
    return std::vector<value_t>(std::begin(x), std::end(x));
  }

private:
  int linger;
  long timeout;
  std::string maddress, saddress;
  std::uint16_t spub, smaster, mworker;
  index_t startind, k{1};
  value_t *xb, *gb, *ge;
  const value_t *xb_c, *xe_c, *gb_c, *ge_c;
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

    xb = x.data();
    xb_c = xb;
    gb = g.data();
    gb_c = gb;
    ge_c = gb_c + g.size();

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

#ifdef POLO_WITH_CURL
    communicator::ip myip = communicator::ip::getexternal();
    address = "tcp://" + myip.get();
#endif

    msg.addpart('r');
    msg.send(request);

    poll.additem(request, communicator::zmq::poll_event::pollin);

    if (poll.poll(timeout) == 0)
      throw std::runtime_error(address + ": No response from " + saddress +
                               ":" + std::to_string(sworker) + " for " +
                               std::to_string(timeout) + " ms.");

    msg.receive(request);
    if (msg.numparts() != 2 || msg.read<char>(0) != 'r')
      throw std::runtime_error(address + ": Wrong message from " + saddress +
                               ":" + std::to_string(sworker) + ".");

    detail::deserialize(msg, 1, wid);

    poll.clear();
    poll.additem(subscription, communicator::zmq::poll_event::pollin);
    poll.additem(request, communicator::zmq::poll_event::pollin);

    return x;
  }

  template <class Algorithm, class Loss, class Logger, class Terminator,
            class Encoder>
  void solve(Algorithm *, Loss &&loss, Logger &&, Terminator &&,
             Encoder &&encoder) {
    auto f = [&]() {
      fval = std::forward<Loss>(loss)(xb_c, gb);
      return std::forward<Encoder>(encoder)(gb_c, ge_c);
    };
    kernel<typename std::decay<Encoder>::type>(f, nullptr);
  }

  template <class Algorithm, class Loss, class Sampler, class Logger,
            class Terminator, class Encoder>
  void solve(Algorithm *, Loss &&loss,
             utility::sampler::detail::component_sampler_t, Sampler &&sampler,
             const index_t num_components, Logger &&, Terminator &&,
             Encoder &&encoder) {
    std::vector<index_t> components(num_components);
    index_t *cb = components.data();
    index_t *ce = cb + num_components;
    const index_t *cb_c = cb;
    const index_t *ce_c = ce;

    auto f = [&, cb, ce, cb_c, ce_c]() {
      std::forward<Sampler>(sampler)(cb, ce);
      fval = std::forward<Loss>(loss)(xb_c, gb, cb_c, ce_c);
      return std::forward<Encoder>(encoder)(gb_c, ge_c);
    };
    kernel<typename std::decay<Encoder>::type>(f, nullptr);
  }

  template <class Algorithm, class Loss, class Sampler, class Logger,
            class Terminator, class Encoder>
  void solve(Algorithm *, Loss &&loss,
             utility::sampler::detail::coordinate_sampler_t, Sampler &&sampler,
             const index_t num_coordinates, Logger &&, Terminator &&,
             Encoder &&encoder) {
    std::vector<index_t> coordinates(num_coordinates);
    index_t *cb = coordinates.data();
    index_t *ce = cb + num_coordinates;
    const index_t *cb_c = cb;
    const index_t *ce_c = ce;

    auto f = [&, cb, ce, cb_c, ce_c]() {
      fval = std::forward<Loss>(loss)(xb_c, gb);
      std::forward<Sampler>(sampler)(cb, ce);
      return std::forward<Encoder>(encoder)(gb_c, ge_c, cb_c, ce_c);
    };
    kernel<typename std::decay<Encoder>::type>(f, &coordinates);
  }

  template <class Algorithm, class Loss, class Sampler1, class Sampler2,
            class Logger, class Terminator, class Encoder>
  void solve(Algorithm *, Loss &&loss,
             utility::sampler::detail::component_sampler_t, Sampler1 &&sampler1,
             const index_t num_components,
             utility::sampler::detail::coordinate_sampler_t,
             Sampler2 &&sampler2, const index_t num_coordinates, Logger &&,
             Terminator &&, Encoder &&encoder) {
    std::vector<index_t> components(num_components);
    index_t *compb = components.data();
    index_t *compe = compb + components.size();
    const index_t *compb_c = compb;
    const index_t *compe_c = compe;

    std::vector<index_t> coordinates(num_coordinates);
    index_t *coorb = coordinates.data();
    index_t *coore = coorb + coordinates.size();
    const index_t *coorb_c = coorb;
    const index_t *coore_c = coore;

    auto f = [&, compb, compe, compb_c, compe_c, coorb, coore, coorb_c,
              coore_c]() {
      std::forward<Sampler1>(sampler1)(compb, compe);
      fval = std::forward<Loss>(loss)(xb_c, gb, compb_c, compe_c);
      std::forward<Sampler2>(sampler2)(coorb, coore);
      return std::forward<Encoder>(encoder)(gb_c, ge_c, coorb_c, coore_c);
    };
    kernel<typename std::decay<Encoder>::type>(f, &coordinates);
  }

  value_t getf() const { return fval; }
  std::vector<value_t> getx() const {
    return std::vector<value_t>(std::begin(x), std::end(x));
  }

  ~worker() = default;

private:
  void askfor(const char tag, const std::vector<index_t> *indices) {
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

  template <class Encoder> void send(const Encoder &encoder) {
    index_t indstart{0}, indend;
    std::vector<std::future<void>> futures;

    const auto wdata = detail::serialize(wid);
    const auto kdata = detail::serialize(k);
    const auto fdata = detail::serialize(fval);

    for (std::size_t pid = 1; pid < msg.numparts(); pid += 2) {
      detail::deserialize(msg, pid, indend);

      auto edata = detail::serialize(encoder.slice(indstart, indend));

      std::promise<void> promise;
      futures.push_back(promise.get_future());

      std::thread(
          [=](const std::string address, const std::string edata,
              std::promise<void> promise) {
            communicator::zmq::message msg;
            msg.addpart('g');
            msg.addpart(std::begin(wdata), std::end(wdata));
            msg.addpart(std::begin(kdata), std::end(kdata));
            msg.addpart(std::begin(fdata), std::end(fdata));
            msg.addpart(std::begin(edata), std::end(edata));

            communicator::zmq::socket req(ctx,
                                          communicator::zmq::socket_type::req);
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
          msg.read(pid + 1), std::move(edata), std::move(promise))
          .detach();
    }

    for (const auto &future : futures)
      future.wait();

    for (auto &future : futures)
      future.get();
  }

  template <class Encoder, class Function>
  void kernel(Function &&f, const std::vector<index_t> *indices) {
    typename Encoder::result_type enc;

    askfor('x', nullptr);

    while (poll.poll(timeout) > 0) {
      if (poll[0].isready()) {
        msg.receive(subscription);
        if (msg.size(1) == 1 && msg.read<char>(1) == 'T')
          break;
      }

      if (poll[1].isready()) {
        msg.receive(request);
        if (msg.size() == 0) {
          askfor('x', nullptr);
        } else {
          const char tag = msg.read<char>(0);
          if (tag == 'u')
            askfor('x', nullptr);
          else if (tag == 'x') {
            try {
              receive_x();
              enc = std::forward<Function>(f)();
              askfor('g', indices);
            } catch (...) {
              ping();
            }
          } else if (tag == 'g') {
            try {
              send(enc);
              ack();
            } catch (...) {
              ping();
            }
          }
        }
      }
    }
  }

  int linger;
  long timeout;
  std::string saddress;
  std::uint16_t spub, sworker;
  index_t wid, k;
  value_t fval;
  value_t *xb, *gb;
  const value_t *xb_c, *gb_c, *ge_c;
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

  template <class Algorithm, class Loss, class Logger, class Terminator,
            class Encoder>
  void solve(Algorithm *alg, Loss &&loss, Logger &&logger,
             Terminator &&terminate, Encoder &&) {
    std::string data;
    std::vector<index_t> indices;
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

        if (tag == 'r') {
          data = detail::serialize(wid++);
          msg.addpart(std::begin(data), std::end(data));
          msg.send(worker);
        } else if (tag == 'u') {
          k++;
          msg.send(worker);
          msg.clear();
          msg.addpart('M');
          data = detail::serialize(k);
          msg.addpart(std::begin(data), std::end(data));
          msg.send(publisher);
        } else if (tag == 'x' || tag == 'g') {
          detail::deserialize(msg, pid, indices);
          msg.pop_back();

          if (tag == 'x') {
            data = paramserver::detail::serialize(k);
            msg.addpart(std::begin(data), std::end(data));
          }

          for (const auto &pair : datadist) {
            if (pair.first.first <= indices[1] &&
                pair.first.second > indices[0]) {
              data = paramserver::detail::serialize(pair.first.second);
              msg.addpart(std::begin(data), std::end(data));
              msg.addpart(std::begin(pair.second), std::end(pair.second));
            }
          }

          msg.send(worker);
        }
      }

      if (poll[0].isready()) {
        // TODO: Implement handling of termination requests from masters
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

  template <class Algorithm, class Loss, class Space, class Sampler,
            class Logger, class Terminator, class Encoder>
  void solve(Algorithm *alg, Loss &&loss, Space, Sampler &&, const index_t,
             Logger &&logger, Terminator &&terminate, Encoder &&encoder) {
    solve(alg, std::forward<Loss>(loss), std::forward<Logger>(logger),
          std::forward<Terminator>(terminate), std::forward<Encoder>(encoder));
  }

  template <class Algorithm, class Loss, class Sampler1, class Sampler2,
            class Logger, class Terminator, class Encoder>
  void solve(Algorithm *alg, Loss &&loss,
             utility::sampler::detail::component_sampler_t, Sampler1 &&,
             const index_t, utility::sampler::detail::coordinate_sampler_t,
             Sampler2 &&, const index_t, Logger &&logger,
             Terminator &&terminate, Encoder &&encoder) {
    solve(alg, std::forward<Loss>(loss), std::forward<Logger>(logger),
          std::forward<Terminator>(terminate), std::forward<Encoder>(encoder));
  }

  value_t getf() const { return 0; }
  std::vector<value_t> getx() const { return {}; }

  ~scheduler() = default;

private:
  int linger;
  long timeout;
  std::int32_t nmasters;
  std::uint16_t ppub, pmaster, pworker;
  index_t wid{0}, k{1};
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
