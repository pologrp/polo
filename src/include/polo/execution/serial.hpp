#ifndef POLO_EXECUTION_SERIAL_HPP_
#define POLO_EXECUTION_SERIAL_HPP_

#include <utility>
#include <vector>

#include "polo/utility/sampler.hpp"

namespace polo {
namespace execution {
template <class value_t, class index_t> struct serial {
  serial() = default;

  serial(const serial &) = default;
  serial &operator=(const serial &) = default;
  serial(serial &&) = default;
  serial &operator=(serial &&) = default;

protected:
  template <class InputIt>
  std::vector<value_t> initialize(InputIt xbegin, InputIt xend) {
    k = 1;
    fval = 0
    x = std::vector<value_t>(xbegin, xend);
    g = std::vector<value_t>(x.size());
    return x;
  }

  template <class Algorithm, class Loss, class Terminator, class Logger>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger) {
    const value_t *xb_c = x.data();
    const value_t *xe_c = xb_c + x.size();

    value_t *gb = g.data();
    const value_t *gb_c = gb;

    fval = std::forward<Loss>(loss)(xb_c, gb);
    while (!std::forward<Terminator>(terminate)(k, fval, xb_c, xe_c, gb_c)) {
      iterate(alg, std::forward<Logger>(logger));
      fval = std::forward<Loss>(loss)(xb_c, gb);
    }
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Sampler>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger, utility::sampler::detail::component_sampler_t,
             Sampler &&sampler, const index_t num_components) {
    const value_t *xb_c = x.data();
    const value_t *xe_c = xb_c + x.size();

    value_t *gb = g.data();
    const value_t *gb_c = gb;

    std::vector<index_t> components(num_components);
    index_t *cb = components.data();
    index_t *ce = cb + components.size();
    const index_t *cb_c = cb;
    const index_t *ce_c = ce;

    while (!std::forward<Terminator>(terminate)(k, fval, xb_c, xe_c, gb_c)) {
      std::forward<Sampler>(sampler)(cb, ce);
      fval = std::forward<Loss>(loss)(xb_c, gb, cb_c, ce_c);
      iterate(alg, std::forward<Logger>(logger));
    }
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Sampler>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger, utility::sampler::detail::coordinate_sampler_t,
             Sampler &&sampler, const index_t num_coordinates) {
    const value_t *xb_c = x.data();
    const value_t *xe_c = xb_c + x.size();

    const value_t *gb_c = g.data();

    std::vector<index_t> coordinates(num_coordinates);
    index_t *cb = coordinates.data();
    index_t *ce = cb + coordinates.size();
    const index_t *cb_c = cb;
    const index_t *ce_c = ce;

    std::vector<value_t> partial(num_coordinates);
    value_t *pb = partial.data();

    while (!std::forward<Terminator>(terminate)(k, fval, xb_c, xe_c, gb_c)) {
      std::forward<Sampler>(sampler)(cb, ce);
      fval = std::forward<Loss>(loss)(xb_c, pb, cb_c, ce_c);
      for (auto &val : g)
        val = 0;
      for (std::size_t idx = 0; idx < num_coordinates; idx++)
        g[coordinates[idx]] = partial[idx];
      iterate(alg, std::forward<Logger>(logger));
    }
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Sampler1, class Sampler2>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger, utility::sampler::detail::component_sampler_t,
             Sampler1 &&sampler1, const index_t num_components,
             utility::sampler::detail::coordinate_sampler_t,
             Sampler2 &&sampler2, const index_t num_coordinates) {
    const value_t *xb_c = x.data();
    const value_t *xe_c = xb_c + x.size();

    const value_t *gb_c = g.data();

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

    std::vector<value_t> partial(num_coordinates);
    value_t *pb = partial.data();

    while (!std::forward<Terminator>(terminate)(k, fval, xb_c, xe_c, gb_c)) {
      std::forward<Sampler1>(sampler1)(compb, compe);
      std::forward<Sampler2>(sampler2)(coorb, coore);
      fval = std::forward<Loss>(loss)(xb_c, pb, compb_c, compe_c, coorb_c,
                                      coore_c);
      for (auto &val : g)
        val = 0;
      for (std::size_t idx = 0; idx < num_coordinates; idx++)
        g[coordinates[idx]] = partial[idx];
      iterate(alg, std::forward<Logger>(logger));
    }
  }

  value_t getf() const { return fval; }
  std::vector<value_t> getx() const { return x; }

  ~serial() = default;

private:
  template <class Algorithm, class Logger>
  void iterate(Algorithm *alg, Logger &&logger) {
    value_t *xb = x.data();
    const value_t *xb_c = xb;
    const value_t *xe_c = xb_c + x.size();

    value_t *gb = g.data();
    const value_t *gb_c = gb;
    const value_t *ge_c = gb_c + g.size();

    alg->boost(index_t(0), k, k, gb_c, ge_c, gb);
    alg->smooth(k, k, xb_c, xe_c, gb_c, gb);
    const value_t step = alg->step(k, k, fval, xb_c, xe_c, gb_c);
    alg->prox(step, xb_c, xe_c, gb_c, xb);
    std::forward<Logger>(logger)(k, fval, xb_c, xe_c, gb_c);
    k++;
  }

  index_t k{1};
  value_t fval{0};
  std::vector<value_t> x, g;
};
} // namespace execution
} // namespace polo

#endif
