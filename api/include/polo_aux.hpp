#ifndef POLO_AUX_HPP_
#define POLO_AUX_HPP_

#include <stdexcept>

#include "polo_api.h"

#include "polo/execution/paramserver.hpp"

inline void parse_ex(const std::exception &ex, polo_error_t &err) {
  err.id = -1;
  const char *c = ex.what();
  int idx = 0;
  while (*c != '\0' && idx < 256) {
    err.what[idx++] = *c++;
  }
}

inline polo::execution::paramserver::options
parse_ps_opts(const polo_ps_options_t &opts) {
  polo::execution::paramserver::options options;
  options.linger(opts.linger);
  options.master(opts.maddress, opts.mworker);
  options.master_timeout(opts.mtimeout);
  options.num_masters(opts.num_masters);
  options.scheduler(opts.saddress, opts.sports[0], opts.sports[1],
                    opts.sports[2]);
  options.scheduler_timeout(opts.stimeout);
  options.worker_timeout(opts.wtimeout);
  return options;
}

#endif
