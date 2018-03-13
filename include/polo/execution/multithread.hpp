#ifndef MULTITHREAD_HPP_
#define MULTITHREAD_HPP_

#include <atomic>
#include <iterator>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include "polo/utility/atomic.hpp"
#include "polo/utility/sampler.hpp"

namespace polo {
namespace execution {
namespace detail {
template <class value_t> struct select_atomic;

template <> struct select_atomic<float> { using type = utility::atomic_float; };
template <> struct select_atomic<double> {
  using type = utility::atomic_double;
};

template <class value_t, bool consistent> struct multithread {
  using internal_value =
      typename std::conditional<consistent, value_t,
                                typename select_atomic<value_t>::type>::type;
  using internal_idx =
      typename std::conditional<consistent, std::size_t,
                                std::atomic<std::size_t>>::type;

  multithread() = default;

  multithread(const multithread &) = default;
  multithread &operator=(const multithread &) = default;
  multithread(multithread &&) = default;
  multithread &operator=(multithread &&) = default;

protected:
  void params(const unsigned int nthreads) { this->nthreads = nthreads; }

  template <class InputIt> void initialize(InputIt xbegin, InputIt xend) {
    x = std::vector<internal_value>(xbegin, xend);
    g = std::vector<internal_value>(x.size());
  }

  template <class Algorithm, class Loss, class Terminator, class Logger>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger) {
    auto task = [&, alg]() {
      kernel(alg, std::forward<Loss>(loss), std::forward<Terminator>(terminate),
             std::forward<Logger>(logger));
    };
    run_in_parallel(task);
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Space, class Sampler>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger, Space s, Sampler &&sampler,
             const std::size_t num) {
    auto task = [&, alg, sampler]() {
      kernel(alg, std::forward<Loss>(loss), std::forward<Terminator>(terminate),
             std::forward<Logger>(logger), s, sampler, num);
    };
    run_in_parallel(task);
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Sampler1, class Sampler2>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger, utility::sampler::detail::component_sampler_t s1,
             Sampler1 &&sampler1, const std::size_t num_components,
             utility::sampler::detail::coordinate_sampler_t s2,
             Sampler2 &&sampler2, const std::size_t num_coordinates) {
    auto task = [&, alg, sampler1, sampler2]() {
      kernel(alg, std::forward<Loss>(loss), std::forward<Terminator>(terminate),
             std::forward<Logger>(logger), s1, sampler1, num_components, s2,
             sampler2, num_coordinates);
    };
    run_in_parallel(task);
  }

  value_t getf() const { return fval; }
  std::vector<value_t> getx() const {
    return std::vector<value_t>(std::begin(x), std::end(x));
  }

  ~multithread() = default;

private:
  template <class Task> void run_in_parallel(Task &&task) {
    std::vector<std::thread> workers(nthreads);
    for (auto &worker : workers)
      worker = std::thread(task);
    for (auto &worker : workers)
      worker.join();
  }

  template <class Algorithm, class Loss, class Terminator, class Logger>
  void kernel(Algorithm *alg, Loss &&loss, Terminator &&terminate,
              Logger &&logger) {
    value_t flocal;
    const std::size_t dim{x.size()};

    std::vector<value_t> xlocal(dim);
    const value_t *xbegin{&xlocal[0]};

    std::vector<value_t> glocal(dim);

    for (;;) {
      read(xlocal, typename std::integral_constant<bool, consistent>::type{});
      flocal = std::forward<Loss>(loss)(xbegin, &glocal[0]);
      if (!update(alg, flocal, glocal, std::forward<Terminator>(terminate),
                  std::forward<Logger>(logger),
                  typename std::integral_constant<bool, consistent>::type{}))
        break;
    }
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Sampler>
  void kernel(Algorithm *alg, Loss &&loss, Terminator &&terminate,
              Logger &&logger, utility::sampler::detail::component_sampler_t,
              Sampler &&sampler, const std::size_t num_components) {
    value_t flocal;
    const std::size_t dim{x.size()};

    std::vector<value_t> xlocal(dim), glocal(dim);
    std::vector<std::size_t> components(num_components);

    const value_t *xbegin{&xlocal[0]};

    for (;;) {
      read(xlocal, typename std::integral_constant<bool, consistent>::type{});
      std::forward<Sampler>(sampler)(std::begin(components),
                                     std::end(components));
      flocal = std::forward<Loss>(loss)(xbegin, &glocal[0], &components[0],
                                        &components[num_components]);
      if (!update(alg, flocal, glocal, std::forward<Terminator>(terminate),
                  std::forward<Logger>(logger),
                  typename std::integral_constant<bool, consistent>::type{}))
        break;
    }
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Sampler>
  void kernel(Algorithm *alg, Loss &&loss, Terminator &&terminate,
              Logger &&logger, utility::sampler::detail::coordinate_sampler_t,
              Sampler &&sampler, const std::size_t num_coordinates) {
    value_t flocal;
    const std::size_t dim{x.size()};

    std::vector<value_t> xlocal(dim), partial(num_coordinates);
    std::vector<std::size_t> coordinates(num_coordinates);

    const value_t *xbegin{&xlocal[0]};

    for (;;) {
      read(xlocal, typename std::integral_constant<bool, consistent>::type{});
      std::forward<Sampler>(sampler)(std::begin(coordinates),
                                     std::end(coordinates));
      flocal = std::forward<Loss>(loss)(xbegin, &partial[0], &coordinates[0],
                                        &coordinates[num_coordinates]);
      if (!update(alg, flocal, partial, coordinates,
                  std::forward<Terminator>(terminate),
                  std::forward<Logger>(logger),
                  typename std::integral_constant<bool, consistent>::type{}))
        break;
    }
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Sampler1, class Sampler2>
  void kernel(Algorithm *alg, Loss &&loss, Terminator &&terminate,
              Logger &&logger, utility::sampler::detail::component_sampler_t,
              Sampler1 &&sampler1, const std::size_t num_components,
              utility::sampler::detail::coordinate_sampler_t,
              Sampler2 &&sampler2, const std::size_t num_coordinates) {
    value_t flocal;
    const std::size_t dim{x.size()};

    std::vector<value_t> xlocal(dim), partial(num_coordinates);
    std::vector<std::size_t> components(num_components),
        coordinates(num_coordinates);

    const value_t *xbegin{&xlocal[0]};

    for (;;) {
      read(xlocal, typename std::integral_constant<bool, consistent>::type{});
      std::forward<Sampler1>(sampler1)(std::begin(components),
                                       std::end(components));
      std::forward<Sampler2>(sampler2)(std::begin(coordinates),
                                       std::end(coordinates));
      flocal = std::forward<Loss>(loss)(
          xbegin, &partial[0], &components[0], &components[num_components],
          &coordinates[0], &coordinates[num_coordinates]);
      if (!update(alg, flocal, partial, coordinates,
                  std::forward<Terminator>(terminate),
                  std::forward<Logger>(logger),
                  typename std::integral_constant<bool, consistent>::type{}))
        break;
    }
  }

  template <class Container> void read(Container &xlocal, std::false_type) {
    std::copy(std::begin(x), std::end(x), std::begin(xlocal));
  }
  template <class Container> void read(Container &xlocal, std::true_type) {
    std::lock_guard<std::mutex> lock(sync);
    read(xlocal, std::false_type{});
  }

  template <class Algorithm, class Container, class Terminator, class Logger>
  bool update(Algorithm *alg, value_t flocal, Container &glocal,
              Terminator &&terminate, Logger &&logger, std::false_type) {
    fval = flocal;

    if (std::forward<Terminator>(terminate)(k, fval, std::begin(x), std::end(x),
                                            std::begin(g)))
      return false;

    alg->grad(std::begin(glocal), std::end(glocal), std::begin(g));
    alg->smooth(k, std::begin(x), std::end(x), std::begin(g), std::begin(g));
    auto step = alg->step(k, fval, std::begin(x), std::end(x), std::begin(g));
    alg->project(step, std::begin(x), std::end(x), std::begin(g),
                 std::begin(x));
    std::forward<Logger>(logger)(k, fval, std::begin(x), std::end(x),
                                 std::begin(g), std::end(g));
    k++;
    return true;
  }
  template <class Algorithm, class Container, class Terminator, class Logger>
  bool update(Algorithm *alg, value_t flocal, Container &glocal,
              Terminator &&terminate, Logger &&logger, std::true_type) {
    std::lock_guard<std::mutex> lock(sync);
    return update(alg, flocal, glocal, std::forward<Terminator>(terminate),
                  std::forward<Logger>(logger), std::false_type{});
  }

  template <class Algorithm, class Container1, class Container2,
            class Terminator, class Logger>
  bool update(Algorithm *alg, value_t flocal, Container1 &partial,
              Container2 &coordinates, Terminator &&terminate, Logger &&logger,
              std::false_type) {
    fval = flocal;

    if (std::forward<Terminator>(terminate)(k, fval, std::begin(x), std::end(x),
                                            std::begin(g)))
      return false;

    for (std::size_t idx = 0; idx < coordinates.size(); idx++)
      g[coordinates[idx]] = partial[idx];

    alg->grad(std::begin(g), std::end(g), std::begin(g));
    alg->smooth(k, std::begin(x), std::end(x), std::begin(g), std::begin(g));
    auto step = alg->step(k, fval, std::begin(x), std::end(x), std::begin(g));
    alg->project(step, std::begin(x), std::end(x), std::begin(g),
                 std::begin(x));
    std::forward<Logger>(logger)(k, fval, std::begin(x), std::end(x),
                                 std::begin(g), std::end(g));
    k++;
    return true;
  }
  template <class Algorithm, class Container1, class Container2,
            class Terminator, class Logger>
  bool update(Algorithm *alg, value_t flocal, Container1 &partial,
              Container2 &coordinates, Terminator &&terminate, Logger &&logger,
              std::true_type) {
    std::lock_guard<std::mutex> lock(sync);
    return update(alg, flocal, partial, coordinates,
                  std::forward<Terminator>(terminate),
                  std::forward<Logger>(logger), std::false_type{});
  }

  internal_idx k{1};
  internal_value fval{0};
  std::vector<internal_value> x, g;
  unsigned int nthreads{std::thread::hardware_concurrency()};
  std::mutex sync;
};
} // namespace detail

template <class value_t> using consistent = detail::multithread<value_t, true>;
template <class value_t>
using inconsistent = detail::multithread<value_t, false>;
} // namespace execution
} // namespace polo

#endif
