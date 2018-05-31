#ifndef POLO_MATRIX_DMATRIX_HPP_
#define POLO_MATRIX_DMATRIX_HPP_

#include <exception>
#include <limits>
#include <utility>
#include <vector>

#include "polo/matrix/amatrix.hpp"
#include "polo/utility/blas.hpp"

namespace polo {
namespace matrix {
template <class value_t, class index_t> struct smatrix;

template <class value_t, class index_t>
struct dmatrix : public amatrix<value_t, index_t> {
  dmatrix() noexcept(noexcept(std::vector<value_t>())) = default;
  dmatrix(const index_t nrows)
      : amatrix<value_t, index_t>(nrows), values_(nrows * nrows) {}
  dmatrix(const index_t nrows, const index_t ncols)
      : amatrix<value_t, index_t>(nrows, ncols), values_(nrows * ncols) {}

  dmatrix(const index_t nrows, const index_t ncols,
          const std::vector<value_t> &values)
      : amatrix<value_t, index_t>(nrows, ncols) {
    if (values.size() != std::size_t(nrows) * ncols)
      throw std::domain_error("dmatrix: dimension mismatch in construction");
    values_ = values;
  }

  void mult_add(const char trans, const value_t alpha, const value_t *x,
                const value_t beta, value_t *y) const noexcept override {
    utility::matrix::blas<value_t>::gemv(
        trans, amatrix<value_t, index_t>::nrows(),
        amatrix<value_t, index_t>::ncols(), alpha, &values_[0],
        amatrix<value_t, index_t>::nrows(), x, 1, beta, y, 1);
  }
  void mult_add(const char trans, const value_t alpha, const value_t *x,
                const value_t beta, value_t *y, const index_t *rbegin,
                const index_t *rend) const noexcept override {
    while (rbegin != rend) {
      const index_t row = *rbegin++;
      utility::matrix::blas<value_t>::gemv(
          trans, 1, amatrix<value_t, index_t>::ncols(), alpha, &values_[row],
          amatrix<value_t, index_t>::nrows(), x, 1, beta, y, 1);
    }
  }

  smatrix<value_t, index_t>
  sparse(const value_t eps = std::numeric_limits<value_t>::min()) const {
    if (eps < 0)
      throw std::domain_error("dmatrix::sparse: negative eps is not allowed");
    const index_t nrows = amatrix<value_t, index_t>::nrows();
    const index_t ncols = amatrix<value_t, index_t>::ncols();
    std::vector<index_t> row_ptr{0}, cols;
    std::vector<value_t> values;
    for (index_t row = 0; row < nrows; row++) {
      for (index_t col = 0; col < ncols; col++) {
        const value_t val = values_[col * nrows + row];
        if ((val > eps) | (val < -eps)) {
          values.push_back(val);
          cols.push_back(col);
        }
      }
      row_ptr.push_back(cols.size());
    }
    return smatrix<value_t, index_t>(nrows, ncols, std::move(row_ptr),
                                     std::move(cols), std::move(values));
  }

private:
  std::vector<value_t> values_;
};
} // namespace matrix
} // namespace polo

#endif