#include "polo_api.h"
#include "polo_aux.hpp"

#include "polo/algorithm/proxgradient.hpp"
#include "polo/execution/paramserver.hpp"
#include "polo/smoothing/adagrad.hpp"

extern "C" {
polo_error_t polo_adagrad_psw(const polo_value_t *xbegin,
                              const polo_value_t *xend, polo_loss_t loss_f,
                              void *loss_d, polo_logger_t log_f, void *log_d,
                              const polo_ps_options_t opts) {
  using namespace polo;
  polo_error_t err;
  err.id = 0;
  try {
    algorithm::proxgradient<polo_value_t, polo_index_t, boosting::none,
                            step::constant, smoothing::adagrad, prox::none,
                            execution::paramserver::worker>
        alg;
    alg.execution_parameters(parse_ps_opts(opts));
    alg.initialize(xbegin, xend);
    alg.solve(
        [=](const polo_value_t *xbegin, polo_value_t *gbegin) {
          return loss_f(xbegin, gbegin, loss_d);
        },
        [=](const polo_index_t k, const polo_value_t fval,
            const polo_value_t *xbegin, const polo_value_t *xend,
            const polo_value_t *gbegin) {
          log_f(k, fval, xbegin, xend, gbegin, log_d);
        },
        nullptr);
  } catch (const std::exception &ex) {
    parse_ex(ex, err);
  }
  return err;
}
}
