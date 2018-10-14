#ifndef POLO_API_COMMON_HPP_
#define POLO_API_COMMON_HPP_

#include "polo/polo.hpp"
#include <stdint.h>

template <class value_t>
using loss_t = value_t (*)(const value_t *, value_t *, void *);
template <class value_t>
using init_t = void (*)(const value_t *, const value_t *, void *);

using value_t = double;
using index_t = int;

using termination_t =
    typename polo::utility::terminator::custom<value_t, index_t>::func_t;
using termination =
    typename polo::utility::terminator::custom<value_t, index_t>;
using log_t = typename polo::utility::logger::custom<value_t, index_t>::func_t;
using logger = typename polo::utility::logger::custom<value_t, index_t>;
using option_t = polo::execution::paramserver::options;

#endif
