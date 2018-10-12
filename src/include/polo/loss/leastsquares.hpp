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

  using aloss<value_t, index_t>::operator();
  using aloss<value_t, index_t>::partial;

  value_t operator()(const value_t *x, value_t *g) const noexcept override {
    value_t loss{0};
    std::vector<value_t> residual(aloss<value_t, index_t>::data_.nsamples());
    aloss<value_t, index_t>::data_.residual(x, &residual[0]);
    for (const value_t r : residual)
      loss += 0.5 * r * r;
    auto datamatrix = aloss<value_t, index_t>::data_.matrix();
    datamatrix->mult_add('t', 1, &residual[0], 0, g);
    return loss;
  }
  value_t incremental(const value_t *x, value_t *g, const index_t *ibegin,
                      const index_t *iend) const noexcept override {
    value_t loss{0};
    std::vector<value_t> residual(std::distance(ibegin, iend));
    aloss<value_t, index_t>::data_.residual(x, &residual[0], ibegin, iend);
    for (const value_t r : residual)
      loss += 0.5 * r * r;
    auto datamatrix = aloss<value_t, index_t>::data_.matrix();
    datamatrix->mult_add('t', 1, &residual[0], 0, g, ibegin, iend);
    return loss;
  }
};
} // namespace loss
} // namespace polo

#endif
