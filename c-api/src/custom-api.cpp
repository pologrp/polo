#include "common.hpp"

extern "C" {

using namespace polo;

/* Custom API */

using boost_t = typename boosting::custom<value_t, index_t>::func_t;
using step_t = typename step::custom<value_t, index_t>::func_t;
using smooth_t = typename smoothing::custom<value_t, index_t>::func_t;
using prox_t = typename prox::custom<value_t, index_t>::func_t;

/* Serial algorithms  */
using serial_t =
    algorithm::proxgradient<value_t, index_t, boosting::custom, step::custom,
                            smoothing::custom, prox::custom, execution::serial>;

void *proxgradient_s(init_t<value_t> init_fcn, boost_t boost_fcn,
                     void *boost_data, step_t step_fcn, void *step_data,
                     smooth_t smooth_fcn, void *smooth_data, prox_t prox_fcn,
                     void *prox_data) {
  serial_t *alg = new serial_t();
  alg->boosting_parameters(init_fcn, boost_fcn, boost_data);
  alg->smoothing_parameters(init_fcn, smooth_fcn, smooth_data);
  alg->step_parameters(init_fcn, step_fcn, step_data);
  alg->prox_parameters(init_fcn, prox_fcn, prox_data);
  return static_cast<void *>(alg);
  }

  void delete_proxgradient_s(void *handle) {
    delete static_cast<serial_t *>(handle);
  }

  void run_serial(void *handle, const value_t *xbegin, const value_t *xend,
                  loss_t<value_t> loss_fcn, void *loss_data,
                  termination_t termination_fcn, void *termination_data,
                  log_t logger_fcn, void *logger_data) {
    serial_t *alg = static_cast<serial_t *>(handle);
    alg->initialize(xbegin, xend);
    alg->solve(
        [=](const value_t *xbegin, value_t *gbegin) -> value_t {
          return loss_fcn(xbegin, gbegin, loss_data);
        },
        termination(termination_fcn, termination_data),
        logger(logger_fcn, logger_data));
  }

  value_t getf_s(void *handle) {
    return static_cast<const serial_t *>(handle)->getf();
  }

  void getx_s(void *handle, value_t *x) {
    auto x_val = static_cast<const serial_t *>(handle)->getx();
    std::copy(x_val.cbegin(), x_val.cbegin()+x_val.size(), x);
  }

  /* Multithreaded algorithms  */
  using multithread_t =
      algorithm::proxgradient<value_t, index_t, boosting::custom,
                              step::custom, smoothing::custom, prox::custom,
                              execution::consistent>;

  void *proxgradient_mt(init_t<value_t> init_fcn, boost_t boost_fcn,
                        void *boost_data, step_t step_fcn, void *step_data,
                        smooth_t smooth_fcn, void *smooth_data, prox_t prox_fcn,
                        void *prox_data) {
    multithread_t *alg = new multithread_t();
    alg->boosting_parameters(init_fcn, boost_fcn, boost_data);
    alg->smoothing_parameters(init_fcn, smooth_fcn, smooth_data);
    alg->step_parameters(init_fcn, step_fcn, step_data);
    alg->prox_parameters(init_fcn, prox_fcn, prox_data);
    return static_cast<void *>(alg);
  }

  void delete_proxgradient_mt(void *handle) {
    delete static_cast<multithread_t *>(handle);
  }

  void run_multithread(void *handle, const value_t *xbegin,
                       const value_t *xend, loss_t<value_t> loss_fcn,
                       void *loss_data, termination_t termination_fcn,
                       void *termination_data, log_t logger_fcn,
                       void *logger_data) {
    multithread_t *alg = static_cast<multithread_t *>(handle);
    alg->initialize(xbegin, xend);
    alg->solve(
        [=](const value_t *xbegin, value_t *gbegin) -> value_t {
          return loss_fcn(xbegin, gbegin, loss_data);
        },
        termination(termination_fcn, termination_data),
        logger(logger_fcn, logger_data));
  }

  value_t getf_mt(void *handle) {
    return static_cast<const multithread_t *>(handle)->getf();
  }

  void getx_mt(void *handle, value_t *x) {
    auto x_val = static_cast<const multithread_t *>(handle)->getx();
    std::copy(x_val.cbegin(), x_val.cbegin() + x_val.size(), x);
  }

  /* Parameter server */

  /* Parameter server master */
  using master_t =
      algorithm::proxgradient<value_t, index_t, boosting::custom,
                              step::custom, smoothing::custom, prox::custom,
                              execution::paramserver::master>;

  void* proxgradient_master(init_t<value_t> init_fcn, boost_t boost_fcn,
			    void *boost_data, step_t step_fcn, void *step_data,
			    smooth_t smooth_fcn, void *smooth_data, prox_t prox_fcn,
			    void *prox_data, void *option_data) {
    option_t* psopts = static_cast<option_t *>(option_data);
    master_t* alg = new master_t();
    alg->boosting_parameters(init_fcn, boost_fcn, boost_data);
    alg->smoothing_parameters(init_fcn, smooth_fcn, smooth_data);
    alg->step_parameters(init_fcn, step_fcn, step_data);
    alg->prox_parameters(init_fcn, prox_fcn, prox_data);
    alg->execution_parameters(*psopts);
    return static_cast<void *>(alg);
  }

  void delete_proxgradient_master(void *handle) {
    delete static_cast<master_t *>(handle);
  }

  void run_master(void *handle, const value_t *xbegin, const value_t *xend,
                  termination_t termination_fcn, void *termination_data,
                  log_t logger_fcn, void *logger_data) {
    master_t *alg = static_cast<master_t *>(handle);
    alg->initialize(xbegin, xend);
    alg->solve(nullptr, termination(termination_fcn, termination_data),
               logger(logger_fcn, logger_data));
  }

  // value_t getf_master(void *handle) {
  //   return static_cast<const master_t *>(handle)->getf();
  // }

  // void getx_master(void *handle, value_t *x) {
  //   auto x_val = static_cast<const master_t *>(handle)->getx();
  //   std::copy(x_val.cbegin(), x_val.cbegin()+x_val.size(), x);
  // }

  /* Parameter server worker */
  using worker_t =
      algorithm::proxgradient<value_t, index_t, boosting::custom,
                              step::custom, smoothing::custom, prox::custom,
                              execution::paramserver::worker>;

  void *proxgradient_worker(init_t<value_t> init_fcn, boost_t boost_fcn,
                            void *boost_data, step_t step_fcn, void *step_data,
                            smooth_t smooth_fcn, void *smooth_data,
                            prox_t prox_fcn, void *prox_data,
                            void *option_data) {
    option_t *psopts = static_cast<option_t *>(option_data);
    worker_t *alg = new worker_t();
    alg->boosting_parameters(init_fcn, boost_fcn, boost_data);
    alg->smoothing_parameters(init_fcn, smooth_fcn, smooth_data);
    alg->step_parameters(init_fcn, step_fcn, step_data);
    alg->prox_parameters(init_fcn, prox_fcn, prox_data);
    alg->execution_parameters(*psopts);
    return static_cast<void *>(alg);
  }

  void delete_proxgradient_worker(void *handle) {
    delete static_cast<worker_t *>(handle);
  }

  void run_worker(void *handle, const value_t *xbegin, const value_t *xend,
                  loss_t<value_t> loss_fcn, void *loss_data,
                  termination_t termination_fcn, void *termination_data,
                  log_t logger_fcn, void *logger_data) {
    worker_t *alg = static_cast<worker_t *>(handle);
    alg->initialize(xbegin, xend);
    alg->solve(
        [=](const value_t *xbegin, value_t *gbegin) -> value_t {
          return loss_fcn(xbegin, gbegin, loss_data);
        },
        termination(termination_fcn, termination_data),
        logger(logger_fcn, logger_data));
  }

  // value_t getf_worker(void *handle) {
  //   return static_cast<const worker_t *>(handle)->getf();
  // }

  // void getx_worker(void *handle, value_t *x) {
  //   auto x_val = static_cast<const worker_t *>(handle)->getx();
  //   std::copy(x_val.cbegin(), x_val.cbegin()+x_val.size(), x);
  // }

  /* Parameter server master */
  using scheduler_t =
      algorithm::proxgradient<value_t, index_t, boosting::custom,
                              step::custom, smoothing::custom, prox::custom,
                              execution::paramserver::scheduler>;

  void *proxgradient_scheduler(init_t<value_t> init_fcn, boost_t boost_fcn,
                               void *boost_data, step_t step_fcn,
                               void *step_data, smooth_t smooth_fcn,
                               void *smooth_data, prox_t prox_fcn,
                               void *prox_data, void *option_data) {
    option_t *psopts = static_cast<option_t *>(option_data);
    scheduler_t *alg = new scheduler_t();
    alg->boosting_parameters(init_fcn, boost_fcn, boost_data);
    alg->smoothing_parameters(init_fcn, smooth_fcn, smooth_data);
    alg->step_parameters(init_fcn, step_fcn, step_data);
    alg->prox_parameters(init_fcn, prox_fcn, prox_data);
    alg->execution_parameters(*psopts);
    return static_cast<void *>(alg);
  }

  void delete_proxgradient_scheduler(void *handle) {
    delete static_cast<scheduler_t *>(handle);
  }

  void run_scheduler(void *handle, const value_t *xbegin,
                     const value_t *xend, termination_t termination_fcn,
                     void *termination_data, log_t logger_fcn,
                     void *logger_data) {
    scheduler_t *alg = static_cast<scheduler_t *>(handle);
    alg->initialize(xbegin, xend);
    alg->solve(nullptr, termination(termination_fcn, termination_data),
               logger(logger_fcn, logger_data));
  }

  // value_t getf_scheduler(void *handle) {
  //   return static_cast<const scheduler_t *>(handle)->getf();
  // }

  // void getx_scheduler(void *handle, value_t *x) {
  //   auto x_val = static_cast<const scheduler_t *>(handle)->getx();
  //   std::copy(x_val.cbegin(), x_val.cbegin() + x_val.size(), x);
  // }
}
