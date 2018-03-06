#ifndef NULL_HPP_
#define NULL_HPP_

namespace polo {
namespace utility {
namespace detail {
struct null {
  void operator()(...) {}
};
} // namespace detail
} // namespace utility
} // namespace polo

#endif
