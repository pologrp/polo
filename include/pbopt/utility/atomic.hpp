#ifndef ATOMIC_HPP_
#define ATOMIC_HPP_

#include <atomic>
#include <cstdint>

namespace pbopt {
namespace utility {

template <class float_t, class int_t> struct atomic {
  using value_type = float_t;
  using difference_type = value_type;

  atomic() noexcept = default;
  constexpr atomic(float_t desired) noexcept
      : value(reinterpret_cast<int_t &>(desired)) {}
  atomic(const atomic &) = delete;

  float_t operator=(float_t desired) noexcept {
    const int_t value = (this->value = reinterpret_cast<int_t &>(desired));
    return reinterpret_cast<const float_t &>(value);
  }
  float_t operator=(float_t desired) volatile noexcept {
    const int_t value = (this->value = reinterpret_cast<int_t &>(desired));
    return reinterpret_cast<const float_t &>(value);
  }
  atomic &operator=(const atomic &) = delete;
  atomic &operator=(const atomic &) volatile = delete;

  operator float_t() const noexcept {
    const int_t value = this->value;
    return reinterpret_cast<const float_t &>(value);
  }
  operator float_t() const volatile noexcept {
    const int_t value = this->value;
    return reinterpret_cast<const float_t &>(value);
  }

  float_t operator+=(float_t rhs) noexcept {
    float_t old_value, new_value;
    do {
      old_value = *this;
      new_value = old_value + rhs;
    } while (
        !value.compare_exchange_weak(reinterpret_cast<int_t &>(old_value),
                                     reinterpret_cast<int_t &>(new_value)));
    return new_value;
  }
  float_t operator+=(float_t rhs) volatile noexcept {
    float_t old_value, new_value;
    do {
      old_value = *this;
      new_value = old_value + rhs;
    } while (
        !value.compare_exchange_weak(reinterpret_cast<int_t &>(old_value),
                                     reinterpret_cast<int_t &>(new_value)));
    return new_value;
  }

  float_t operator-=(float_t rhs) noexcept {
    float_t old_value, new_value;
    do {
      old_value = *this;
      new_value = old_value - rhs;
    } while (
        !value.compare_exchange_weak(reinterpret_cast<int_t &>(old_value),
                                     reinterpret_cast<int_t &>(new_value)));
    return new_value;
  }
  float_t operator-=(float_t rhs) volatile noexcept {
    float_t old_value, new_value;
    do {
      old_value = *this;
      new_value = old_value - rhs;
    } while (
        !value.compare_exchange_weak(reinterpret_cast<int_t &>(old_value),
                                     reinterpret_cast<int_t &>(new_value)));
    return new_value;
  }

  float_t operator*=(float_t rhs) noexcept {
    float_t old_value, new_value;
    do {
      old_value = *this;
      new_value = old_value * rhs;
    } while (
        !value.compare_exchange_weak(reinterpret_cast<int_t &>(old_value),
                                     reinterpret_cast<int_t &>(new_value)));
    return new_value;
  }
  float_t operator*=(float_t rhs) volatile noexcept {
    float_t old_value, new_value;
    do {
      old_value = *this;
      new_value = old_value * rhs;
    } while (
        !value.compare_exchange_weak(reinterpret_cast<int_t &>(old_value),
                                     reinterpret_cast<int_t &>(new_value)));
    return new_value;
  }

  float_t operator/=(float_t rhs) noexcept {
    float_t old_value, new_value;
    do {
      old_value = *this;
      new_value = old_value / rhs;
    } while (
        !value.compare_exchange_weak(reinterpret_cast<int_t &>(old_value),
                                     reinterpret_cast<int_t &>(new_value)));
    return new_value;
  }
  float_t operator/=(float_t rhs) volatile noexcept {
    float_t old_value, new_value;
    do {
      old_value = *this;
      new_value = old_value / rhs;
    } while (
        !value.compare_exchange_weak(reinterpret_cast<int_t &>(old_value),
                                     reinterpret_cast<int_t &>(new_value)));
    return new_value;
  }

private:
  std::atomic<int_t> value;
};

using atomic_float = atomic<float, std::uint32_t>;
using atomic_double = atomic<double, std::uint64_t>;

} // namespace utility
} // namespace pbopt

#endif
