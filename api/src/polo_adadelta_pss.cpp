#include "polo_api.h"
#include "polo_aux.hpp"

#include "polo/algorithm/proxgradient.hpp"
#include "polo/execution/paramserver.hpp"
#include "polo/smoothing/adadelta.hpp"

extern "C" {
polo_error_t polo_adadelta_pss(const polo_value_t *xbegin,
                               const polo_value_t *xend, polo_logger_t log_f,
                               void *log_d, polo_terminator_t term_f,
                               void *term_d, const polo_ps_options_t opts) {
  using namespace polo;
  polo_error_t err;
  err.id = 0;
  try {
    algorithm::proxgradient<polo_value_t, polo_index_t, boosting::none,
                            step::constant, smoothing::adadelta, prox::none,
                            execution::paramserver::scheduler>
        alg;
    alg.execution_parameters(parse_ps_opts(opts));
    alg.initialize(xbegin, xend);
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
