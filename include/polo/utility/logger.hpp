#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <chrono>
#include <cstdint>
#include <type_traits>
#include <vector>

namespace polo {
namespace utility {
namespace logger {

template <class value_t, bool log_x_v, bool log_g_v> struct logger_t {
  template <class InputIt1, class InputIt2>
  void operator()(const std::size_t k, const value_t fval, InputIt1 xbegin,
                  InputIt1 xend, InputIt2 gbegin, InputIt2 gend) {
    tend = std::chrono::high_resolution_clock::now();
    const auto telapsed =
        std::chrono::duration<value_t, std::chrono::milliseconds::period>(
            tend - tstart);
    iterations.push_back(k);
    times.push_back(telapsed.count());
    fvalues.push_back(fval);
    log_x(xbegin, xend, std::integral_constant<bool, log_x_v>{});
    log_g(gbegin, gend, std::integral_constant<bool, log_g_v>{});
    tstart = std::chrono::high_resolution_clock::now();
  }

private:
  template <class InputIt>
  void log_x(InputIt xbegin, InputIt xend, std::true_type) {
    xvalues.emplace_back(xbegin, xend);
  }
  template <class InputIt> void log_x(InputIt, InputIt, std::false_type) {}
  template <class InputIt>
  void log_g(InputIt gbegin, InputIt gend, std::true_type) {
    gvalues.emplace_back(gbegin, gend);
  }
  template <class InputIt> void log_g(InputIt, InputIt, std::false_type) {}

  std::vector<std::size_t> iterations;
  std::vector<value_t> times;
  std::vector<value_t> fvalues;
  std::vector<std::vector<value_t>> xvalues, gvalues;
  std::chrono::time_point<std::chrono::high_resolution_clock> tstart{
      std::chrono::high_resolution_clock::now()},
      tend;
};

template <class value_t> using value = logger_t<value_t, false, false>;
template <class value_t> using decision = logger_t<value_t, true, false>;
template <class value_t> using gradient = logger_t<value_t, false, true>;
template <class value_t> using full = logger_t<value_t, true, true>;

} // namespace logger
} // namespace utility
} // namespace polo

#endif