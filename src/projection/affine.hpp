#ifndef AFFINE_HPP_
#define AFFINE_HPP_

#include <algorithm>
#include <iterator>
#include <vector>

#include "projection/none.hpp"
#include "utility/blas.hpp"
#include "utility/lapack.hpp"

namespace pbopt {
namespace projection {

template <class float_t> struct affine : private none<float_t> {
  affine() = default;

  affine(const affine &) = default;
  affine &operator=(const affine &) = default;
  affine(affine &&) = default;
  affine &operator=(affine &&) = default;

  template <class InputIt1, class InputIt2>
  void params(InputIt1 abegin, InputIt1 aend, InputIt2 bbegin, InputIt2 bend) {
    A = std::vector<float_t>(abegin, aend);
    b = std::vector<float_t>(bbegin, bend);
    factorize();
  }
  void params(std::vector<float_t> A, std::vector<float_t> b) {
    this->A = std::move(A);
    this->b = std::move(b);
    factorize();
  }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(const float_t step, InputIt1 xold_begin, InputIt1 xold_end,
                   InputIt2 gbegin, OutputIt xnew_begin) {
    none<float_t>::project(step, xold_begin, xold_end, gbegin,
                           std::begin(temp2));
    std::copy(std::begin(b), std::end(b), std::begin(temp1));
    pbopt::utility::blas<float_t>::gemv('N', m, d, 1, A.data(), m, temp2.data(),
                                        1, -1, temp1.data(), 1);
    pbopt::utility::lapack<float_t>::pptrs('L', m, 1, H.data(), temp1.data(),
                                           m);
    pbopt::utility::blas<float_t>::gemv('T', m, d, -1, A.data(), m,
                                        temp1.data(), 1, 1, temp2.data(), 1);
    return std::copy(std::begin(temp2), std::end(temp2), xnew_begin);
  }

protected:
  ~affine() = default;

private:
  void factorize() {
    m = b.size();
    d = A.size() / m;

    temp1 = std::vector<float_t>(m);
    temp2 = std::vector<float_t>(d);

    H = std::vector<float_t>((m * (m + 1)) / 2);
    for (std::size_t idx = 0; idx < d; idx++)
      pbopt::utility::blas<float_t>::spr('L', m, 1, &A[idx * m], 1, H.data());

    pbopt::utility::lapack<float_t>::pptrf('L', m, H.data());
  }
  std::size_t m, d;
  std::vector<float_t> A, b, H, temp1, temp2;
};

} // namespace projection
} // namespace pbopt

#endif
