#ifndef SMATRIX_HPP_
#define SMATRIX_HPP_

#include <algorithm>
#include <exception>
#include <utility>
#include <vector>

#include "polo/matrix/amatrix.hpp"

namespace polo {
namespace matrix {
template <class value_t, class index_t> struct dmatrix;

template <class value_t, class index_t>
struct smatrix : public amatrix<value_t, index_t> {
  smatrix() noexcept(noexcept(std::vector<value_t>())) = default;
  smatrix(const index_t nrows)
      : amatrix<value_t, index_t>(nrows), row_ptr_(nrows + 1) {}
  smatrix(const index_t nrows, const index_t ncols)
      : amatrix<value_t, index_t>(nrows, ncols), row_ptr_(nrows + 1) {}

  smatrix(const index_t nrows, const index_t ncols,
          const std::vector<index_t> &row_ptr, const std::vector<index_t> &cols,
          const std::vector<value_t> &values)
      : amatrix<value_t, index_t>(nrows, ncols) {
    if ((std::size_t(nrows) != row_ptr.size() - 1) |
        (cols.size() != values.size()))
      throw std::domain_error("smatrix: dimension mismatch in construction");
    auto res = std::max_element(std::begin(cols), std::end(cols));
    if (res != std::end(cols) && *res >= ncols)
      throw std::domain_error("smatrix: dimension mismatch in construction");
    if (values.size() > 0 && std::size_t(row_ptr.back()) != values.size())
      throw std::domain_error("smatrix: dimension mismatch in construction");
    row_ptr_ = row_ptr;
    cols_ = cols;
    values_ = values;
  }

  value_t density() const noexcept override {
    const index_t nrows = amatrix<value_t, index_t>::nrows();
    const index_t ncols = amatrix<value_t, index_t>::ncols();
    return ncols * nrows == 0 ? 0 : value_t(values_.size()) / nrows / ncols;
  }

  void mult_add(const char trans, const value_t alpha, const value_t *x,
                const value_t beta, value_t *y) const noexcept override {
    kernel(trans, alpha, x, beta, y, nullptr, nullptr);
  }
  void mult_add(const char trans, const value_t alpha, const value_t *x,
                const value_t beta, value_t *y, const index_t *rbegin,
                const index_t *rend) const noexcept override {
    kernel(trans, alpha, x, beta, y, rbegin, rend);
  }

  dmatrix<value_t, index_t> dense() const {
    const index_t nrows = amatrix<value_t, index_t>::nrows();
    const index_t ncols = amatrix<value_t, index_t>::ncols();
    std::vector<value_t> values(nrows * ncols);
    for (index_t row = 0; row < nrows; row++) {
      const index_t nnz = row_ptr_[row + 1] - row_ptr_[row];
      index_t colstart = row_ptr_[row];
      for (index_t colidx = colstart; colidx < colstart + nnz; colidx++)
        values[cols_[colidx] * nrows + row] = values_[colidx];
    }
    return dmatrix<value_t, index_t>(nrows, ncols, std::move(values));
  }

private:
  void kernel(const char trans, const value_t alpha, const value_t *x,
              const value_t beta, value_t *y, const index_t *rbegin,
              const index_t *rend) const noexcept {
    const index_t nrows = amatrix<value_t, index_t>::nrows();
    const index_t ncols = amatrix<value_t, index_t>::ncols();
    if ((trans == 'n') | (trans == 'N')) {
      if ((rbegin == nullptr) | (rend == nullptr))
        for (index_t row = 0; row < nrows; row++)
          notrans_f(alpha, x, row, beta, y);
      else
        while (rbegin != rend)
          notrans_f(alpha, x, *rbegin++, beta, y);
    } else {
      for (index_t col = 0; col < ncols; col++)
        y[col] *= beta;
      if ((rbegin == nullptr) | (rend == nullptr))
        for (index_t row = 0; row < nrows; row++)
          trans_f(alpha, x, row, y);
      else
        while (rbegin != rend)
          trans_f(alpha, x, *rbegin++, y);
    }
  }

  void notrans_f(const value_t alpha, const value_t *x, const index_t row,
                 const value_t beta, value_t *y) const noexcept {
    const index_t nnz = row_ptr_[row + 1] - row_ptr_[row];
    index_t colstart = row_ptr_[row];
    y[row] *= beta;
    for (index_t colidx = colstart; colidx < colstart + nnz; colidx++)
      y[row] += alpha * values_[colidx] * x[cols_[colidx]];
  }

  void trans_f(const value_t alpha, const value_t *x, const index_t row,
               value_t *y) const noexcept {
    const index_t nnz = row_ptr_[row + 1] - row_ptr_[row];
    index_t colstart = row_ptr_[row];
    for (index_t colidx = colstart; colidx < colstart + nnz; colidx++)
      y[cols_[colidx]] += alpha * values_[colidx] * x[row];
  }

  std::vector<index_t> row_ptr_, cols_;
  std::vector<value_t> values_;
};
} // namespace matrix
} // namespace polo

#endif
