#ifndef POLO_LOSS_ALOSS_HPP_
#define POLO_LOSS_ALOSS_HPP_

#include "polo/loss/data.hpp"

namespace polo {
namespace loss {
template <class value_t, class index_t> struct aloss {
  using data_t = loss::data<value_t, index_t>;
  using matrix_t = typename data_t::matrix_t;
  using vector_t = typename data_t::vector_t;

  aloss() = default;
  aloss(data_t data) : data_(std::move(data)) {}

  void data(data_t data) { data_ = std::move(data); }
  data_t data() const noexcept { return data_; }

  index_t nsamples() const noexcept { return data_.nsamples(); }
  index_t nfeatures() const noexcept { return data_.nfeatures(); }

  matrix_t matrix() const noexcept { return data_.matrix(); }
  vector_t labels() const noexcept { return data_.labels(); }

  virtual value_t operator()(const value_t *x, value_t *g) const noexcept = 0;
  virtual value_t operator()(const value_t *x, value_t *g, const index_t *ib,
                             const index_t *ie) const noexcept = 0;

  virtual ~aloss() = default;

protected:
  data_t data_;
};
} // namespace loss
} // namespace polo

#endif
