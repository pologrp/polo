#ifndef SINGLESTAGE_HPP_
#define SINGLESTAGE_HPP_

#include <iterator>
#include <utility>
#include <vector>

#include "polo/execution/serial.hpp"
#include "polo/gradient/none.hpp"
#include "polo/projection/none.hpp"
#include "polo/smoothing/none.hpp"
#include "polo/step/constant.hpp"
#include "polo/utility/maxiter.hpp"

namespace polo {
namespace algorithm {

template <class value_t, template <class> class Gradient = polo::gradient::none,
          template <class> class StepSize = polo::step::constant,
          template <class> class Smoothing = polo::smoothing::none,
          template <class> class Projection = polo::projection::none,
          template <class> class Execution = polo::execution::serial>
struct singlestage : public Gradient<value_t>,
                     public StepSize<value_t>,
                     public Smoothing<value_t>,
                     public Projection<value_t>,
                     public Execution<value_t> {
  singlestage() = default;
  template <class ForwardIt> singlestage(ForwardIt &&xbegin, ForwardIt &&xend) {
    initialize(std::forward<ForwardIt>(xbegin), std::forward<ForwardIt>(xend));
  }
  singlestage(const std::vector<value_t> &x)
      : singlestage(std::begin(x), std::end(x)) {}

  template <class ForwardIt> void initialize(ForwardIt xbegin, ForwardIt xend) {
    Gradient<value_t>::initialize(xbegin, xend);
    StepSize<value_t>::initialize(xbegin, xend);
    Smoothing<value_t>::initialize(xbegin, xend);
    Projection<value_t>::initialize(xbegin, xend);
    Execution<value_t>::initialize(xbegin, xend);
  }

  void initialize(const std::vector<value_t> &x) {
    initialize(std::begin(x), std::end(x));
  }

  template <class... Ts> void gradient_params(Ts &&... params) {
    Gradient<value_t>::params(std::forward<Ts>(params)...);
  }
  template <class... Ts> void step_params(Ts &&... params) {
    StepSize<value_t>::params(std::forward<Ts>(params)...);
  }
  template <class... Ts> void smoothing_params(Ts &&... params) {
    Smoothing<value_t>::params(std::forward<Ts>(params)...);
  }
  template <class... Ts> void projection_params(Ts &&... params) {
    Projection<value_t>::params(std::forward<Ts>(params)...);
  }
  template <class... Ts> void execution_params(Ts &&... params) {
    Execution<value_t>::params(std::forward<Ts>(params)...);
  }

  template <class Func1, class Func2>
  void solve(Func1 &&loss, Func2 &&terminate) {
    Execution<value_t>::solve(this, std::forward<Func1>(loss),
                              std::forward<Func2>(terminate));
  }

  template <class Func> void solve(Func &&loss) {
    solve(std::forward<Func>(loss), utility::terminator::maxiter{100});
  }

  value_t getf() const { return Execution<value_t>::getf(); }
  std::vector<value_t> getx() const { return Execution<value_t>::getx(); }
};

} // namespace algorithm
} // namespace polo

#endif
