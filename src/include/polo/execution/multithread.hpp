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

  template <class Algorithm, class Loss, class Logger, class Terminator,
            class Encoder>
  void solve(Algorithm *alg, Loss &&loss, Logger &&logger,
             Terminator &&terminate, Encoder &&encoder) {
    auto task = [&, alg, encoder](const index_t wid) {
      kernel(alg, wid, std::forward<Loss>(loss), std::forward<Logger>(logger),
             std::forward<Terminator>(terminate), encoder);
    };
    run_in_parallel(task);
  }

  template <class Algorithm, class Loss, class Space, class Sampler,
            class Logger, class Terminator, class Encoder>
  void solve(Algorithm *alg, Loss &&loss, Space s, Sampler &&sampler,
             const index_t num, Logger &&logger, Terminator &&terminate,
             Encoder &&encoder) {
    auto task = [&, alg, s, sampler, num, encoder](const index_t wid) {
      kernel(alg, wid, std::forward<Loss>(loss), s, sampler, num,
             std::forward<Logger>(logger), std::forward<Terminator>(terminate),
             encoder);
    };
    run_in_parallel(task);
  }

  template <class Algorithm, class Loss, class Sampler1, class Sampler2,
            class Logger, class Terminator, class Encoder>
  void solve(Algorithm *alg, Loss &&loss,
             utility::sampler::detail::component_sampler_t s1,
             Sampler1 &&sampler1, const index_t num_components,
             utility::sampler::detail::coordinate_sampler_t s2,
             Sampler2 &&sampler2, const index_t num_coordinates,
             Logger &&logger, Terminator &&terminate, Encoder &&encoder) {
    auto task = [&, alg, s1, sampler1, num_components, s2, sampler2,
                 num_coordinates, encoder](const index_t wid) {
      kernel(alg, wid, std::forward<Loss>(loss), s1, sampler1, num_components,
             s2, sampler2, num_coordinates, std::forward<Logger>(logger),
             std::forward<Terminator>(terminate), encoder);
    };
    run_in_parallel(task);
  }

  value_t getf() const { return fval; }
  std::vector<value_t> getx() const {
    return std::vector<value_t>(std::begin(x), std::end(x));
  }

  ~multithread() = default;

private:
  template <class Task> void run_in_parallel(Task task) {
    std::vector<std::thread> workers(nthreads);
    index_t wid = 0;
    for (auto &worker : workers)
      worker = std::thread(task, wid++);
    for (auto &worker : workers)
      worker.join();
  }

  template <class Algorithm, class Loss, class Logger, class Terminator,
            class Encoder>
  void kernel(Algorithm *alg, const index_t wid, Loss &&loss, Logger &&logger,
              Terminator &&terminate, Encoder encoder) {
    value_t flocal;
    const std::size_t dim = x.size();

    std::vector<value_t> xlocal(dim);
    const value_t *xb_c = xlocal.data();

    std::vector<value_t> glocal(dim);
    value_t *gb = glocal.data();
    value_t *ge = gb + dim;
    const value_t *gb_c = gb;
    const value_t *ge_c = ge;

    for (;;) {
      const index_t klocal = k;
      read(xlocal, std::integral_constant<bool, consistent>{});
      flocal = std::forward<Loss>(loss)(xb_c, gb);
      auto enc = encoder(gb_c, ge_c);
      enc(gb, ge);
      if (!update(alg, wid, klocal, flocal, glocal,
                  std::forward<Terminator>(terminate),
                  std::forward<Logger>(logger),
                  std::integral_constant<bool, consistent>{}))
        break;
    }
  }

  template <class Algorithm, class Loss, class Sampler, class Logger,
            class Terminator, class Encoder>
  void kernel(Algorithm *alg, const index_t wid, Loss &&loss,
              utility::sampler::detail::component_sampler_t, Sampler sampler,
              const index_t num_components, Logger &&logger,
              Terminator &&terminate, Encoder encoder) {
    value_t flocal;
    const std::size_t dim = x.size();

    std::vector<value_t> xlocal(dim);
    const value_t *xb_c = xlocal.data();

    std::vector<value_t> glocal(dim);
    value_t *gb = glocal.data();
    value_t *ge = gb + dim;
    const value_t *gb_c = gb;
    const value_t *ge_c = ge;

    std::vector<index_t> components(num_components);
    index_t *cb = components.data();
    index_t *ce = cb + num_components;
    const index_t *cb_c = cb;
    const index_t *ce_c = ce;

    for (;;) {
      const index_t klocal = k;
      read(xlocal, std::integral_constant<bool, consistent>{});
      sampler(cb, ce);
      flocal = std::forward<Loss>(loss)(xb_c, gb, cb_c, ce_c);
      auto enc = encoder(gb_c, ge_c);
      enc(gb, ge);
      if (!update(alg, wid, klocal, flocal, glocal,
                  std::forward<Terminator>(terminate),
                  std::forward<Logger>(logger),
                  std::integral_constant<bool, consistent>{}))
        break;
    }
  }

  template <class Algorithm, class Loss, class Sampler, class Logger,
            class Terminator, class Encoder>
  void kernel(Algorithm *alg, const index_t wid, Loss &&loss,
              utility::sampler::detail::coordinate_sampler_t, Sampler sampler,
              const index_t num_coordinates, Logger &&logger,
              Terminator &&terminate, Encoder encoder) {
    value_t flocal;
    const std::size_t dim = x.size();

    std::vector<value_t> xlocal(dim);
    const value_t *xb_c = xlocal.data();

    std::vector<value_t> glocal(dim);
    value_t *gb = glocal.data();
    value_t *ge = gb + dim;
    const value_t *gb_c = gb;
    const value_t *ge_c = ge;

    std::vector<index_t> coordinates(num_coordinates);
    index_t *cb = coordinates.data();
    index_t *ce = cb + num_coordinates;
    const index_t *cb_c = cb;
    const index_t *ce_c = ce;

    for (;;) {
      const index_t klocal = k;
      read(xlocal, std::integral_constant<bool, consistent>{});
      flocal = std::forward<Loss>(loss)(xb_c, gb);
      sampler(cb, ce);
      auto enc = encoder(gb_c, ge_c, cb_c, ce_c);
      enc(gb, ge);
      if (!update(alg, wid, klocal, flocal, glocal,
                  std::forward<Terminator>(terminate),
                  std::forward<Logger>(logger),
                  std::integral_constant<bool, consistent>{}))
        break;
    }
  }

  template <class Algorithm, class Loss, class Sampler1, class Sampler2,
            class Logger, class Terminator, class Encoder>
  void kernel(Algorithm *alg, const index_t wid, Loss &&loss,
              utility::sampler::detail::component_sampler_t, Sampler1 sampler1,
              const index_t num_components,
              utility::sampler::detail::coordinate_sampler_t, Sampler2 sampler2,
              const index_t num_coordinates, Logger &&logger,
              Terminator &&terminate, Encoder encoder) {
    value_t flocal;
    const std::size_t dim = x.size();

    std::vector<value_t> xlocal(dim);
    const value_t *xb_c = xlocal.data();

    std::vector<value_t> glocal(dim);
    value_t *gb = glocal.data();
    value_t *ge = gb + dim;
    const value_t *gb_c = gb;
    const value_t *ge_c = ge;

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

    for (;;) {
      const index_t klocal = k;
      read(xlocal, std::integral_constant<bool, consistent>{});
      sampler1(compb, compe);
      flocal = std::forward<Loss>(loss)(xb_c, gb, compb_c, compe_c);
      sampler2(coorb, coore);
      auto enc = encoder(gb_c, ge_c, coorb_c, coore_c);
      enc(gb, ge);
      if (!update(alg, wid, klocal, flocal, glocal,
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
  bool update(Algorithm *alg, const index_t wid, const index_t klocal,
              const value_t flocal, const std::vector<value_t> &glocal,
              Terminator &&terminate, Logger &&logger, std::false_type) {
    internal_value *xb = x.data();
    const internal_value *xb_c = xb;
    const internal_value *xe_c = xb_c + x.size();

    internal_value *gb = g.data();
    const internal_value *gb_c = gb;

    const value_t *glb = glocal.data();
    const value_t *gle = glb + glocal.size();

    fval = flocal;

    if (std::forward<Terminator>(terminate)(k, fval, xb_c, xe_c, gb_c))
      return false;

    alg->boost(wid, klocal, k, glb, gle, gb);
    alg->smooth(klocal, k, xb_c, xe_c, gb_c, gb);
    const value_t step = alg->step(klocal, k, fval, xb_c, xe_c, gb_c);
    alg->prox(step, xb_c, xe_c, gb_c, xb);
    std::forward<Logger>(logger)(k, fval, xb_c, xe_c, gb_c);
    k++;
    return true;
  }
  template <class Algorithm, class Terminator, class Logger>
  bool update(Algorithm *alg, const index_t wid, const index_t klocal,
              const value_t flocal, const std::vector<value_t> &glocal,
              Terminator &&terminate, Logger &&logger, std::true_type) {
    std::lock_guard<std::mutex> lock(sync);
    return update(alg, wid, klocal, flocal, glocal,
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
