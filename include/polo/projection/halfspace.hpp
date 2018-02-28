#ifndef HALFSPACE_HPP_
#define HALFSPACE_HPP_

#include <algorithm>
#include <iterator>
#include <vector>

namespace polo {
namespace projection {

template <class float_t> struct halfspace {
  halfspace() = default;

  halfspace(const halfspace &) = default;
  halfspace &operator=(const halfspace &) = default;
  halfspace(halfspace &&) = default;
  halfspace &operator=(halfspace &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(const float_t step, InputIt1 xold_begin, InputIt1 xold_end,
                   InputIt2 gbegin, OutputIt xnew_begin) {
    std::size_t idx{0};
    float_t atx{0}, xval;
    while (xold_begin != xold_end) {
      xval = *xold_begin++ - step * *gbegin++;
      atx += a[idx] * xval;
      temp[idx] = xval;
      idx++;
    }
    const float_t diff = atx - alpha;
    return std::transform(std::begin(temp), std::end(temp), std::begin(a),
                          xnew_begin, [&](const float_t &x, const float_t &a) {
                            return x - std::max(diff, float_t{0}) / norm * a;
                          });
  }

protected:
  template <class InputIt>
  void params(InputIt abegin, InputIt aend, const float_t alpha) {
    a = std::vector<float_t>(abegin, aend);
    temp = std::vector<float_t>(a.size());
    this->alpha = alpha;
    for (const auto val : a)
      norm += val * val;
  }
  void params(std::vector<float_t> a, const float_t alpha) {
    this->a = std::move(a);
    temp = std::vector<float_t>(this->a.size());
    this->alpha = alpha;
    for (const auto val : this->a)
      norm += val * val;
  }

  template <class InputIt> void initialize(InputIt, InputIt) {}

  ~halfspace() = default;

private:
  float_t alpha, norm{0};
  std::vector<float_t> a, temp;
};

} // namespace projection
} // namespace polo

#endif
