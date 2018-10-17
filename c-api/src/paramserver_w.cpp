#include "polo/polo.hpp"
#include "ps_api.h"

using namespace polo;

err_t paramserver_w(const value_t *xbegin, const value_t *xend, loss_t loss,
                    void *loss_data, const ps_opt_t options) {
  err_t err;
  err.id = 0;

  try {
    execution::paramserver::options opts;
    opts.linger(options.linger);
    opts.scheduler(options.saddress, options.sports[0], options.sports[1],
                   options.sports[2]);
    opts.worker_timeout(options.wtimeout);

    algorithm::proxgradient<value_t, index_t, boosting::none, step::constant,
                            smoothing::none, prox::none,
                            execution::paramserver::worker>
        alg;

    alg.execution_parameters(opts);
    alg.initialize(xbegin, xend);
    alg.solve(
        [=](const value_t *xb, value_t *gb) { return loss(xb, gb, loss_data); },
        nullptr, nullptr);
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
