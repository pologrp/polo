#ifndef POLO_EXECUTION_MULTITHREAD_HPP_
#define POLO_EXECUTION_MULTITHREAD_HPP_

#include <algorithm>
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
  void parameters(const unsigned int nthreads) { this->nthreads = nthreads; }

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
    const std::size_t dim = x.size();

    std::vector<value_t> xlocal(dim);
    const value_t *xb_c = xlocal.data();

    std::vector<value_t> glocal(dim);
    value_t *glb = glocal.data();

    for (;;) {
      read(xlocal, std::integral_constant<bool, consistent>{});
      flocal = std::forward<Loss>(loss)(xb_c, glb);
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
    const std::size_t dim = x.size();

    std::vector<value_t> xlocal(dim), glocal(dim);
    std::vector<index_t> components(num_components);
    index_t *cb = components.data();
    index_t *ce = cb + num_components;
    const index_t *cb_c = components.data();
    const index_t *ce_c = cb_c + num_components;

    const value_t *xb_c = xlocal.data();
    value_t *glb = glocal.data();

    for (;;) {
      read(xlocal, std::integral_constant<bool, consistent>{});
      sampler(cb, ce);
      std::sort(cb, ce);
      flocal = std::forward<Loss>(loss)(xb_c, glb, cb_c, ce_c);
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
    const std::size_t dim = x.size();

    std::vector<value_t> xlocal(dim), partial(num_coordinates);
    std::vector<index_t> coordinates(num_coordinates);
    index_t *cb = coordinates.data();
    index_t *ce = cb + num_coordinates;
    const index_t *cb_c = coordinates.data();
    const index_t *ce_c = cb_c + num_coordinates;

    const value_t *xb_c = xlocal.data();
    value_t *pb = partial.data();

    for (;;) {
      read(xlocal, std::integral_constant<bool, consistent>{});
      sampler(cb, ce);
      std::sort(cb, ce);
      flocal = std::forward<Loss>(loss)(xb_c, pb, cb_c, ce_c);
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
    std::vector<index_t> components(num_components);
    index_t *compb = components.data();
    index_t *compe = compb + components.size();
    const index_t *compb_c = components.data();
    const index_t *compe_c = compb_c + components.size();

    std::vector<index_t> coordinates(num_coordinates);
    index_t *coorb = coordinates.data();
    index_t *coore = coorb + coordinates.size();
    const index_t *coorb_c = coordinates.data();
    const index_t *coore_c = coorb_c + coordinates.size();

    const value_t *xb_c = xlocal.data();
    value_t *pb = partial.data();

    for (;;) {
      read(xlocal, std::integral_constant<bool, consistent>{});
      sampler1(compb, compe);
      std::sort(compb, compe);
      sampler2(coorb, coore);
      std::sort(coorb, coore);
      flocal = std::forward<Loss>(loss)(xb_c, pb, compb_c, compe_c, coorb_c,
                                        coore_c);
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
    const value_t *glb = glocal.data();
    const value_t *gle = glb + glocal.size();

    internal_value *gb = g.data();
    const internal_value *gb_c = g.data();

    internal_value *xb = x.data();
    const internal_value *xb_c = x.data();
    const internal_value *xe_c = xb_c + x.size();

    fval = flocal;

    if (std::forward<Terminator>(terminate)(k, fval, xb_c, xe_c, gb_c))
      return false;

    alg->boost(glb, gle, gb);
    alg->smooth(k, xb_c, xe_c, gb_c, gb);
    const auto step = alg->step(k, fval, xb_c, xe_c, gb_c);
    alg->prox(step, xb_c, xe_c, gb_c, xb);
    std::forward<Logger>(logger)(k, fval, xb_c, xe_c, gb_c);
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
    internal_value *gb = g.data();
    const internal_value *gb_c = g.data();
    const internal_value *ge_c = gb_c + g.size();

    internal_value *xb = x.data();
    const internal_value *xb_c = x.data();
    const internal_value *xe_c = xb_c + x.size();

    fval = flocal;

    if (std::forward<Terminator>(terminate)(k, fval, xb_c, xe_c, gb_c))
      return false;

    for (std::size_t idx = 0; idx < coordinates.size(); idx++)
      g[coordinates[idx]] = partial[idx];

    alg->boost(gb_c, ge_c, gb);
    alg->smooth(k, xb_c, xe_c, gb_c, gb);
    const auto step = alg->step(k, fval, xb_c, xe_c, gb_c);
    alg->prox(step, xb_c, xe_c, gb_c, xb);
    std::forward<Logger>(logger)(k, fval, xb_c, xe_c, gb_c);
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
