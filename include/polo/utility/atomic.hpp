#ifndef ATOMIC_HPP_
#define ATOMIC_HPP_

#include <atomic>
#include <cstdint>

namespace polo {
namespace utility {
namespace detail {
template <class value_t, class block_t> struct atomic {
  using value_type = value_t;
  using difference_type = value_type;

  atomic() noexcept = default;
  constexpr atomic(value_t desired) noexcept
      : value(reinterpret_cast<block_t &>(desired)) {}
  atomic(const atomic &) = delete;

  value_t operator=(value_t desired) noexcept {
    const block_t value = (this->value = reinterpret_cast<block_t &>(desired));
    return reinterpret_cast<const value_t &>(value);
  }
  value_t operator=(value_t desired) volatile noexcept {
    const block_t value = (this->value = reinterpret_cast<block_t &>(desired));
    return reinterpret_cast<const value_t &>(value);
  }
  atomic &operator=(const atomic &) = delete;
  atomic &operator=(const atomic &) volatile = delete;

  operator value_t() const noexcept {
    const block_t value = this->value;
    return reinterpret_cast<const value_t &>(value);
  }
  operator value_t() const volatile noexcept {
    const block_t value = this->value;
    return reinterpret_cast<const value_t &>(value);
  }

  value_t operator+=(value_t rhs) noexcept {
    value_t old_value, new_value;
    do {
      old_value = *this;
      new_value = old_value + rhs;
    } while (
        !value.compare_exchange_weak(reinterpret_cast<block_t &>(old_value),
                                     reinterpret_cast<block_t &>(new_value)));
    return new_value;
  }
  value_t operator+=(value_t rhs) volatile noexcept {
    value_t old_value, new_value;
    do {
      old_value = *this;
      new_value = old_value + rhs;
    } while (
        !value.compare_exchange_weak(reinterpret_cast<block_t &>(old_value),
                                     reinterpret_cast<block_t &>(new_value)));
    return new_value;
  }

  value_t operator-=(value_t rhs) noexcept {
    value_t old_value, new_value;
    do {
      old_value = *this;
      new_value = old_value - rhs;
    } while (
        !value.compare_exchange_weak(reinterpret_cast<block_t &>(old_value),
                                     reinterpret_cast<block_t &>(new_value)));
    return new_value;
  }
  value_t operator-=(value_t rhs) volatile noexcept {
    value_t old_value, new_value;
    do {
      old_value = *this;
      new_value = old_value - rhs;
    } while (
        !value.compare_exchange_weak(reinterpret_cast<block_t &>(old_value),
                                     reinterpret_cast<block_t &>(new_value)));
    return new_value;
  }

  value_t operator*=(value_t rhs) noexcept {
    value_t old_value, new_value;
    do {
      old_value = *this;
      new_value = old_value * rhs;
    } while (
        !value.compare_exchange_weak(reinterpret_cast<block_t &>(old_value),
                                     reinterpret_cast<block_t &>(new_value)));
    return new_value;
  }
  value_t operator*=(value_t rhs) volatile noexcept {
    value_t old_value, new_value;
    do {
      old_value = *this;
      new_value = old_value * rhs;
    } while (
        !value.compare_exchange_weak(reinterpret_cast<block_t &>(old_value),
                                     reinterpret_cast<block_t &>(new_value)));
    return new_value;
  }

  value_t operator/=(value_t rhs) noexcept {
    value_t old_value, new_value;
    do {
      old_value = *this;
      new_value = old_value / rhs;
    } while (
        !value.compare_exchange_weak(reinterpret_cast<block_t &>(old_value),
                                     reinterpret_cast<block_t &>(new_value)));
    return new_value;
  }
  value_t operator/=(value_t rhs) volatile noexcept {
    value_t old_value, new_value;
    do {
      old_value = *this;
      new_value = old_value / rhs;
    } while (
        !value.compare_exchange_weak(reinterpret_cast<block_t &>(old_value),
                                     reinterpret_cast<block_t &>(new_value)));
    return new_value;
  }

private:
  std::atomic<block_t> value;
};
} // namespace detail

using atomic_float = detail::atomic<float, std::uint32_t>;
using atomic_double = detail::atomic<double, std::uint64_t>;
} // namespace utility
} // namespace polo

#endif
