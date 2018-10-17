#include "polo/polo.hpp"
#include "ps_api.h"

using namespace polo;

err_t paramserver_m(master_log_t logger, const ps_opt_t options, init_t init,
                    boost_t boost_compute, void *boost_data,
                    step_t step_compute, void *step_data,
                    smoothing_t smoothing_compute, void *smoothing_data,
                    prox_t prox_compute, void *prox_data) {
  err_t err;
  err.id = 0;

  try {
    execution::paramserver::options opts;
    opts.linger(options.linger);
    opts.master(options.maddress, options.mworker);
    opts.master_timeout(options.mtimeout);
    opts.scheduler(options.saddress, options.sports[0], options.sports[1],
                   options.sports[2]);

    algorithm::proxgradient<value_t, index_t, boosting::custom, step::custom,
                            smoothing::custom, prox::custom,
                            execution::paramserver::master>
        alg;

    alg.boosting_parameters(init, boost_compute, boost_data);
    alg.step_parameters(init, step_compute, step_data);
    alg.smoothing_parameters(init, smoothing_compute, smoothing_data);
    alg.prox_parameters(init, prox_compute, prox_data);
    alg.execution_parameters(opts);

    alg.initialize(nullptr, nullptr);

    alg.solve(nullptr, nullptr,
              [=](const index_t k, const value_t fval, const value_t *xbegin,
                  const value_t *xend, const value_t *gbegin) {
                logger(k, fval, xbegin, xend, gbegin);
              });
  } catch (const std::exception &ex) {
    err.id = -1;
    const char *c = ex.what();
    int idx = 0;
    while (*c != '\0' && idx < 256) {
      err.what[idx++] = *c++;
    }
  }

  return err;
}
