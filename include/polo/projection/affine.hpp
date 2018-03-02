#ifndef AFFINE_HPP_
#define AFFINE_HPP_

#include <algorithm>
#include <iterator>
#include <vector>

#include "polo/projection/none.hpp"
#include "polo/utility/blas.hpp"
#include "polo/utility/lapack.hpp"

namespace polo {
namespace projection {

template <class value_t> struct affine : private none<value_t> {
  affine() = default;

  affine(const affine &) = default;
  affine &operator=(const affine &) = default;
  affine(affine &&) = default;
  affine &operator=(affine &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(const value_t step, InputIt1 xold_begin, InputIt1 xold_end,
                   InputIt2 gbegin, OutputIt xnew_begin) {
    none<value_t>::project(step, xold_begin, xold_end, gbegin,
                           std::begin(temp2));
    std::copy(std::begin(b), std::end(b), std::begin(temp1));
    polo::utility::matrix::blas<value_t>::gemv(
        'N', m, d, 1, A.data(), m, temp2.data(), 1, -1, temp1.data(), 1);
    polo::utility::matrix::lapack<value_t>::pptrs('L', m, 1, H.data(),
                                                  temp1.data(), m);
    polo::utility::matrix::blas<value_t>::gemv(
        'T', m, d, -1, A.data(), m, temp1.data(), 1, 1, temp2.data(), 1);
    return std::copy(std::begin(temp2), std::end(temp2), xnew_begin);
  }

protected:
  template <class InputIt1, class InputIt2>
  void params(InputIt1 abegin, InputIt1 aend, InputIt2 bbegin, InputIt2 bend) {
    A = std::vector<value_t>(abegin, aend);
    b = std::vector<value_t>(bbegin, bend);
    factorize();
  }
  void params(std::vector<value_t> A, std::vector<value_t> b) {
    this->A = std::move(A);
    this->b = std::move(b);
    factorize();
  }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~affine() = default;

private:
  void factorize() {
    m = b.size();
    d = A.size() / m;

    temp1 = std::vector<value_t>(m);
    temp2 = std::vector<value_t>(d);

    H = std::vector<value_t>((m * (m + 1)) / 2);
    for (std::size_t idx = 0; idx < d; idx++)
      polo::utility::matrix::blas<value_t>::spr('L', m, 1, &A[idx * m], 1,
                                                H.data());

    polo::utility::matrix::lapack<value_t>::pptrf('L', m, H.data());
  }
  std::size_t m, d;
  std::vector<value_t> A, b, H, temp1, temp2;
};

} // namespace projection
} // namespace polo

#endif
