#ifndef SINGLESTAGE_CPP_
#define SINGLESTAGE_CPP_

#include <utility>
#include <vector>

#include "execution/serial.cpp"
#include "projection/none.cpp"
#include "smoothing/none.cpp"
#include "step/constant.cpp"
#include "utility/maxiter.cpp"

using namespace pbopt;

namespace pbopt {
namespace algorithm {

template <class float_t, template <class> class Gradient,
          template <class> class StepSize = step::constant,
          template <class> class Smoothing = smoothing::none,
          template <class> class Projection = projection::none,
          template <class> class Execution = execution::serial>
struct singlestage : private Gradient<float_t>,
                     private StepSize<float_t>,
                     private Smoothing<float_t>,
                     private Projection<float_t>,
                     private Execution<float_t> {
  singlestage() = default;
  template <class ForwardIt> singlestage(ForwardIt &&xbegin, ForwardIt &&xend) {
    initialize(std::forward<ForwardIt>(xbegin), std::forward<ForwardIt>(xend));
  }

  template <class ForwardIt> void initialize(ForwardIt xbegin, ForwardIt xend) {
    Gradient<float_t>::initialize(xbegin, xend);
    StepSize<float_t>::initialize(xbegin, xend);
    Smoothing<float_t>::initialize(xbegin, xend);
    Projection<float_t>::initialize(xbegin, xend);
    Execution<float_t>::initialize(xbegin, xend);
    x = std::vector<float_t>(xbegin, xend);
    g = std::vector<float_t>(x.size());
  }

  template <class... Ts> void gradient_params(Ts &&... params) {
    Gradient<float_t>::params(std::forward<Ts>(params)...);
  }
  template <class... Ts> void step_params(Ts &&... params) {
    StepSize<float_t>::params(std::forward<Ts>(params)...);
  }
  template <class... Ts> void smoothing_params(Ts &&... params) {
    Smoothing<float_t>::params(std::forward<Ts>(params)...);
  }
  template <class... Ts> void projection_params(Ts &&... params) {
    Projection<float_t>::params(std::forward<Ts>(params)...);
  }
  template <class... Ts> void execution_params(Ts &&... params) {
    Execution<float_t>::params(std::forward<Ts>(params)...);
  }

  template <class Func> void solve(Func &&loss) {
    solve(std::forward<Func>(loss), utility::maxiter{10000});
  }

  template <class Func1, class Func2>
  void solve(Func1 &&loss, Func2 &&terminate) {
    const std::size_t dim = x.size();
    const float_t *xbegin = x.data();
    const float_t *xend = x.data() + dim;
    float_t step;
    while (!terminate(k, fval, xbegin, xend, g.data())) {
      fval = loss(xbegin, xend, g.data());
      Gradient<float_t>::grad(g.data(), g.data() + dim, g.data());
      Smoothing<float_t>::smooth(k, xbegin, xend, g.data(), g.data());
      step = StepSize<float_t>::step(k, fval, xbegin, xend, g.data());
      Projection<float_t>::project(step, xbegin, xend, g.data(), x.data());
      k++;
    }
  }

  float_t getf() const { return fval; }
  std::vector<float_t> getx() const { return x; }

private:
  std::size_t k{1};
  float_t fval{0};
  std::vector<float_t> x, g;
};

} // namespace algorithm
} // namespace pbopt

#endif
