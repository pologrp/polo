#include "polo_api.h"
#include "polo_aux.hpp"

#include "polo/algorithm/proxgradient.hpp"
#include "polo/boosting/custom.hpp"
#include "polo/encoder/identity.hpp"
#include "polo/prox/custom.hpp"
#include "polo/smoothing/custom.hpp"
#include "polo/step/custom.hpp"

extern "C" {
polo_error_t polo_proxgrad_s(const polo_value_t *xbegin,
                             const polo_value_t *xend, polo_loss_t loss_f,
                             void *loss_d, polo_terminator_t term_f,
                             void *term_d, polo_logger_t log_f, void *log_d,
                             polo_init_t init_f, polo_boost_t boost_f,
                             void *boost_d, polo_step_t step_f, void *step_d,
                             polo_smooth_t smooth_f, void *smooth_d,
                             polo_prox_t prox_f, void *prox_d) {
  using namespace polo;
  polo_error_t err;
  err.id = 0;
  try {
    algorithm::proxgradient<polo_value_t, polo_index_t, boosting::custom,
                            step::custom, smoothing::custom, prox::custom,
                            execution::serial>
        alg;
    alg.boosting_parameters(init_f, boost_f, boost_d);
    alg.step_parameters(init_f, step_f, step_d);
    alg.smoothing_parameters(init_f, smooth_f, smooth_d);
    alg.prox_parameters(init_f, prox_f, prox_d);
    alg.initialize(xbegin, xend);
    alg.solve([=](const polo_value_t *x,
                  polo_value_t *g) { return loss_f(x, g, loss_d); },
              encoder::identity<polo_value_t, polo_index_t>{},
              [=](const polo_index_t k, const polo_value_t fval,
                  const polo_value_t *xbegin, const polo_value_t *xend,
                  const polo_value_t *gbegin) {
                return bool(term_f(k, fval, xbegin, xend, gbegin, term_d));
              },
              [=](const polo_index_t k, const polo_value_t fval,
                  const polo_value_t *xbegin, const polo_value_t *xend,
                  const polo_value_t *gbegin) {
                log_f(k, fval, xbegin, xend, gbegin, log_d);
              });
  } catch (const std::exception &ex) {
    parse_ex(ex, err);
  }
  return err;
}
}
