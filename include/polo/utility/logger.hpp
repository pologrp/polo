#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <chrono>
#include <cstdint>
#include <mutex>
#include <type_traits>
#include <vector>

namespace polo {
namespace utility {
namespace logger {
namespace detail {
template <class value_t, bool log_x_v, bool log_g_v> struct logger {
  template <class InputIt1, class InputIt2>
  void operator()(const std::size_t k, const value_t fval, InputIt1 xbegin,
                  InputIt1 xend, InputIt2 gbegin, InputIt2 gend) {
    std::lock_guard<std::mutex> lock(sync);
    tend = std::chrono::high_resolution_clock::now();
    const auto telapsed =
        std::chrono::duration<value_t, std::chrono::milliseconds::period>(
            tend - tstart);
    iterations.push_back(k);
    times.push_back(telapsed.count());
    fvalues.push_back(fval);
    log(xvalues, xbegin, xend, std::integral_constant<bool, log_x_v>{});
    log(gvalues, gbegin, gend, std::integral_constant<bool, log_g_v>{});
    tstart = std::chrono::high_resolution_clock::now();
  }

private:
  template <class Container, class InputIt>
  void log(Container &c, InputIt begin, InputIt end, std::true_type) {
    c.emplace_back(begin, end);
  }
  template <class Container, class InputIt>
  void log(Container &, InputIt, InputIt, std::false_type) {}

  std::mutex sync;
  std::vector<std::size_t> iterations;
  std::vector<value_t> times;
  std::vector<value_t> fvalues;
  std::vector<std::vector<value_t>> xvalues, gvalues;
  std::chrono::time_point<std::chrono::high_resolution_clock> tstart{
      std::chrono::high_resolution_clock::now()},
      tend;
};
} // namespace detail

template <class value_t> using value = detail::logger<value_t, false, false>;
template <class value_t> using decision = detail::logger<value_t, true, false>;
template <class value_t> using gradient = detail::logger<value_t, false, true>;
template <class value_t> using full = detail::logger<value_t, true, true>;
} // namespace logger
} // namespace utility
} // namespace polo

#endif
