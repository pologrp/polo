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
    fval = 0;
    x = std::vector<value_t>(xbegin, xend);
    g = std::vector<value_t>(x.size());
    xb = x.data();
    xb_c = xb;
    xe_c = xb_c + x.size();
    gb = g.data();
    ge = gb + g.size();
    gb_c = gb;
    ge_c = ge;
    return x;
  }

  template <class Algorithm, class Loss, class Logger, class Terminator,
            class Encoder>
  void solve(Algorithm *alg, Loss &&loss, Logger &&logger,
             Terminator &&terminate, Encoder &&encoder) {
    fval = std::forward<Loss>(loss)(xb_c, gb);
    auto enc = std::forward<Encoder>(encoder)(gb_c, ge_c);
    enc(gb, ge);
    while (!std::forward<Terminator>(terminate)(k, fval, xb_c, xe_c, gb_c)) {
      iterate(alg, std::forward<Logger>(logger));
      fval = std::forward<Loss>(loss)(xb_c, gb);
      enc = std::forward<Encoder>(encoder)(gb_c, ge_c);
      enc(gb, ge);
    }
  }

  template <class Algorithm, class Loss, class Sampler, class Logger,
            class Terminator, class Encoder>
  void solve(Algorithm *alg, Loss &&loss,
             utility::sampler::detail::component_sampler_t, Sampler &&sampler,
             const index_t num_components, Logger &&logger,
             Terminator &&terminate, Encoder &&encoder) {
    std::vector<index_t> components(num_components);
    index_t *cb = components.data();
    index_t *ce = cb + components.size();
    const index_t *cb_c = cb;
    const index_t *ce_c = ce;

    std::forward<Sampler>(sampler)(cb, ce);
    fval = std::forward<Loss>(loss)(xb_c, gb, cb_c, ce_c);
    auto enc = std::forward<Encoder>(encoder)(gb_c, ge_c);
    enc(gb, ge);
    while (!std::forward<Terminator>(terminate)(k, fval, xb_c, xe_c, gb_c)) {
      iterate(alg, std::forward<Logger>(logger));
      std::forward<Sampler>(sampler)(cb, ce);
      fval = std::forward<Loss>(loss)(xb_c, gb, cb_c, ce_c);
      enc = std::forward<Encoder>(encoder)(gb_c, ge_c);
      enc(gb, ge);
    }
  }

  template <class Algorithm, class Loss, class Sampler, class Logger,
            class Terminator, class Encoder>
  void solve(Algorithm *alg, Loss &&loss,
             utility::sampler::detail::coordinate_sampler_t, Sampler &&sampler,
             const index_t num_coordinates, Logger &&logger,
             Terminator &&terminate, Encoder &&encoder) {
    std::vector<index_t> coordinates(num_coordinates);
    index_t *cb = coordinates.data();
    index_t *ce = cb + coordinates.size();
    const index_t *cb_c = cb;
    const index_t *ce_c = ce;

    fval = std::forward<Loss>(loss)(xb_c, gb);
    std::forward<Sampler>(sampler)(cb, ce);
    auto enc = std::forward<Encoder>(encoder)(gb_c, ge_c, cb_c, ce_c);
    enc(gb, ge);
    while (!std::forward<Terminator>(terminate)(k, fval, xb_c, xe_c, gb_c)) {
      iterate(alg, std::forward<Logger>(logger));
      fval = std::forward<Loss>(loss)(xb_c, gb);
      std::forward<Sampler>(sampler)(cb, ce);
      enc = std::forward<Encoder>(encoder)(gb_c, ge_c, cb_c, ce_c);
      enc(gb, ge);
    }
  }

  template <class Algorithm, class Loss, class Sampler1, class Sampler2,
            class Logger, class Terminator, class Encoder>
  void solve(Algorithm *alg, Loss &&loss,
             utility::sampler::detail::component_sampler_t, Sampler1 &&sampler1,
             const index_t num_components,
             utility::sampler::detail::coordinate_sampler_t,
             Sampler2 &&sampler2, const index_t num_coordinates,
             Logger &&logger, Terminator &&terminate, Encoder &&encoder) {
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

    std::forward<Sampler1>(sampler1)(compb, compe);
    fval = std::forward<Loss>(loss)(xb_c, gb, compb_c, compe_c);
    std::forward<Sampler2>(sampler2)(coorb, coore);
    auto enc = std::forward<Encoder>(encoder)(gb_c, ge_c, coorb_c, coore_c);
    enc(gb, ge);
    while (!std::forward<Terminator>(terminate)(k, fval, xb_c, xe_c, gb_c)) {
      iterate(alg, std::forward<Logger>(logger));
      std::forward<Sampler1>(sampler1)(compb, compe);
      fval = std::forward<Loss>(loss)(xb_c, gb, compb_c, compe_c);
      std::forward<Sampler2>(sampler2)(coorb, coore);
      enc = std::forward<Encoder>(encoder)(gb_c, ge_c, coorb_c, coore_c);
      enc(gb, ge);
    }
  }

  value_t getf() const { return fval; }
  std::vector<value_t> getx() const { return x; }

  ~serial() = default;

private:
  template <class Algorithm, class Logger>
  void iterate(Algorithm *alg, Logger &&logger) {
    alg->boost(index_t(0), k, k, gb_c, ge_c, gb);
    alg->smooth(k, k, xb_c, xe_c, gb_c, gb);
    const value_t step = alg->step(k, k, fval, xb_c, xe_c, gb_c);
    alg->prox(step, xb_c, xe_c, gb_c, xb);
    std::forward<Logger>(logger)(k, fval, xb_c, xe_c, gb_c);
    k++;
  }

  index_t k{1};
  value_t fval{0};
  value_t *xb, *gb, *ge;
  const value_t *xb_c, *xe_c, *gb_c, *ge_c;
  std::vector<value_t> x, g;
};
} // namespace execution
} // namespace polo

#endif
