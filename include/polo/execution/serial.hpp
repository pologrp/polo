#ifndef SERIAL_HPP_
#define SERIAL_HPP_

#include <iterator>
#include <vector>

namespace polo {
namespace execution {
template <class value_t> struct serial {
  serial() = default;

  serial(const serial &) = default;
  serial &operator=(const serial &) = default;
  serial(serial &&) = default;
  serial &operator=(serial &&) = default;

protected:
  template <class InputIt> void initialize(InputIt xbegin, InputIt xend) {
    x = std::vector<value_t>(xbegin, xend);
    g = std::vector<value_t>(x.size());
  }

  template <class T, class Func1, class Func2>
  void solve(T *algptr, Func1 &&loss, Func2 &&terminate) {
    value_t step;
    const value_t *xbegin{&x[0]};

    while (!terminate(k, fval, std::begin(x), std::end(x), std::begin(g))) {
      fval = loss(xbegin, &g[0]);
      algptr->grad(std::begin(g), std::end(g), std::begin(g));
      algptr->smooth(k, std::begin(x), std::end(x), std::begin(g),
                     std::begin(g));
      step = algptr->step(k, fval, std::begin(x), std::end(x), std::begin(g));
      algptr->project(step, std::begin(x), std::end(x), std::begin(g),
                      std::begin(x));
      k++;
    }
  }

  value_t getf() const { return fval; }
  std::vector<value_t> getx() const { return x; }

  ~serial() = default;

private:
  std::size_t k{1};
  value_t fval{0};
  std::vector<value_t> x, g;
};
} // namespace execution
} // namespace polo

#endif
