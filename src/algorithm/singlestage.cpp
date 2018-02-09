#ifndef SINGLESTAGE_CPP_
#define SINGLESTAGE_CPP_

#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

#include "execution/serial.cpp"
#include "projection/none.cpp"
#include "smoothing/none.cpp"
#include "step/constant.cpp"

using namespace pbopt;

namespace pbopt {
namespace algorithm {

template <class float_t, template <class> class Descent,
          template <class> class StepSize = step::constant,
          template <class> class Smoothing = smoothing::none,
          template <class> class Projection = projection::none,
          template <class> class Execution = execution::serial>
class singlestage : private Descent<float_t>,
                    private StepSize<float_t>,
                    private Smoothing<float_t>,
                    private Projection<float_t>,
                    private Execution<float_t> {
  struct iterator_t;
  using loss_t = float_t (*)(const float_t *, const float_t *, float_t *,
                             void *);

public:
  singlestage() = default;

  template <class... Ts> void descent_params(Ts &&... params) {
    Descent<float_t>::set(std::forward<Ts>(params)...);
  }
  template <class... Ts> void step_params(Ts &&... params) {
    StepSize<float_t>::set(std::forward<Ts>(params)...);
  }
  template <class... Ts> void smoothing_params(Ts &&... params) {
    Smoothing<float_t>::set(std::forward<Ts>(params)...);
  }

  template <class T> void termination_condition(T t) { terminated = t; }

  void set_loss_func(loss_t loss_func) { this->loss_func = loss_func; }
  void set_loss_data(void *loss_data) { this->loss_data = loss_data; }

  iterator_t begin() { return iterator_t{this, false}; }
  iterator_t end() { return iterator_t{this, true}; }

  void solve() {
    while (
        !terminated(this->vals.k, this->vals.fval, this->vals.x, this->vals.g))
      step();
  }

  template <class ForwardIt> void init(ForwardIt &&x_begin, ForwardIt &&x_end) {
    Descent<float_t>::init(std::forward<ForwardIt>(x_begin),
                           std::forward<ForwardIt>(x_end));
    Smoothing<float_t>::init(std::forward<ForwardIt>(x_begin),
                             std::forward<ForwardIt>(x_end));
    vals = algorithm_info(std::forward<ForwardIt>(x_begin),
                          std::forward<ForwardIt>(x_end));
  }

private:
  struct algorithm_info {
    std::size_t k{1};
    float_t fval{0};
    std::vector<float_t> x, d, g;

    algorithm_info() = default;

    template <class InputIt>
    algorithm_info(InputIt &&x_begin, InputIt &&x_end)
        : k{1}, fval{0},
          x(std::forward<InputIt>(x_begin), std::forward<InputIt>(x_end)),
          d{std::vector<float_t>(x.size())}, g{std::vector<float_t>(x.size())} {
    }
  };

  algorithm_info vals;

  struct iterator_t {
    iterator_t(singlestage *algorithm, bool terminated = false)
        : algorithm{algorithm}, terminated{terminated} {}

    bool operator!=(const iterator_t &rhs) const {
      return this->terminated != rhs.terminated;
    }
    const algorithm_info &operator*() const { return algorithm->vals; }
    const algorithm_info *operator->() const { return &algorithm->vals; }
    iterator_t &operator++() {
      const std::size_t dim = algorithm->vals.x.size();
      algorithm->step();
      terminated = algorithm->terminated(
          algorithm->vals.k, algorithm->vals.fval, algorithm->vals.x.data(),
          algorithm->vals.x.data() + dim, algorithm->vals.g.data());
      return *this;
    }
    iterator_t operator++(int) {
      iterator_t temp{*this};
      operator++();
      return temp;
    }

  private:
    singlestage *algorithm;
    bool terminated;
  };

  loss_t loss_func{nullptr};
  void *loss_data{nullptr};

  std::function<bool(const std::size_t, const float_t, const float_t *,
                     const float_t *, const float_t *)>
      terminated;

  void step() {
    float_t *xbegin = vals.x.data();
    float_t *dbegin = vals.d.data();
    float_t *gbegin = vals.g.data();
    const std::size_t dim = vals.x.size();

    vals.fval = loss_func(xbegin, xbegin + dim, dbegin, loss_data);
    Descent<float_t>::direction(dbegin, dbegin + dim, dbegin);

    Smoothing<float_t>::smooth(dbegin, dbegin + dim, xbegin, vals.k, gbegin);
    float_t sz = StepSize<float_t>::get(gbegin, gbegin + dim, xbegin, vals.k);
    Projection<float_t>::project(gbegin, gbegin + dim, xbegin, sz, xbegin);
    vals.k++;
  }
};

} // namespace algorithm
} // namespace pbopt

#endif
