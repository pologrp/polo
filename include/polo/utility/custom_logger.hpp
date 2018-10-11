#ifndef POLO_UTILITY_CUSTOMLOG_HPP_
#define POLO_UTILITY_CUSTOMLOG_HPP_

namespace polo {
namespace utility {
namespace logger {
template <class value_t, class index_t> struct custom {
  using func_t = void (*)(const index_t, const value_t, const value_t *,
                          const value_t *, const value_t *, void *);

  custom(func_t log, void *data) : log_{log}, data_{data} {}

  void operator()(const index_t k, const value_t fval, const value_t *x_begin,
                  const value_t *x_end, const value_t *g_begin) const {
    log_(k, fval, x_begin, x_end, g_begin, data_);
  }

private:
  func_t log_;
  void *data_;
};
} // namespace logger
} // namespace utility
} // namespace polo

#endif
