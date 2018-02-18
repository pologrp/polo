#ifndef SERIAL_HPP_
#define SERIAL_HPP_

#include <vector>

namespace pbopt {
namespace execution {

template <class float_t> struct serial {
  serial() = default;

  serial(const serial &) = default;
  serial &operator=(const serial &) = default;
  serial(serial &&) = default;
  serial &operator=(serial &&) = default;

protected:
  template <class InputIt> void initialize(InputIt xbegin, InputIt xend) {
    x = std::vector<float_t>(xbegin, xend);
    g = std::vector<float_t>(x.size());
  }

  template <class T, class Func1, class Func2>
  void solve(T *algptr, Func1 &&loss, Func2 &&terminate) {
    const std::size_t dim = x.size();
    const float_t *xbegin = x.data();
    const float_t *xend = x.data() + dim;
    float_t step;
    while (!terminate(k, fval, xbegin, xend, g.data())) {
      fval = loss(xbegin, xend, g.data());
      algptr->grad(g.data(), g.data() + dim, g.data());
      algptr->smooth(k, xbegin, xend, g.data(), g.data());
      step = algptr->step(k, fval, xbegin, xend, g.data());
      algptr->project(step, xbegin, xend, g.data(), x.data());
      k++;
    }
  }

  float_t getf() const { return fval; }
  std::vector<float_t> getx() const { return x; }

  ~serial() = default;

private:
  std::size_t k{1};
  float_t fval{0};
  std::vector<float_t> x, g;
};

} // namespace execution
} // namespace pbopt

#endif
