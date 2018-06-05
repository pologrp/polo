#ifndef POLO_PROX_CUSTOM_HPP_
#define POLO_PROX_CUSTOM_HPP_

namespace polo {
namespace prox {

template <class value_t, class index_t> struct custom {
  using init_t = void (*)(const value_t *, const value_t *, void *);
  using func_t = value_t *(*)(const value_t, const value_t *, const value_t *,
                              const value_t *, value_t *, void *);

  custom() = default;

  value_t *prox(const value_t step, const value_t *xbegin, const value_t *xend,
                const value_t *gcurr, value_t *xcurr) {
    return compute_(step, xbegin, xend, gcurr, xcurr, data_);
  }

protected:
  void initialize(const value_t *xbegin, const value_t *xend) {
    init_(xbegin, xend, data_);
  }

  void parameters(init_t init, func_t compute, void *data) {
    init_ = init;
    compute_ = compute;
    data_ = data;
  }

  ~custom() = default;

private:
  init_t init_;
  func_t compute_;
  void *data_;
};
} // namespace prox
} // namespace polo

#endif
