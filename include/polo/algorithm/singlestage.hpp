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
#include "polo/utility/null.hpp"
#include "polo/utility/sampler.hpp"

namespace polo {
namespace algorithm {
template <class value_t = double, class index_t = int,
          template <class, class> class Gradient = gradient::none,
          template <class, class> class StepSize = step::constant,
          template <class, class> class Smoothing = smoothing::none,
          template <class, class> class Projection = projection::none,
          template <class, class> class Execution = execution::serial>
struct singlestage : public Gradient<value_t, index_t>,
                     public StepSize<value_t, index_t>,
                     public Smoothing<value_t, index_t>,
                     public Projection<value_t, index_t>,
                     public Execution<value_t, index_t> {
  singlestage() = default;
  template <class ForwardIt> singlestage(ForwardIt xbegin, ForwardIt xend) {
    initialize(xbegin, xend);
  }
  template <class T>
  singlestage(const std::vector<T> &x)
      : singlestage(std::begin(x), std::end(x)) {}

  template <class ForwardIt> void initialize(ForwardIt xbegin, ForwardIt xend) {
    std::vector<value_t> x =
        Execution<value_t, index_t>::initialize(xbegin, xend);
    Gradient<value_t, index_t>::initialize(std::begin(x), std::end(x));
    StepSize<value_t, index_t>::initialize(std::begin(x), std::end(x));
    Smoothing<value_t, index_t>::initialize(std::begin(x), std::end(x));
    Projection<value_t, index_t>::initialize(std::begin(x), std::end(x));
  }
  template <class T> void initialize(const std::vector<T> &x) {
    initialize(std::begin(x), std::end(x));
  }

  template <class... Ts> void gradient_params(Ts &&... params) {
    Gradient<value_t, index_t>::params(std::forward<Ts>(params)...);
  }
  template <class... Ts> void step_params(Ts &&... params) {
    StepSize<value_t, index_t>::params(std::forward<Ts>(params)...);
  }
  template <class... Ts> void smoothing_params(Ts &&... params) {
    Smoothing<value_t, index_t>::params(std::forward<Ts>(params)...);
  }
  template <class... Ts> void projection_params(Ts &&... params) {
    Projection<value_t, index_t>::params(std::forward<Ts>(params)...);
  }
  template <class... Ts> void execution_params(Ts &&... params) {
    Execution<value_t, index_t>::params(std::forward<Ts>(params)...);
  }

  template <class Loss, class Terminator, class Logger>
  void solve(Loss &&loss, Terminator &&terminator, Logger &&logger) {
    Execution<value_t, index_t>::solve(this, std::forward<Loss>(loss),
                                       std::forward<Terminator>(terminator),
                                       std::forward<Logger>(logger));
  }
  template <class Loss, class Terminator>
  void solve(Loss &&loss, Terminator &&terminator) {
    solve(std::forward<Loss>(loss), std::forward<Terminator>(terminator),
          utility::detail::null{});
  }
  template <class Loss> void solve(Loss &&loss) {
    solve(std::forward<Loss>(loss),
          utility::terminator::maxiter<value_t, index_t>{100},
          utility::detail::null{});
  }

  template <class Loss, class Sampler, class Terminator, class Logger>
  void solve(Loss &&loss, utility::sampler::detail::component_sampler_t s,
             Sampler &&sampler, const index_t num_components,
             Terminator &&terminator, Logger &&logger) {
    Execution<value_t, index_t>::solve(
        this, std::forward<Loss>(loss), std::forward<Terminator>(terminator),
        std::forward<Logger>(logger), s, std::forward<Sampler>(sampler),
        num_components);
  }
  template <class Loss, class Sampler, class Terminator>
  void solve(Loss &&loss, utility::sampler::detail::component_sampler_t s,
             Sampler &&sampler, const index_t num_components,
             Terminator &&terminator) {
    solve(std::forward<Loss>(loss), s, std::forward<Sampler>(sampler),
          num_components, std::forward<Terminator>(terminator),
          utility::detail::null{});
  }
  template <class Loss, class Sampler>
  void solve(Loss &&loss, utility::sampler::detail::component_sampler_t s,
             Sampler &&sampler, const index_t num_components) {
    solve(std::forward<Loss>(loss), s, std::forward<Sampler>(sampler),
          num_components, utility::terminator::maxiter<value_t, index_t>{100},
          utility::detail::null{});
  }

  template <class Loss, class Sampler, class Terminator, class Logger>
  void solve(Loss &&loss, utility::sampler::detail::coordinate_sampler_t s,
             Sampler &&sampler, const index_t num_coordinates,
             Terminator &&terminator, Logger &&logger) {
    Execution<value_t, index_t>::solve(
        this, std::forward<Loss>(loss), std::forward<Terminator>(terminator),
        std::forward<Logger>(logger), s, std::forward<Sampler>(sampler),
        num_coordinates);
  }
  template <class Loss, class Sampler, class Terminator>
  void solve(Loss &&loss, utility::sampler::detail::coordinate_sampler_t s,
             Sampler &&sampler, const index_t num_coordinates,
             Terminator &&terminator) {
    solve(std::forward<Loss>(loss), s, std::forward<Sampler>(sampler),
          num_coordinates, std::forward<Terminator>(terminator),
          utility::detail::null{});
  }
  template <class Loss, class Sampler>
  void solve(Loss &&loss, utility::sampler::detail::coordinate_sampler_t s,
             Sampler &&sampler, const index_t num_coordinates) {
    solve(std::forward<Loss>(loss), s, std::forward<Sampler>(sampler),
          num_coordinates, utility::terminator::maxiter<value_t, index_t>{100},
          utility::detail::null{});
  }

  template <class Loss, class Sampler1, class Sampler2, class Terminator,
            class Logger>
  void solve(Loss &&loss, utility::sampler::detail::component_sampler_t s1,
             Sampler1 &&sampler1, const index_t num_components,
             utility::sampler::detail::coordinate_sampler_t s2,
             Sampler2 &&sampler2, const index_t num_coordinates,
             Terminator &&terminator, Logger &&logger) {
    Execution<value_t, index_t>::solve(
        this, std::forward<Loss>(loss), std::forward<Terminator>(terminator),
        std::forward<Logger>(logger), s1, std::forward<Sampler1>(sampler1),
        num_components, s2, std::forward<Sampler2>(sampler2), num_coordinates);
  }
  template <class Loss, class Sampler1, class Sampler2, class Terminator>
  void solve(Loss &&loss, utility::sampler::detail::component_sampler_t s1,
             Sampler1 &&sampler1, const index_t num_components,
             utility::sampler::detail::coordinate_sampler_t s2,
             Sampler2 &&sampler2, const index_t num_coordinates,
             Terminator &&terminator) {
    solve(std::forward<Loss>(loss), s1, std::forward<Sampler1>(sampler1),
          num_components, s2, std::forward<Sampler2>(sampler2), num_coordinates,
          std::forward<Terminator>(terminator), utility::detail::null{});
  }
  template <class Loss, class Sampler1, class Sampler2>
  void solve(Loss &&loss, utility::sampler::detail::component_sampler_t s1,
             Sampler1 &&sampler1, const index_t num_components,
             utility::sampler::detail::coordinate_sampler_t s2,
             Sampler2 &&sampler2, const index_t num_coordinates) {
    solve(std::forward<Loss>(loss), s1, std::forward<Sampler1>(sampler1),
          num_components, s2, std::forward<Sampler2>(sampler2), num_coordinates,
          utility::terminator::maxiter<value_t, index_t>{100},
          utility::detail::null{});
  }

  value_t getf() const { return Execution<value_t, index_t>::getf(); }
  std::vector<value_t> getx() const {
    return Execution<value_t, index_t>::getx();
  }
};
} // namespace algorithm
} // namespace polo

#endif
