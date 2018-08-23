#ifndef POLO_LOSS_DATA_HPP_
#define POLO_LOSS_DATA_HPP_

#include <algorithm>
#include <exception>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "polo/matrix/amatrix.hpp"
#include "polo/matrix/dmatrix.hpp"
#include "polo/matrix/smatrix.hpp"

namespace polo {
namespace loss {
template <class value_t, class index_t> struct data {
  data() = default;
  data(std::shared_ptr<const matrix::amatrix<value_t, index_t>> A,
       std::shared_ptr<const std::vector<value_t>> b)
      : A(std::move(A)), b(std::move(b)) {
    if (std::size_t(this->A->nrows()) != this->b->size())
      throw std::domain_error("data: dimension mismatch in construction");
  }

  index_t nsamples() const noexcept { return A->nrows(); }
  index_t nfeatures() const noexcept { return A->ncols(); }
  value_t density() const noexcept { return A->density(); }
  std::size_t size() const noexcept {
    return A->size() + sizeof(value_t) * b->size();
  }

  void residual(const value_t *x, value_t *r) const noexcept {
    std::copy(std::begin(*b), std::end(*b), r);
    A->mult_add('n', 1, x, -1, r);
  }
  void residual(const value_t *x, value_t *r, const index_t *ibegin,
                const index_t *iend) const noexcept {
    for (const index_t *temp = ibegin; temp < iend; temp++)
      r[*temp] = (*b)[*temp];
    A->mult_add('n', 1, x, -1, r, ibegin, iend);
  }

  std::shared_ptr<const matrix::amatrix<value_t, index_t>> matrix() const
      noexcept {
    return A;
  }
  std::shared_ptr<const std::vector<value_t>> labels() const noexcept {
    return b;
  }

  void save(const std::string &filename) const {
    std::ofstream file(filename, std::ios_base::binary);
    A->save(file);
    file.write(reinterpret_cast<const char *>(b->data()),
               b->size() * sizeof(value_t));
  }
  void load(const std::string &filename, bool dense) {
    std::ifstream file(filename, std::ios_base::binary);
    A.reset();
    b.reset();
    if (dense) {
      matrix::dmatrix<value_t, index_t> matrix_;
      matrix_.load(file);
      A.reset(new matrix::dmatrix<value_t, index_t>(std::move(matrix_)));
    } else {
      matrix::smatrix<value_t, index_t> matrix_;
      matrix_.load(file);
      A.reset(new matrix::smatrix<value_t, index_t>(std::move(matrix_)));
    }
    std::vector<value_t> labels(A->nrows());
    file.read(reinterpret_cast<char *>(&labels[0]),
              A->nrows() * sizeof(value_t));
    b.reset(new std::vector<value_t>(std::move(labels)));
  }

private:
  std::shared_ptr<const matrix::amatrix<value_t, index_t>> A;
  std::shared_ptr<const std::vector<value_t>> b;
};
} // namespace loss
} // namespace polo

#endif
