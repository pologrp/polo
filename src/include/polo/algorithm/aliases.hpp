#ifndef POLO_ALGORITHM_ALIASES_HPP_
#define POLO_ALGORITHM_ALIASES_HPP_

#include "polo/algorithm/proxgradient.hpp"
#include "polo/boosting.hpp"
#include "polo/execution.hpp"
#include "polo/prox.hpp"
#include "polo/smoothing.hpp"
#include "polo/step.hpp"

namespace polo {
namespace algorithm {
template <class value_t, class index_t,
          template <class, class> class execution = execution::serial>
using gd = proxgradient<value_t, index_t, boosting::none, step::constant,
                        smoothing::none, prox::none, execution>;
}
} // namespace polo

#endif
