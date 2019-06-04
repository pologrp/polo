#ifndef POLO_LOSS_LEASTSQUARES_HPP_
#define POLO_LOSS_LEASTSQUARES_HPP_

#include <iterator>

#include "polo/loss/aloss.hpp"

namespace polo {
namespace loss {
template <class value_t, class index_t>
struct leastsquares : public aloss<value_t, index_t> {
  leastsquares() = default;
  leastsquares(data<value_t, index_t> data)
      : aloss<value_t, index_t>(std::move(data)) {}

  value_t operator()(const value_t *x, value_t *g) const noexcept override {
    value_t loss{0};
    std::vector<value_t> residual(aloss<value_t, index_t>::nsamples());
    aloss<value_t, index_t>::data_.residual(x, residual.data());
    for (const value_t r : residual)
      loss += 0.5 * r * r;
    aloss<value_t, index_t>::matrix()->mult_add('t', 1, residual.data(), 0, g);
    return loss;
  }
  value_t operator()(const value_t *x, value_t *g, const index_t *ib,
                     const index_t *ie) const noexcept override {
    value_t loss{0};
    std::vector<value_t> residual(std::distance(ib, ie));
    aloss<value_t, index_t>::data_.residual(x, residual.data(), ib, ie);
    for (const value_t r : residual)
      loss += 0.5 * r * r;
    aloss<value_t, index_t>::matrix()->mult_add('t', 1, residual.data(), 0, g,
                                                ib, ie);
    return loss;
  }
};
} // namespace loss
} // namespace polo

#endif
