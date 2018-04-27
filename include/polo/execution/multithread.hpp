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
template <class value_t, bool consistent = true> struct select_type {
  using type = value_t;
};
template <class value_t> struct select_type<value_t, false> {
  using type = std::atomic<value_t>;
};
template <> struct select_type<float, false> {
  using type = utility::atomic_float;
};
template <> struct select_type<double, false> {
  using type = utility::atomic_double;
};

template <class value_t, class index_t, bool consistent> struct multithread {
  using internal_value = typename select_type<value_t, consistent>::type;
  using internal_idx = typename select_type<index_t, consistent>::type;

  multithread() = default;

  multithread(const multithread &) = default;
  multithread &operator=(const multithread &) = default;
  multithread(multithread &&) = default;
  multithread &operator=(multithread &&) = default;

protected:
  void params(const unsigned int nthreads) { this->nthreads = nthreads; }

  template <class InputIt>
  std::vector<value_t> initialize(InputIt xbegin, InputIt xend) {
    x = std::vector<internal_value>(xbegin, xend);
    g = std::vector<internal_value>(x.size());
    return std::vector<value_t>(std::begin(x), std::end(x));
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
             Logger &&logger, Space s, Sampler &&sampler, const index_t num) {
    auto task = [&, alg, s, sampler, num]() {
      kernel(alg, std::forward<Loss>(loss), std::forward<Terminator>(terminate),
             std::forward<Logger>(logger), s, sampler, num);
    };
    run_in_parallel(task);
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Sampler1, class Sampler2>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger, utility::sampler::detail::component_sampler_t s1,
             Sampler1 &&sampler1, const index_t num_components,
             utility::sampler::detail::coordinate_sampler_t s2,
             Sampler2 &&sampler2, const index_t num_coordinates) {
    auto task = [&, alg, s1, sampler1, num_components, s2, sampler2,
                 num_coordinates]() {
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
      read(xlocal, std::integral_constant<bool, consistent>{});
      flocal = std::forward<Loss>(loss)(xbegin, &glocal[0]);
      if (!update(alg, flocal, glocal, std::forward<Terminator>(terminate),
                  std::forward<Logger>(logger),
                  std::integral_constant<bool, consistent>{}))
        break;
    }
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Sampler>
  void kernel(Algorithm *alg, Loss &&loss, Terminator &&terminate,
              Logger &&logger, utility::sampler::detail::component_sampler_t,
              Sampler sampler, const index_t num_components) {
    value_t flocal;
    const std::size_t dim{x.size()};

    std::vector<value_t> xlocal(dim), glocal(dim);
    std::vector<index_t> components(num_components);

    const value_t *xbegin{&xlocal[0]};

    for (;;) {
      read(xlocal, std::integral_constant<bool, consistent>{});
      sampler(std::begin(components), std::end(components));
      flocal = std::forward<Loss>(loss)(xbegin, &glocal[0], &components[0],
                                        &components[num_components]);
      if (!update(alg, flocal, glocal, std::forward<Terminator>(terminate),
                  std::forward<Logger>(logger),
                  std::integral_constant<bool, consistent>{}))
        break;
    }
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Sampler>
  void kernel(Algorithm *alg, Loss &&loss, Terminator &&terminate,
              Logger &&logger, utility::sampler::detail::coordinate_sampler_t,
              Sampler sampler, const index_t num_coordinates) {
    value_t flocal;
    const std::size_t dim{x.size()};

    std::vector<value_t> xlocal(dim), partial(num_coordinates);
    std::vector<index_t> coordinates(num_coordinates);

    const value_t *xbegin{&xlocal[0]};

    for (;;) {
      read(xlocal, std::integral_constant<bool, consistent>{});
      sampler(std::begin(coordinates), std::end(coordinates));
      flocal = std::forward<Loss>(loss)(xbegin, &partial[0], &coordinates[0],
                                        &coordinates[num_coordinates]);
      if (!update(alg, flocal, partial, coordinates,
                  std::forward<Terminator>(terminate),
                  std::forward<Logger>(logger),
                  std::integral_constant<bool, consistent>{}))
        break;
    }
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Sampler1, class Sampler2>
  void kernel(Algorithm *alg, Loss &&loss, Terminator &&terminate,
              Logger &&logger, utility::sampler::detail::component_sampler_t,
              Sampler1 sampler1, const index_t num_components,
              utility::sampler::detail::coordinate_sampler_t, Sampler2 sampler2,
              const index_t num_coordinates) {
    value_t flocal;
    const std::size_t dim{x.size()};

    std::vector<value_t> xlocal(dim), partial(num_coordinates);
    std::vector<index_t> components(num_components),
        coordinates(num_coordinates);

    const value_t *xbegin{&xlocal[0]};

    for (;;) {
      read(xlocal, std::integral_constant<bool, consistent>{});
      sampler1(std::begin(components), std::end(components));
      sampler2(std::begin(coordinates), std::end(coordinates));
      flocal = std::forward<Loss>(loss)(
          xbegin, &partial[0], &components[0], &components[num_components],
          &coordinates[0], &coordinates[num_coordinates]);
      if (!update(alg, flocal, partial, coordinates,
                  std::forward<Terminator>(terminate),
                  std::forward<Logger>(logger),
                  std::integral_constant<bool, consistent>{}))
        break;
    }
  }

  void read(std::vector<value_t> &xlocal, std::false_type) {
    std::copy(std::begin(x), std::end(x), std::begin(xlocal));
  }
  void read(std::vector<value_t> &xlocal, std::true_type) {
    std::lock_guard<std::mutex> lock(sync);
    read(xlocal, std::false_type{});
  }

  template <class Algorithm, class Terminator, class Logger>
  bool update(Algorithm *alg, const value_t flocal,
              const std::vector<value_t> &glocal, Terminator &&terminate,
              Logger &&logger, std::false_type) {
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
  template <class Algorithm, class Terminator, class Logger>
  bool update(Algorithm *alg, const value_t flocal,
              std::vector<value_t> &glocal, Terminator &&terminate,
              Logger &&logger, std::true_type) {
    std::lock_guard<std::mutex> lock(sync);
    return update(alg, flocal, glocal, std::forward<Terminator>(terminate),
                  std::forward<Logger>(logger), std::false_type{});
  }

  template <class Algorithm, class Terminator, class Logger>
  bool update(Algorithm *alg, const value_t flocal,
              const std::vector<value_t> &partial,
              const std::vector<index_t> &coordinates, Terminator &&terminate,
              Logger &&logger, std::false_type) {
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
  template <class Algorithm, class Terminator, class Logger>
  bool update(Algorithm *alg, const value_t flocal,
              const std::vector<value_t> &partial,
              const std::vector<index_t> &coordinates, Terminator &&terminate,
              Logger &&logger, std::true_type) {
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

template <class value_t, class index_t>
using consistent = detail::multithread<value_t, index_t, true>;
template <class value_t, class index_t>
using inconsistent = detail::multithread<value_t, index_t, false>;
} // namespace execution
} // namespace polo

#endif
