#ifndef POLO_LOSS_LOGISTIC_HPP_
#define POLO_LOSS_LOGISTIC_HPP_

#include <algorithm>
#include <cmath>
#include <iterator>

#include "polo/loss/aloss.hpp"

namespace polo {
namespace loss {
template <class value_t, class index_t>
struct logistic : public aloss<value_t, index_t> {
  logistic() = default;
  logistic(data<value_t, index_t> data)
      : aloss<value_t, index_t>(std::move(data)) {}

  using aloss<value_t, index_t>::operator();
  using aloss<value_t, index_t>::partial;

  value_t operator()(const value_t *x, value_t *g) const noexcept override {
    value_t loss{0};
    std::vector<value_t> ax(aloss<value_t, index_t>::data_.nsamples());
    auto A = aloss<value_t, index_t>::data_.matrix();
    auto b = aloss<value_t, index_t>::data_.labels();
    A->mult_add('n', 1, x, 0, &ax[0]);
    std::transform(std::begin(*b), std::end(*b), std::begin(ax), std::begin(ax),
                   [](const value_t b, const value_t ax) { return -b * ax; });
    std::transform(std::begin(*b), std::end(*b), std::begin(ax), std::begin(ax),
                   [&](const value_t b, const value_t val) {
                     const value_t temp = std::exp(val);
                     if (std::isinf(temp)) {
                       loss += val;
                       return -b;
                     } else {
                       loss += std::log1p(temp);
                       return -b * temp / (1 + temp);
                     }
                   });
    A->mult_add('t', 1, &ax[0], 0, g);
    return loss;
  }
  value_t incremental(const value_t *x, value_t *g, const index_t *ibegin,
                      const index_t *iend) const noexcept override {
    value_t loss{0};
    std::vector<value_t> ax(std::distance(ibegin, iend));
    auto A = aloss<value_t, index_t>::data_.matrix();
    auto b = aloss<value_t, index_t>::data_.labels();
    A->mult_add('n', 1, x, 0, &ax[0], ibegin, iend);
    const index_t *ibegin2 = ibegin;
    index_t idx = 0;
    while (ibegin2 != iend)
      ax[idx++] *= -(*b)[*ibegin2++];
    ibegin2 = ibegin;
    for (auto &val : ax) {
      const value_t temp = std::exp(val);
      if (std::isinf(temp)) {
        loss += val;
        val = -(*b)[*ibegin2++];
      } else {
        loss += std::log1p(temp);
        val = -(*b)[*ibegin2++] * temp / (1 + temp);
      }
    }
    A->mult_add('t', 1, &ax[0], 0, g, ibegin, iend);
    return loss;
  }
};
} // namespace loss
} // namespace polo

#endif
