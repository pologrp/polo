#ifndef POLO_STEP_CUSTOM_HPP_
#define POLO_STEP_CUSTOM_HPP_

namespace polo {
namespace step {
template <class value_t, class index_t> struct custom {
  using init_t = void (*)(const value_t *, const value_t *, void *);
  using func_t = value_t (*)(const index_t, const index_t, const value_t,
                             const value_t *, const value_t *, const value_t *,
                             void *);

  custom() = default;

  value_t step(const index_t klocal, const index_t kglobal, const value_t fval,
               const value_t *xbegin, const value_t *xend,
               const value_t *gcurr) {
    return compute_(klocal, kglobal, fval, xbegin, xend, gcurr, data_);
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
} // namespace step
} // namespace polo

#endif
