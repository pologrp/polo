#ifndef POLO_UTILITY_CUSTOMTERM_HPP_
#define POLO_UTILITY_CUSTOMTERM_HPP_

namespace polo {
namespace utility {
namespace terminator {
template <class value_t, class index_t> struct custom {
  using func_t = bool *(*)(const index_t, const value_t, const value_t *,
                           const value_t *, const value_t *, void *);

  custom(func_t terminate, void *data) {
    terminate_ = terminate;
    data_ = data;
  }

  template <class InputIt1, class InputIt2>
  bool operator()(const index_t k, const value_t fval, InputIt1 x_begin,
                  InputIt1 x_end, InputIt2 g_begin) const {
    return terminate_(k, fval, x_begin, x_end, g_begin, data_);
  }

private:
  func_t terminate_;
  void *data_;
};
} // namespace terminator
} // namespace utility
} // namespace polo

#endif
