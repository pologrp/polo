#ifndef POLO_MATRIX_DMATRIX_HPP_
#define POLO_MATRIX_DMATRIX_HPP_

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

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

  dmatrix(const index_t nrows, const index_t ncols, std::vector<value_t> values)
      : amatrix<value_t, index_t>(nrows, ncols) {
    if (values.size() != std::size_t(nrows) * ncols)
      throw std::domain_error("dmatrix: dimension mismatch in construction");
    values_ = std::move(values);
  }

  value_t operator()(const index_t row, const index_t col) const override {
    const index_t nrows_ = amatrix<value_t, index_t>::nrows();
    const index_t ncols_ = amatrix<value_t, index_t>::ncols();
    if (row >= nrows_ || col >= ncols_)
      throw std::range_error("row or column out of range.");
    return values_[col * nrows_ + row];
  }
  std::vector<value_t> getrow(const index_t row) const override {
    const index_t nrows_ = amatrix<value_t, index_t>::nrows();
    const index_t ncols_ = amatrix<value_t, index_t>::ncols();
    if (row >= nrows_)
      throw std::range_error("row out of range.");
    std::vector<value_t> rowvec(ncols_);
    for (index_t col = 0; col < ncols_; col++)
      rowvec[col] = values_[col * nrows_ + row];
    return rowvec;
  }
  std::vector<index_t> colindices(const index_t row) const override {
    const index_t nrows_ = amatrix<value_t, index_t>::nrows();
    const index_t ncols_ = amatrix<value_t, index_t>::ncols();
    if (row >= nrows_)
      throw std::range_error("row out of range.");
    std::vector<index_t> indices(ncols_);
    index_t idx{0};
    std::transform(std::begin(indices), std::end(indices), std::begin(indices),
                   [&](const index_t) { return idx++; });
    return indices;
  }

  void mult_add(const char trans, const value_t alpha, const value_t *x,
                const value_t beta, value_t *y) const noexcept override {
    const index_t nrows = amatrix<value_t, index_t>::nrows();
    const index_t ncols = amatrix<value_t, index_t>::ncols();
    utility::matrix::blas<value_t>::gemv(trans, nrows, ncols, alpha,
                                         &values_[0], nrows, x, 1, beta, y, 1);
  }
  void mult_add(const char trans, const value_t alpha, const value_t *x,
                const value_t beta, value_t *y, const index_t *rbegin,
                const index_t *rend) const noexcept override {
    const index_t nrows = amatrix<value_t, index_t>::nrows();
    const index_t ncols = amatrix<value_t, index_t>::ncols();
    const bool notrans = (trans == 'N') | (trans == 'n');
    value_t *yend = y + (notrans ? std::distance(rbegin, rend) : size_t(ncols));
    std::transform(y, yend, y, [=](const value_t val) { return beta * val; });
    if (notrans)
      while (rbegin != rend) {
        const index_t row = *rbegin++;
        utility::matrix::blas<value_t>::gemv(
            trans, 1, ncols, alpha, &values_[row], nrows, x, 1, 1, y++, 1);
      }
    else
      while (rbegin != rend) {
        const index_t row = *rbegin++;
        utility::matrix::blas<value_t>::gemv(
            trans, 1, ncols, alpha, &values_[row], nrows, x++, 1, 1, y, 1);
      }
  }

  void save(std::ostream &os) const override {
    const index_t nrows_ = amatrix<value_t, index_t>::nrows();
    const index_t ncols_ = amatrix<value_t, index_t>::ncols();
    os.write(reinterpret_cast<const char *>(&nrows_), sizeof(index_t));
    os.write(reinterpret_cast<const char *>(&ncols_), sizeof(index_t));
    os.write(reinterpret_cast<const char *>(&values_[0]),
             std::size_t(nrows_) * ncols_ * sizeof(value_t));
  }
  void load(std::istream &is) override {
    index_t nrows_, ncols_;
    is.read(reinterpret_cast<char *>(&nrows_), sizeof(index_t));
    is.read(reinterpret_cast<char *>(&ncols_), sizeof(index_t));
    amatrix<value_t, index_t>::nrows(nrows_);
    amatrix<value_t, index_t>::ncols(ncols_);
    values_ = std::vector<value_t>(std::size_t(nrows_) * ncols_);
    is.read(reinterpret_cast<char *>(&values_[0]),
            std::size_t(nrows_) * ncols_ * sizeof(value_t));
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
