#ifndef NULL_HPP_
#define NULL_HPP_

namespace polo {
namespace utility {
namespace detail {
struct null {
  template <class... Args> void operator()(Args &&...) {}
};
} // namespace detail
} // namespace utility
} // namespace polo

#endif
