#ifndef POLO_PROX_HALFSPACE_HPP_
#define POLO_PROX_HALFSPACE_HPP_

#include <algorithm>
#include <iterator>
#include <mutex>
#include <vector>

namespace polo {
namespace prox {
template <class value_t, class index_t> struct halfspace {
  halfspace() = default;

  halfspace(const halfspace &) = default;
  halfspace &operator=(const halfspace &) = default;
  halfspace(halfspace &&) = default;
  halfspace &operator=(halfspace &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt prox(const value_t step, InputIt1 xold_begin, InputIt1 xold_end,
                InputIt2 gbegin, OutputIt xnew_begin) {
    std::lock_guard<std::mutex> lock(sync);
    index_t idx{0};
    value_t atx{0}, xval;
    while (xold_begin != xold_end) {
      xval = *xold_begin++ - step * *gbegin++;
      atx += a[idx] * xval;
      temp[idx] = xval;
      idx++;
    }
    const value_t diff = atx - alpha;
    return std::transform(std::begin(temp), std::end(temp), std::begin(a),
                          xnew_begin, [&](const value_t x, const value_t a) {
                            return x - std::max(diff, value_t{0}) / norm * a;
                          });
  }

protected:
  template <class InputIt>
  void parameters(InputIt abegin, InputIt aend, const value_t alpha) {
    a = std::vector<value_t>(abegin, aend);
    temp = std::vector<value_t>(a.size());
    this->alpha = alpha;
    for (const auto val : a)
      norm += val * val;
  }
  template <class T>
  void parameters(const std::vector<T> &a, const value_t alpha) {
    parameters(std::begin(a), std::end(a), alpha);
  }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~halfspace() = default;

private:
  value_t alpha, norm{0};
  std::vector<value_t> a, temp;
  std::mutex sync;
};
} // namespace prox
} // namespace polo

#endif