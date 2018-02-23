#ifndef SINGLESTAGE_HPP_
#define SINGLESTAGE_HPP_

#include <iterator>
#include <utility>
#include <vector>

#include "pbopt/execution/serial.hpp"
#include "pbopt/gradient/none.hpp"
#include "pbopt/projection/none.hpp"
#include "pbopt/smoothing/none.hpp"
#include "pbopt/step/constant.hpp"
#include "pbopt/utility/maxiter.hpp"

namespace pbopt {
namespace algorithm {

template <class float_t,
          template <class> class Gradient = pbopt::gradient::none,
          template <class> class StepSize = pbopt::step::constant,
          template <class> class Smoothing = pbopt::smoothing::none,
          template <class> class Projection = pbopt::projection::none,
          template <class> class Execution = pbopt::execution::serial>
struct singlestage : public Gradient<float_t>,
                     public StepSize<float_t>,
                     public Smoothing<float_t>,
                     public Projection<float_t>,
                     public Execution<float_t> {
  singlestage() = default;
  template <class ForwardIt> singlestage(ForwardIt &&xbegin, ForwardIt &&xend) {
    initialize(std::forward<ForwardIt>(xbegin), std::forward<ForwardIt>(xend));
  }
  singlestage(const std::vector<float_t> &x)
      : singlestage(std::begin(x), std::end(x)) {}

  template <class ForwardIt> void initialize(ForwardIt xbegin, ForwardIt xend) {
    Gradient<float_t>::initialize(xbegin, xend);
    StepSize<float_t>::initialize(xbegin, xend);
    Smoothing<float_t>::initialize(xbegin, xend);
    Projection<float_t>::initialize(xbegin, xend);
    Execution<float_t>::initialize(xbegin, xend);
  }

  void initialize(const std::vector<float_t> &x) {
    initialize(std::begin(x), std::end(x));
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

  template <class Func1, class Func2>
  void solve(Func1 &&loss, Func2 &&terminate) {
    Execution<float_t>::solve(this, std::forward<Func1>(loss),
                              std::forward<Func2>(terminate));
  }

  template <class Func> void solve(Func &&loss) {
    solve(std::forward<Func>(loss), utility::terminator::maxiter{10000});
  }

  float_t getf() const { return Execution<float_t>::getf(); }
  std::vector<float_t> getx() const { return Execution<float_t>::getx(); }
};

} // namespace algorithm
} // namespace pbopt

#endif
