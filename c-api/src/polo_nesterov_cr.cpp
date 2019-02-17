#include <algorithm>
#include <iterator>

#include "polo_api.h"
#include "polo_aux.hpp"

#include "polo/algorithm/proxgradient.hpp"
#include "polo/boosting/momentum.hpp"
#include "polo/encoder/identity.hpp"
#include "polo/execution/multithread.hpp"

extern "C" {
polo_error_t polo_nesterov_cr(polo_value_t *fval, polo_value_t *xbegin,
                              polo_value_t *xend, polo_loss_t loss_f,
                              void *loss_d, polo_terminator_t term_f,
                              void *term_d, polo_logger_t log_f, void *log_d,
                              const polo_value_t mu, const polo_value_t epsilon,
                              const unsigned int nthreads) {
  using namespace polo;
  polo_error_t err;
  err.id = 0;
  try {
    algorithm::proxgradient<polo_value_t, polo_index_t, boosting::nesterov,
                            step::constant, smoothing::none, prox::none,
                            execution::consistent>
        alg;
    alg.boosting_parameters(mu, epsilon);
    alg.execution_parameters(nthreads);
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
