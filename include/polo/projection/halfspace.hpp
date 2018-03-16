#ifndef HALFSPACE_HPP_
#define HALFSPACE_HPP_

#include <algorithm>
#include <iterator>
#include <mutex>
#include <vector>

namespace polo {
namespace projection {
template <class value_t> struct halfspace {
  halfspace() = default;

  halfspace(const halfspace &) = default;
  halfspace &operator=(const halfspace &) = default;
  halfspace(halfspace &&) = default;
  halfspace &operator=(halfspace &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(const value_t step, InputIt1 xold_begin, InputIt1 xold_end,
                   InputIt2 gbegin, OutputIt xnew_begin) {
    std::lock_guard<std::mutex> lock(sync);
    std::size_t idx{0};
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
  void params(InputIt abegin, InputIt aend, const value_t alpha) {
    a = std::vector<value_t>(abegin, aend);
    temp = std::vector<value_t>(a.size());
    this->alpha = alpha;
    for (const auto val : a)
      norm += val * val;
  }
  template <class T> void params(const std::vector<T> &a, const value_t alpha) {
    params(std::begin(a), std::end(a), alpha);
  }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~halfspace() = default;

private:
  value_t alpha, norm{0};
  std::vector<value_t> a, temp;
  std::mutex sync;
};
} // namespace projection
} // namespace polo

#endif
