#ifndef POLO_ALGORITHM_PROXGRADIENT_HPP_
#define POLO_ALGORITHM_PROXGRADIENT_HPP_

#include <iterator>
#include <utility>
#include <vector>

#include "polo/boosting/none.hpp"
#include "polo/encoder/identity.hpp"
#include "polo/execution/serial.hpp"
#include "polo/prox/none.hpp"
#include "polo/smoothing/none.hpp"
#include "polo/step/constant.hpp"
#include "polo/terminator/iteration.hpp"
#include "polo/utility/null.hpp"
#include "polo/utility/sampler.hpp"

namespace polo {
namespace algorithm {
template <class value_t = double, class index_t = int,
          template <class, class> class boosting = boosting::none,
          template <class, class> class step = step::constant,
          template <class, class> class smoothing = smoothing::none,
          template <class, class> class prox = prox::none,
          template <class, class> class execution = execution::serial>
struct proxgradient : public boosting<value_t, index_t>,
                      public step<value_t, index_t>,
                      public smoothing<value_t, index_t>,
                      public prox<value_t, index_t>,
                      public execution<value_t, index_t> {
  proxgradient() = default;
  template <class ForwardIt> proxgradient(ForwardIt xbegin, ForwardIt xend) {
    initialize(xbegin, xend);
  }
  template <class T>
  proxgradient(const std::vector<T> &x)
      : proxgradient(std::begin(x), std::end(x)) {}

  template <class ForwardIt> void initialize(ForwardIt xbegin, ForwardIt xend) {
    std::vector<value_t> x =
        execution<value_t, index_t>::initialize(xbegin, xend);
    const value_t *xb = x.data();
    const value_t *xe = xb + x.size();
    boosting<value_t, index_t>::initialize(xb, xe);
    step<value_t, index_t>::initialize(xb, xe);
    smoothing<value_t, index_t>::initialize(xb, xe);
    prox<value_t, index_t>::initialize(xb, xe);
  }
  template <class T> void initialize(const std::vector<T> &x) {
    initialize(std::begin(x), std::end(x));
  }

  template <class... Ts> void boosting_parameters(Ts &&... params) {
    boosting<value_t, index_t>::parameters(std::forward<Ts>(params)...);
  }
  template <class... Ts> void step_parameters(Ts &&... params) {
    step<value_t, index_t>::parameters(std::forward<Ts>(params)...);
  }
  template <class... Ts> void smoothing_parameters(Ts &&... params) {
    smoothing<value_t, index_t>::parameters(std::forward<Ts>(params)...);
  }
  template <class... Ts> void prox_parameters(Ts &&... params) {
    prox<value_t, index_t>::parameters(std::forward<Ts>(params)...);
  }
  template <class... Ts> void execution_parameters(Ts &&... params) {
    execution<value_t, index_t>::parameters(std::forward<Ts>(params)...);
  }

  template <class Loss, class Logger, class Terminator, class Encoder>
  void solve(Loss &&loss, Logger &&logger, Terminator &&terminator,
             Encoder &&encoder) {
    execution<value_t, index_t>::solve(
        this, std::forward<Loss>(loss), std::forward<Logger>(logger),
        std::forward<Terminator>(terminator), std::forward<Encoder>(encoder));
  }
  template <class Loss, class Logger, class Terminator>
  void solve(Loss &&loss, Logger &&logger, Terminator &&terminator) {
    solve(std::forward<Loss>(loss), std::forward<Logger>(logger),
          std::forward<Terminator>(terminator),
          encoder::identity<value_t, index_t>{});
  }
  template <class Loss, class Logger> void solve(Loss &&loss, Logger &&logger) {
    solve(std::forward<Loss>(loss), std::forward<Logger>(logger),
          terminator::iteration<value_t, index_t>{100},
          encoder::identity<value_t, index_t>{});
  }
  template <class Loss> void solve(Loss &&loss) {
    solve(std::forward<Loss>(loss), utility::detail::null{},
          terminator::iteration<value_t, index_t>{100},
          encoder::identity<value_t, index_t>{});
  }

  template <class Loss, class Sampler, class Logger, class Terminator,
            class Encoder>
  void solve(Loss &&loss, utility::sampler::detail::component_sampler_t s,
             Sampler &&sampler, const index_t num_components, Logger &&logger,
             Terminator &&terminator, Encoder &&encoder) {
    execution<value_t, index_t>::solve(
        this, std::forward<Loss>(loss), s, std::forward<Sampler>(sampler),
        num_components, std::forward<Logger>(logger),
        std::forward<Terminator>(terminator), std::forward<Encoder>(encoder));
  }
  template <class Loss, class Sampler, class Logger, class Terminator>
  void solve(Loss &&loss, utility::sampler::detail::component_sampler_t s,
             Sampler &&sampler, const index_t num_components, Logger &&logger,
             Terminator &&terminator) {
    solve(std::forward<Loss>(loss), s, std::forward<Sampler>(sampler),
          num_components, std::forward<Logger>(logger),
          std::forward<Terminator>(terminator),
          encoder::identity<value_t, index_t>{});
  }
  template <class Loss, class Sampler, class Logger>
  void solve(Loss &&loss, utility::sampler::detail::component_sampler_t s,
             Sampler &&sampler, const index_t num_components, Logger &&logger) {
    solve(std::forward<Loss>(loss), s, std::forward<Sampler>(sampler),
          num_components, std::forward<Logger>(logger),
          terminator::iteration<value_t, index_t>{100},
          encoder::identity<value_t, index_t>{});
  }
  template <class Loss, class Sampler>
  void solve(Loss &&loss, utility::sampler::detail::component_sampler_t s,
             Sampler &&sampler, const index_t num_components) {
    solve(std::forward<Loss>(loss), s, std::forward<Sampler>(sampler),
          num_components, utility::detail::null{},
          terminator::iteration<value_t, index_t>{100},
          encoder::identity<value_t, index_t>{});
  }

  template <class Loss, class Sampler, class Logger, class Terminator,
            class Encoder>
  void solve(Loss &&loss, utility::sampler::detail::coordinate_sampler_t s,
             Sampler &&sampler, const index_t num_coordinates, Logger &&logger,
             Terminator &&terminator, Encoder &&encoder) {
    execution<value_t, index_t>::solve(
        this, std::forward<Loss>(loss), s, std::forward<Sampler>(sampler),
        num_coordinates, std::forward<Logger>(logger),
        std::forward<Terminator>(terminator), std::forward<Encoder>(encoder));
  }
  template <class Loss, class Sampler, class Logger, class Terminator>
  void solve(Loss &&loss, utility::sampler::detail::coordinate_sampler_t s,
             Sampler &&sampler, const index_t num_coordinates, Logger &&logger,
             Terminator &&terminator) {
    solve(std::forward<Loss>(loss), s, std::forward<Sampler>(sampler),
          num_coordinates, std::forward<Logger>(logger),
          std::forward<Terminator>(terminator),
          encoder::identity<value_t, index_t>{});
  }
  template <class Loss, class Sampler, class Logger>
  void solve(Loss &&loss, utility::sampler::detail::coordinate_sampler_t s,
             Sampler &&sampler, const index_t num_coordinates,
             Logger &&logger) {
    solve(std::forward<Loss>(loss), s, std::forward<Sampler>(sampler),
          num_coordinates, std::forward<Logger>(logger),
          terminator::iteration<value_t, index_t>{100},
          encoder::identity<value_t, index_t>{});
  }
  template <class Loss, class Sampler>
  void solve(Loss &&loss, utility::sampler::detail::coordinate_sampler_t s,
             Sampler &&sampler, const index_t num_coordinates) {
    solve(std::forward<Loss>(loss), s, std::forward<Sampler>(sampler),
          num_coordinates, utility::detail::null{},
          terminator::iteration<value_t, index_t>{100},
          encoder::identity<value_t, index_t>{});
  }

  template <class Loss, class Sampler1, class Sampler2, class Logger,
            class Terminator, class Encoder>
  void solve(Loss &&loss, utility::sampler::detail::component_sampler_t s1,
             Sampler1 &&sampler1, const index_t num_components,
             utility::sampler::detail::coordinate_sampler_t s2,
             Sampler2 &&sampler2, const index_t num_coordinates,
             Logger &&logger, Terminator &&terminator, Encoder &&encoder) {
    execution<value_t, index_t>::solve(
        this, std::forward<Loss>(loss), s1, std::forward<Sampler1>(sampler1),
        num_components, s2, std::forward<Sampler2>(sampler2), num_coordinates,
        std::forward<Logger>(logger), std::forward<Terminator>(terminator),
        std::forward<Encoder>(encoder));
  }
  template <class Loss, class Sampler1, class Sampler2, class Logger,
            class Terminator>
  void solve(Loss &&loss, utility::sampler::detail::component_sampler_t s1,
             Sampler1 &&sampler1, const index_t num_components,
             utility::sampler::detail::coordinate_sampler_t s2,
             Sampler2 &&sampler2, const index_t num_coordinates,
             Logger &&logger, Terminator &&terminator) {
    solve(std::forward<Loss>(loss), s1, std::forward<Sampler1>(sampler1),
          num_components, s2, std::forward<Sampler2>(sampler2), num_coordinates,
          std::forward<Logger>(logger), std::forward<Terminator>(terminator),
          encoder::identity<value_t, index_t>{});
  }
  template <class Loss, class Sampler1, class Sampler2, class Logger>
  void solve(Loss &&loss, utility::sampler::detail::component_sampler_t s1,
             Sampler1 &&sampler1, const index_t num_components,
             utility::sampler::detail::coordinate_sampler_t s2,
             Sampler2 &&sampler2, const index_t num_coordinates,
             Logger &&logger) {
    solve(std::forward<Loss>(loss), s1, std::forward<Sampler1>(sampler1),
          num_components, s2, std::forward<Sampler2>(sampler2), num_coordinates,
          std::forward<Logger>(logger),
          terminator::iteration<value_t, index_t>{100},
          encoder::identity<value_t, index_t>{});
  }
  template <class Loss, class Sampler1, class Sampler2>
  void solve(Loss &&loss, utility::sampler::detail::component_sampler_t s1,
             Sampler1 &&sampler1, const index_t num_components,
             utility::sampler::detail::coordinate_sampler_t s2,
             Sampler2 &&sampler2, const index_t num_coordinates) {
    solve(std::forward<Loss>(loss), s1, std::forward<Sampler1>(sampler1),
          num_components, s2, std::forward<Sampler2>(sampler2), num_coordinates,
          utility::detail::null{}, terminator::iteration<value_t, index_t>{100},
          encoder::identity<value_t, index_t>{});
  }

  value_t getf() const { return execution<value_t, index_t>::getf(); }
  std::vector<value_t> getx() const {
    return execution<value_t, index_t>::getx();
  }
};
} // namespace algorithm
} // namespace polo

#endif
