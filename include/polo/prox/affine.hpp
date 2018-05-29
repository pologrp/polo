#ifndef POLO_PROX_AFFINE_HPP_
#define POLO_PROX_AFFINE_HPP_

#include <algorithm>
#include <iterator>
#include <mutex>
#include <vector>

#include "polo/prox/none.hpp"
#include "polo/utility/blas.hpp"
#include "polo/utility/lapack.hpp"

namespace polo {
namespace prox {
template <class value_t, class index_t>
struct affine : private none<value_t, index_t> {
  affine() = default;

  affine(const affine &) = default;
  affine &operator=(const affine &) = default;
  affine(affine &&) = default;
  affine &operator=(affine &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt prox(const value_t step, InputIt1 xold_begin, InputIt1 xold_end,
                InputIt2 gbegin, OutputIt xnew_begin) {
    std::lock_guard<std::mutex> lock(sync);
    none<value_t, index_t>::prox(step, xold_begin, xold_end, gbegin,
                                 std::begin(temp2));
    std::copy(std::begin(b), std::end(b), std::begin(temp1));
    utility::matrix::blas<value_t>::gemv('N', m, d, 1, &A[0], m, &temp2[0], 1,
                                         -1, &temp1[0], 1);
    utility::matrix::lapack<value_t>::pptrs('L', m, 1, &H[0], &temp1[0], m);
    utility::matrix::blas<value_t>::gemv('T', m, d, -1, &A[0], m, &temp1[0], 1,
                                         1, &temp2[0], 1);
    return std::copy(std::begin(temp2), std::end(temp2), xnew_begin);
  }

protected:
  template <class InputIt1, class InputIt2>
  void parameters(InputIt1 abegin, InputIt1 aend, InputIt2 bbegin,
                  InputIt2 bend) {
    A = std::vector<value_t>(abegin, aend);
    b = std::vector<value_t>(bbegin, bend);
    factorize();
  }
  template <class T1, class T2>
  void parameters(const std::vector<T1> &A, const std::vector<T2> &b) {
    parameters(std::begin(A), std::end(A), std::begin(b), std::end(b));
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
      utility::matrix::blas<value_t>::spr('L', m, 1, &A[idx * m], 1, &H[0]);

    utility::matrix::lapack<value_t>::pptrf('L', m, &H[0]);
  }

  std::size_t m, d;
  std::vector<value_t> A, b, H, temp1, temp2;
  std::mutex sync;
};
} // namespace prox
} // namespace polo

#endif
