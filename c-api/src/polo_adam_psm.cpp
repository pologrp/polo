#include "polo_api.h"
#include "polo_aux.hpp"

#include "polo/algorithm/proxgradient.hpp"
#include "polo/boosting/momentum.hpp"
#include "polo/execution/paramserver.hpp"
#include "polo/smoothing/rmsprop.hpp"

extern "C" {
polo_error_t polo_adam_psm(polo_logger_t log_f, void *log_d,
                           polo_terminator_t term_f, void *term_d,
                           const polo_value_t gamma, const polo_value_t beta1,
                           const polo_value_t beta2, const polo_value_t epsilon,
                           const polo_ps_options_t opts) {
  using namespace polo;
  polo_error_t err;
  err.id = 0;
  try {
    algorithm::proxgradient<polo_value_t, polo_index_t, boosting::momentum,
                            step::constant, smoothing::rmsprop, prox::none,
                            execution::paramserver::master>
        alg;
    alg.boosting_parameters(beta1, 1 - beta1);
    alg.step_parameters(gamma);
    alg.smoothing_parameters(beta2, epsilon);
    alg.execution_parameters(parse_ps_opts(opts));
    alg.initialize(nullptr, nullptr);
    alg.solve(nullptr,
              [=](const polo_index_t k, const polo_value_t fval,
                  const polo_value_t *xbegin, const polo_value_t *xend,
                  const polo_value_t *gbegin) {
                log_f(k, fval, xbegin, xend, gbegin, log_d);
              },
              [=](const polo_index_t k, const polo_value_t fval,
                  const polo_value_t *xbegin, const polo_value_t *xend,
                  const polo_value_t *gbegin) {
                return bool(term_f(k, fval, xbegin, xend, gbegin, term_d));
              });
  } catch (const std::exception &ex) {
    parse_ex(ex, err);
  }
  return err;
}
}
