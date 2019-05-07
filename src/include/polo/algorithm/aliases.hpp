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
          template <class, class> class prox = prox::none,
          template <class, class> class execution = execution::serial>
using gd = proxgradient<value_t, index_t, boosting::none, step::constant,
                        smoothing::none, prox, execution>;

template <class value_t, class index_t,
          template <class, class> class prox = prox::none,
          template <class, class> class execution = execution::serial>
using momentum = proxgradient<value_t, index_t, boosting::momentum,
                              step::constant, smoothing::none, prox, execution>;

template <class value_t, class index_t,
          template <class, class> class prox = prox::none,
          template <class, class> class execution = execution::serial>
using nesterov = proxgradient<value_t, index_t, boosting::nesterov,
                              step::constant, smoothing::none, prox, execution>;

template <class value_t, class index_t,
          template <class, class> class prox = prox::none,
          template <class, class> class execution = execution::serial>
using adagrad = proxgradient<value_t, index_t, boosting::none, step::constant,
                             smoothing::adagrad, prox, execution>;

template <class value_t, class index_t,
          template <class, class> class prox = prox::none,
          template <class, class> class execution = execution::serial>
using adadelta = proxgradient<value_t, index_t, boosting::none, step::constant,
                              smoothing::adadelta, prox, execution>;

template <class value_t, class index_t,
          template <class, class> class prox = prox::none,
          template <class, class> class execution = execution::serial>
using adam = proxgradient<value_t, index_t, boosting::momentum, step::constant,
                          smoothing::rmsprop, prox, execution>;

template <class value_t, class index_t,
          template <class, class> class prox = prox::none,
          template <class, class> class execution = execution::serial>
using nadam = proxgradient<value_t, index_t, boosting::nesterov, step::constant,
                           smoothing::rmsprop, prox, execution>;

template <class value_t, class index_t,
          template <class, class> class prox = prox::none,
          template <class, class> class execution = execution::serial>
using adamax =
    proxgradient<value_t, index_t, boosting::momentum, step::constant,
                 smoothing::amsgrad, prox, execution>;

template <class value_t, class index_t,
          template <class, class> class prox = prox::none,
          template <class, class> class execution = execution::serial>
using iag = proxgradient<value_t, index_t, boosting::aggregated, step::constant,
                         smoothing::none, prox, execution>;
} // namespace algorithm
} // namespace polo

#endif
