#ifndef POLO_TERMINATOR_DECISION_HPP_
#define POLO_TERMINATOR_DECISION_HPP_

#include <cmath>
#include <iterator>
#include <type_traits>
#include <vector>

namespace polo {
namespace terminator {
template <class value_t = double, class index_t = int, int p = 2>
struct decision {
  decision(const value_t tol) : tol{tol} {}

  template <class InputIt1, class InputIt2>
  bool operator()(const index_t k, const value_t fval, InputIt1 x_begin,
                  InputIt1 x_end, InputIt2 g_begin) {
    const size_t d = std::distance(x_begin, x_end);
    if (xprev.size() != d) {
      xprev = std::vector<value_t>(x_begin, x_end);
      return false;
    }
    return terminate(x_begin, x_end, std::begin(xprev),
                     std::integral_constant<int, p>{});
  }

private:
  template <class InputIt, class OutputIt, int _p>
  bool terminate(InputIt x_begin, InputIt x_end, OutputIt xprev_begin,
                 std::integral_constant<int, _p>) const {
    value_t norm{0};
    while (x_begin != x_end) {
      const value_t val = *x_begin++;
      const value_t diff = std::abs(*xprev_begin - val);
      norm += std::pow(diff, value_t(_p));
      *xprev_begin++ = val;
    }
    norm = std::pow(norm, value_t(1) / value_t(_p));
    return norm < tol;
  }
  template <class InputIt, class OutputIt>
  bool terminate(InputIt x_begin, InputIt x_end, OutputIt xprev_begin,
                 std::integral_constant<int, -1>) const {
    value_t max{0};
    while (x_begin != x_end) {
      const value_t val = *x_begin++;
      const value_t diff = std::abs(*xprev_begin - val);
      if (diff > max)
        max = diff;
      *xprev_begin++ = val;
    }
    return max < tol;
  }
  template <class InputIt, class OutputIt>
  bool terminate(InputIt x_begin, InputIt x_end, OutputIt xprev_begin,
                 std::integral_constant<int, 1>) const {
    value_t norm{0};
    while (x_begin != x_end) {
      const value_t val = *x_begin++;
      norm += std::abs(*xprev_begin - val);
      *xprev_begin++ = val;
    }
    return norm < tol;
  }
  template <class InputIt, class OutputIt>
  bool terminate(InputIt x_begin, InputIt x_end, OutputIt xprev_begin,
                 std::integral_constant<int, 2>) const {
    value_t norm{0};
    while (x_begin != x_end) {
      const value_t val = *x_begin++;
      const value_t diff = *xprev_begin - val;
      norm += diff * diff;
      *xprev_begin++ = val;
    }
    return std::sqrt(norm) < tol;
  }
  template <class InputIt, class OutputIt>
  bool terminate(InputIt x_begin, InputIt x_end, OutputIt xprev_begin,
                 std::integral_constant<int, 3>) const {
    value_t norm{0};
    while (x_begin != x_end) {
      const value_t val = *x_begin++;
      const value_t diff = std::abs(*xprev_begin - val);
      norm += diff * diff * diff;
      *xprev_begin++ = val;
    }
    return std::cbrt(norm) < tol;
  }

  const value_t tol;
  std::vector<value_t> xprev;
};

template <class value_t, class index_t> struct decision<value_t, index_t, 0> {
  decision(const value_t tol, const value_t threshold = 0)
      : tol{tol}, threshold{threshold} {}

  template <class InputIt1, class InputIt2>
  bool operator()(const index_t k, const value_t fval, InputIt1 x_begin,
                  InputIt1 x_end, InputIt2 g_begin) {
    const size_t d = std::distance(x_begin, x_end);
    if (xprev.size() != d) {
      xprev = std::vector<value_t>(d);
    }
    index_t count{0};
    std::size_t ind{0};
    while (x_begin != x_end) {
      const value_t val = *x_begin++;
      const value_t diff = std::abs(xprev[ind] - val);
      if (diff >= threshold)
        count++;
      xprev[ind++] = val;
    }
    return count < tol;
  }

private:
  const value_t tol;
  const value_t threshold;
  std::vector<value_t> xprev;
};
} // namespace terminator
} // namespace polo

#endif
