#ifndef SERIAL_CPP_
#define SERIAL_CPP_

namespace kth {
namespace optim {
namespace execution {

template <class float_t> struct serial {
  serial() = default;

  // Usual suspects; needed due to the protected destructor.
  serial(const serial &) = default;
  serial &operator=(const serial &) = default;
  serial(serial &&) = default;
  serial &operator=(serial &&) = default;

protected:
  ~serial() = default;
};

} // namespace execution
} // namespace optim
} // namespace kth

#endif
