#ifndef BOX_CPP_
#define BOX_CPP_

#include <algorithm>
#include <vector>

namespace pbopt {
namespace projection {

template <class float_t> struct box {
  box() = default;

  box(const box &) = default;
  box &operator=(const box &) = default;
  box(box &&) = default;
  box &operator=(box &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt poject(InputIt1 gbegin, InputIt1 gend, InputIt2 xbegin,
                  const float_t step, OutputIt xnew) {
    float_t temp;
    std::size_t idx{0};
    while (gbegin != gend) {
      temp = *xbegin++ - step * *gbegin++;
      *xnew++ = std::min(std::max(temp, l[idx]), u[idx]);
      idx++;
    }
    return xnew;
  }

protected:
  ~box() = default;

private:
  std::vector<float_t> l, u;
};

} // namespace projection
} // namespace pbopt

#endif
