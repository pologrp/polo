#ifndef POLO_UTILITY_LOGGER_HPP_
#define POLO_UTILITY_LOGGER_HPP_

#include <chrono>
#include <cstddef>
#include <iterator>
#include <memory>
#include <mutex>
#include <ostream>
#include <type_traits>
#include <vector>

namespace polo {
namespace utility {
namespace logger {
namespace detail {
template <class value_t, class index_t, bool log_x_v, bool log_g_v>
class logger {
  template <class Container, class InputIt>
  void log(Container &c, InputIt begin, InputIt end, std::true_type) {
    c.emplace_back(begin, end);
  }
  template <class Container, class InputIt>
  void log(Container &, InputIt, InputIt, std::false_type) {}

  std::unique_ptr<std::mutex> sync{new std::mutex{}};
  std::vector<index_t> iterations;
  std::vector<value_t> times;
  std::vector<value_t> fvalues;
  std::vector<std::vector<value_t>> xvalues, gvalues;
  std::chrono::time_point<std::chrono::high_resolution_clock> tstart{
      std::chrono::high_resolution_clock::now()},
      tend;

  struct iterate_t {
    iterate_t() = default;
    iterate_t(const index_t *k, const value_t *t, const value_t *f)
        : k{k}, t{t}, f{f} {}
    iterate_t(const index_t *k, const value_t *t, const value_t *f,
              const std::vector<value_t> *x, std::true_type)
        : k{k}, t{t}, f{f}, x{x} {}
    iterate_t(const index_t *k, const value_t *t, const value_t *f,
              const std::vector<value_t> *g, std::false_type)
        : k{k}, t{t}, f{f}, g{g} {}
    iterate_t(const index_t *k, const value_t *t, const value_t *f,
              const std::vector<value_t> *x, const std::vector<value_t> *g)
        : k{k}, t{t}, f{f}, x{x}, g{g} {}

    const index_t &getk() const { return *k; }
    const value_t &gett() const { return *t; }
    const value_t &getf() const { return *f; }
    const std::vector<value_t> &getx() const {
      static_assert(log_x_v,
                    "getx is only defined for full and decision loggers");
      return *x;
    }
    const std::vector<value_t> &getg() const {
      static_assert(log_g_v,
                    "getg is only defined for full and gradient loggers");
      return *g;
    }
    iterate_t &delimiter(const char delim) {
      this->delim = delim;
      return *this;
    }

    friend std::ostream &operator<<(std::ostream &os, const iterate_t &iter) {
      os << *iter.k << iter.delim << *iter.t << iter.delim << *iter.f;
      return os;
    }

  private:
    char delim{','};
    const index_t *k{nullptr};
    const value_t *t{nullptr};
    const value_t *f{nullptr};
    const std::vector<value_t> *x{nullptr};
    const std::vector<value_t> *g{nullptr};
  };

  struct iterator_t {
    using difference_type = std::ptrdiff_t;
    using value_type = iterate_t;
    using pointer = value_type *;
    using reference = value_type &;
    using iterator_category = std::random_access_iterator_tag;

    iterator_t() = default;
    iterator_t(const logger *lptr, const std::ptrdiff_t n) : lptr{lptr}, n{n} {
      setiterate(std::integral_constant<bool, log_x_v>{},
                 std::integral_constant<bool, log_g_v>{});
    }

    reference operator*() { return iterate; }
    value_type const &operator*() const { return iterate; }
    pointer operator->() { return &iterate; }
    value_type const *operator->() const { return &iterate; }
    bool operator==(const iterator_t &rhs) const {
      return n == rhs.n && lptr == rhs.lptr;
    }
    bool operator!=(const iterator_t &rhs) const { return !operator==(rhs); }
    iterator_t &operator++() {
      n++;
      setiterate(std::integral_constant<bool, log_x_v>{},
                 std::integral_constant<bool, log_g_v>{});
      return *this;
    }
    iterator_t operator++(int) {
      iterator_t ip{*this};
      operator++();
      return ip;
    }
    iterator_t &operator--() {
      n--;
      setiterate(std::integral_constant<bool, log_x_v>{},
                 std::integral_constant<bool, log_g_v>{});
      return *this;
    }
    iterator_t operator--(int) {
      iterator_t ip{*this};
      operator--();
      return ip;
    }
    iterator_t &operator+=(const difference_type n) {
      this->n += n;
      setiterate(std::integral_constant<bool, log_x_v>{},
                 std::integral_constant<bool, log_g_v>{});
      return *this;
    }
    friend iterator_t operator+(const iterator_t it, const difference_type n) {
      iterator_t temp{it};
      return temp += n;
    }
    friend iterator_t operator+(const difference_type n, const iterator_t it) {
      return it + n;
    }
    iterator_t &operator-=(const difference_type n) {
      this->n += -n;
      setiterate(std::integral_constant<bool, log_x_v>{},
                 std::integral_constant<bool, log_g_v>{});
      return *this;
    }
    friend iterator_t operator-(const iterator_t it, const difference_type n) {
      iterator_t temp{it};
      return temp -= n;
    }
    difference_type operator-(const iterator_t rhs) const { return n - rhs.n; }
    value_type operator[](const difference_type n) const {
      return *(*this + n);
    }
    bool operator<(const iterator_t rhs) const { return rhs.n - n > 0; }
    bool operator>(const iterator_t rhs) const { return rhs < *this; }
    bool operator>=(const iterator_t rhs) const { return !operator<(rhs); }
    bool operator<=(const iterator_t rhs) const { return !operator>(rhs); }

  private:
    void setiterate(std::true_type, std::true_type) {
      iterate = value_type{&lptr->iterations[0] + n, &lptr->times[0] + n,
                           &lptr->fvalues[0] + n, &lptr->xvalues[0] + n,
                           &lptr->gvalues[0] + n};
    }
    void setiterate(std::true_type, std::false_type) {
      iterate = value_type{&lptr->iterations[0] + n, &lptr->times[0] + n,
                           &lptr->fvalues[0] + n, &lptr->xvalues[0] + n,
                           std::true_type{}};
    }
    void setiterate(std::false_type, std::true_type) {
      iterate = value_type{&lptr->iterations[0] + n, &lptr->times[0] + n,
                           &lptr->fvalues[0] + n, &lptr->gvalues[0] + n,
                           std::false_type{}};
    }
    void setiterate(std::false_type, std::false_type) {
      iterate = value_type{&lptr->iterations[0] + n, &lptr->times[0] + n,
                           &lptr->fvalues[0] + n};
    }

    const logger *lptr{nullptr};
    std::ptrdiff_t n{0};
    value_type iterate;
  };

public:
  template <class InputIt1, class InputIt2>
  void operator()(const index_t k, const value_t fval, InputIt1 xbegin,
                  InputIt1 xend, InputIt2 gbegin) {
    std::lock_guard<std::mutex> lock(*sync);
    tend = std::chrono::high_resolution_clock::now();
    const auto telapsed =
        std::chrono::duration<value_t, std::chrono::milliseconds::period>(
            tend - tstart);
    iterations.push_back(k);
    times.push_back(telapsed.count());
    fvalues.push_back(fval);
    InputIt2 gend = gbegin + std::distance(xbegin, xend);
    log(xvalues, xbegin, xend, std::integral_constant<bool, log_x_v>{});
    log(gvalues, gbegin, gend, std::integral_constant<bool, log_g_v>{});
  }

  iterator_t begin() const { return iterator_t(this, 0); }
  iterator_t end() const { return iterator_t(this, iterations.size()); }
};
} // namespace detail

template <class value_t = double, class index_t = int>
using value = detail::logger<value_t, index_t, false, false>;
template <class value_t = double, class index_t = int>
using decision = detail::logger<value_t, index_t, true, false>;
template <class value_t = double, class index_t = int>
using gradient = detail::logger<value_t, index_t, false, true>;
template <class value_t = double, class index_t = int>
using full = detail::logger<value_t, index_t, true, true>;
} // namespace logger
} // namespace utility
} // namespace polo

#endif
