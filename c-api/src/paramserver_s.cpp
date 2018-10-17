#include "polo/polo.hpp"
#include "ps_api.h"

using namespace polo;

err_t paramserver_s(const value_t *xbegin, const value_t *xend, const index_t K,
                    const ps_opt_t options) {
  err_t err;
  err.id = 0;

  try {
    execution::paramserver::options opts;
    opts.linger(options.linger);
    opts.num_masters(options.num_masters);
    opts.scheduler(options.saddress, options.sports[0], options.sports[1],
                   options.sports[2]);
    opts.scheduler_timeout(options.stimeout);

    algorithm::proxgradient<value_t, index_t, boosting::none, step::constant,
                            smoothing::none, prox::none,
                            execution::paramserver::scheduler>
        alg;

    alg.execution_parameters(opts);

    alg.initialize(xbegin, xend);

    alg.solve(nullptr,
              [=](const index_t k, const value_t, const value_t *,
                  const value_t *, const value_t *) { return k >= K; },
              nullptr);
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
