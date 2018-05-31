#ifndef POLO_MATRIX_AMATRIX_HPP_
#define POLO_MATRIX_AMATRIX_HPP_

namespace polo {
namespace matrix {
template <class value_t, class index_t> struct amatrix {
  amatrix() noexcept = default;
  amatrix(const index_t nrows) noexcept : nrows_{nrows}, ncols_{nrows} {}
  amatrix(const index_t nrows, const index_t ncols) noexcept
      : nrows_{nrows}, ncols_{ncols} {}

  index_t nrows() const noexcept { return nrows_; }
  index_t ncols() const noexcept { return ncols_; }
  virtual value_t density() const noexcept {
    return nrows_ * ncols_ == 0 ? 0 : 1;
  }

  virtual void mult_add(const char trans, const value_t alpha, const value_t *x,
                        const value_t beta, value_t *y) const noexcept = 0;
  virtual void mult_add(const char trans, const value_t alpha, const value_t *x,
                        const value_t beta, value_t *y, const index_t *rbegin,
                        const index_t *rend) const noexcept = 0;

  void mult(const char trans, const value_t alpha, const value_t *x,
            value_t *y) const noexcept {
    mult_add(trans, alpha, x, 0, y);
  }
  void mult(const char trans, const value_t alpha, const value_t *x, value_t *y,
            const index_t *rbegin, const index_t *rend) const noexcept {
    mult_add(trans, alpha, x, 0, y, rbegin, rend);
  }

  virtual ~amatrix() = default;

private:
  index_t nrows_{0}, ncols_{0};
};
} // namespace matrix
} // namespace polo

#endif