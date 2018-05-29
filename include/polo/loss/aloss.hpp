#ifndef POLO_LOSS_ALOSS_HPP_
#define POLO_LOSS_ALOSS_HPP_

#include "polo/loss/data.hpp"

namespace polo {
namespace loss {
template <class value_t, class index_t> struct aloss {
  aloss() = default;
  aloss(loss::data<value_t, index_t> data) : data_(std::move(data)) {}

  void data(loss::data<value_t, index_t> data) { data_ = std::move(data); }
  loss::data<value_t, index_t> data() const noexcept { return data_; }

  std::shared_ptr<matrix::amatrix<value_t, index_t>> matrix() const noexcept {
    return data_.matrix();
  }
  std::shared_ptr<std::vector<value_t>> labels() const noexcept {
    return data_.labels();
  }

  virtual value_t operator()(const value_t *x, value_t *g) const noexcept = 0;
  virtual value_t partial(const value_t *x, value_t *g, const index_t *ibegin,
                          const index_t *iend) const noexcept {
    std::vector<value_t> full(data_.nfeatures());
    value_t fval = operator()(x, &full[0]);
    std::size_t idx{0};
    while (ibegin != iend)
      g[idx++] = full[*ibegin++];
    return fval;
  };
  virtual value_t incremental(const value_t *x, value_t *g,
                              const index_t *ibegin, const index_t *iend) const
      noexcept = 0;
  virtual value_t operator()(const value_t *x, value_t *g,
                             const index_t *i1begin, const index_t *i1end,
                             const index_t *i2begin, const index_t *i2end) const
      noexcept {
    std::vector<value_t> full(data_.nfeatures());
    value_t fval = incremental(x, &full[0], i1begin, i1end);
    std::size_t idx{0};
    while (i2begin != i2end)
      g[idx++] = full[*i2begin++];
    return fval;
  };

  virtual ~aloss() = default;

protected:
  loss::data<value_t, index_t> data_;
};
} // namespace loss
} // namespace polo

#endif
