#include <algorithm>
#include <iterator>

#include "polo_api.h"
#include "polo_aux.hpp"

#include "polo/algorithm/proxgradient.hpp"
#include "polo/boosting/momentum.hpp"
#include "polo/smoothing/rmsprop.hpp"

extern "C" {
polo_error_t polo_adam_s(polo_value_t *fval, polo_value_t *xbegin,
                         polo_value_t *xend, polo_loss_t loss_f, void *loss_d,
                         polo_logger_t log_f, void *log_d,
                         polo_terminator_t term_f, void *term_d,
                         const polo_value_t gamma, const polo_value_t beta1,
                         const polo_value_t beta2, const polo_value_t epsilon) {
  using namespace polo;
  polo_error_t err;
  err.id = 0;
  try {
    algorithm::proxgradient<polo_value_t, polo_index_t, boosting::momentum,
                            step::constant, smoothing::rmsprop, prox::none,
                            execution::serial>
        alg;
    alg.boosting_parameters(beta1, 1 - beta1);
    alg.step_parameters(gamma);
    alg.smoothing_parameters(beta2, epsilon);
    alg.initialize(xbegin, xend);
    alg.solve([=](const polo_value_t *x,
                  polo_value_t *g) { return loss_f(x, g, loss_d); },
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
    const auto xopt = alg.getx();
    const auto fopt = alg.getf();
    std::copy(std::begin(xopt), std::end(xopt), xbegin);
    *fval = fopt;
  } catch (const std::exception &ex) {
    parse_ex(ex, err);
  }
  return err;
}
}
