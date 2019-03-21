#ifndef POLO_TERMINATOR_GRADIENT_HPP_
#define POLO_TERMINATOR_GRADIENT_HPP_

#include <cmath>
#include <numeric>
#include <type_traits>

namespace polo {
namespace terminator {
template <class value_t = double, class index_t = int, int p = 2>
struct gradient {
  gradient(const value_t tol) : tol{tol} {}

  template <class InputIt1, class InputIt2>
  bool operator()(const index_t k, const value_t fval, InputIt1 x_begin,
                  InputIt1 x_end, InputIt2 g_begin) const {
    return terminate(x_begin, x_end, g_begin, std::integral_constant<int, p>{});
  }

private:
  template <class InputIt1, class InputIt2, int _p>
  bool terminate(InputIt1 x_begin, InputIt1 x_end, InputIt2 g_begin,
                 std::integral_constant<int, _p>) const {
    value_t norm{0};
    while (x_begin != x_end) {
      const value_t val = std::abs(*g_begin++);
      norm += std::pow(val, value_t(_p));
      x_begin++;
    }
    norm = std::pow(norm, value_t(1) / value_t(_p));
    return norm < tol;
  }
  template <class InputIt1, class InputIt2>
  bool terminate(InputIt1 x_begin, InputIt1 x_end, InputIt2 g_begin,
                 std::integral_constant<int, -1>) const {
    value_t max{0};
    while (x_begin != x_end) {
      const value_t val = std::abs(*g_begin++);
      if (val > max)
        max = val;
      x_begin++;
    }
    return max < tol;
  }
  template <class InputIt1, class InputIt2>
  bool terminate(InputIt1 x_begin, InputIt1 x_end, InputIt2 g_begin,
                 std::integral_constant<int, 1>) const {
    value_t norm{0};
    while (x_begin != x_end) {
      norm += std::abs(*g_begin++);
      x_begin++;
    }
    return norm < tol;
  }
  template <class InputIt1, class InputIt2>
  bool terminate(InputIt1 x_begin, InputIt1 x_end, InputIt2 g_begin,
                 std::integral_constant<int, 2>) const {
    value_t norm{0};
    while (x_begin != x_end) {
      const value_t val = *g_begin++;
      norm += val * val;
      x_begin++;
    }
    return std::sqrt(norm) < tol;
  }
  template <class InputIt1, class InputIt2>
  bool terminate(InputIt1 x_begin, InputIt1 x_end, InputIt2 g_begin,
                 std::integral_constant<int, 3>) const {
    value_t norm{0};
    while (x_begin != x_end) {
      const value_t val = std::abs(*g_begin++);
      norm += val * val * val;
      x_begin++;
    }
    return std::cbrt(norm) < tol;
  }

  const value_t tol;
};

template <class value_t, class index_t> struct gradient<value_t, index_t, 0> {
  gradient(const value_t tol, const value_t threshold = 0)
      : tol{tol}, threshold{threshold} {}

  template <class InputIt1, class InputIt2>
  bool operator()(const index_t k, const value_t fval, InputIt1 x_begin,
                  InputIt1 x_end, InputIt2 g_begin) const {
    index_t count{0};
    while (x_begin != x_end) {
      const value_t val = std::abs(*g_begin++);
      if (val >= threshold)
        count++;
      x_begin++;
    }
    return count < tol;
  }

private:
  const value_t tol;
  const value_t threshold;
};
} // namespace terminator
} // namespace polo

#endif
