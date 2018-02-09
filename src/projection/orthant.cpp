#ifndef ORTHANT_CPP_
#define ORTHANT_CPP_

#include <algorithm>

namespace pbopt {
namespace projection {

struct _pos_orthant {};
struct _neg_orthant {};

template <class float_t, class T> struct orthant {
  orthant() = default;

  // Usual suspects; needed due to the protected destructor.
  orthant(const orthant &) = default;
  orthant &operator=(const orthant &) = default;
  orthant(orthant &&) = default;
  orthant &operator=(orthant &&) = default;

  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(InputIt1 g_first, InputIt1 g_last, InputIt2 xold,
                   const float_t step, OutputIt xnew) {
    return project(g_first, g_last, x_old, step, xnew, T{});
  }

protected:
  ~orthant() = default;

private:
  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(InputIt1 g_first, InputIt1 g_last, InputIt2 xold,
                   const float_t step, OutputIt xnew, _pos_orthant) {
    while (g_first != g_last)
      *xnew++ = std::max(*xold++ - step * *g_first++, float_t{0});
    return xnew;
  }
  template <class InputIt1, class InputIt2, class OutputIt>
  OutputIt project(InputIt1 g_first, InputIt1 g_last, InputIt2 xold,
                   const float_t step, OutputIt xnew, _neg_orthant) {
    while (g_first != g_last)
      *xnew++ = std::min(*xold++ - step * *g_first++, float_t{0});
    return xnew;
  }
};

template <class float_t>
using positive_orthant = orthant<float_t, _pos_orthant>;

template <class float_t>
using negative_orthant = orthant<float_t, _neg_orthant>;

} // namespace projection
} // namespace pbopt

#endif
