#include "common.hpp"

#define CAT(a, b) a##b
#define CAT2(a, b) CAT(a, b)
#define CAT3(a, b, c) CAT2(CAT2(a, b), c)
#define EXECUTOR(algname, executor, abbrv, boost, steplength, smooth, proxim)  \
  value_t CAT3(default, _, CAT3(algname, _, abbrv))(                           \
      value_t * xbegin, value_t * xend, loss_t<value_t> loss_fcn,              \
      void *loss_data, termination_t termination_fcn, void *termination_data,  \
      log_t logger_fcn, void *logger_data) {                                   \
    algorithm::proxgradient<value_t, index_t, boosting::boost,                 \
                            step::steplength, smoothing::smooth, prox::proxim, \
                            execution::executor>                               \
        alg;                                                                   \
    alg.initialize(xbegin, xend);                                              \
    alg.solve(                                                                 \
        [=](const value_t *xbegin, value_t *gbegin) -> value_t {               \
          return loss_fcn(xbegin, gbegin, loss_data);                          \
        },                                                                     \
        termination(termination_fcn, termination_data),                        \
        logger(logger_fcn, logger_data));                                      \
    const auto x = alg.getx();                                                 \
    std::copy(x.begin(), x.end(), xbegin);                                     \
    return alg.getf();                                                         \
  }

#define PARAMSERVER_EXECUTOR(algname, executor, abbrv, boost, steplength,      \
                             smooth, proxim)                                   \
  value_t CAT3(default, _, CAT3(algname, _, abbrv))(                           \
      value_t * xbegin, value_t * xend, loss_t<value_t> loss_fcn,              \
      void *loss_data, option_t option_data) {                                 \
    auto psopts = get_psopts(option_data);                                     \
    algorithm::proxgradient<value_t, index_t, boosting::boost,                 \
                            step::steplength, smoothing::smooth, prox::proxim, \
                            execution::executor>                               \
        alg;                                                                   \
    alg.execution_parameters(psopts);                                          \
    alg.initialize(xbegin, xend);                                              \
    alg.solve([=](const value_t *xbegin, value_t *gbegin) -> value_t {         \
      return loss_fcn(xbegin, gbegin, loss_data);                              \
    });                                                                        \
    const auto x = alg.getx();                                                 \
    std::copy(x.begin(), x.end(), xbegin);                                     \
    return alg.getf();                                                         \
  }

#define DEFINE_ALG(algname, boost, steplength, smooth, proxim)                 \
  EXECUTOR(algname, serial, s, boost, steplength, smooth, proxim)              \
  EXECUTOR(algname, consistent, mtc, boost, steplength, smooth, proxim)        \
  value_t CAT3(default, _, CAT3(algname, _, mti))(                             \
      value_t * xbegin, value_t * xend, loss_t<value_t> loss_fcn,              \
      void *loss_data) {                                                       \
    algorithm::proxgradient<value_t, index_t, boosting::boost,                 \
                            step::steplength, smoothing::smooth, prox::proxim, \
                            execution::inconsistent>                           \
        alg;                                                                   \
    alg.initialize(xbegin, xend);                                              \
    alg.solve([=](const value_t *xbegin, value_t *gbegin) -> value_t {         \
      return loss_fcn(xbegin, gbegin, loss_data);                              \
    });                                                                        \
    const auto x = alg.getx();                                                 \
    std::copy(x.begin(), x.end(), xbegin);                                     \
    return alg.getf();                                                         \
  }                                                                            \
  PARAMSERVER_EXECUTOR(algname, paramserver::master, psm, boost, steplength,   \
                       smooth, proxim)                                         \
  PARAMSERVER_EXECUTOR(algname, paramserver::worker, psw, boost, steplength,   \
                       smooth, proxim)                                         \
  PARAMSERVER_EXECUTOR(algname, paramserver::scheduler, pss, boost,            \
                       steplength, smooth, proxim)

extern "C" {

using namespace polo;

/* Algorithms with default parameters */
/* Gradient descent */
DEFINE_ALG(gradient, none, constant, none, none);
/* Gradient descent with momentum */
DEFINE_ALG(momentum, momentum, constant, none, none);
/* Nesterov */
DEFINE_ALG(nesterov, nesterov, constant, none, none);
/* Adagrad */
DEFINE_ALG(adagrad, none, constant, adagrad, none);
/* Adadelta */
DEFINE_ALG(adadelta, none, constant, adadelta, none);
/* Adam */
DEFINE_ALG(adam, momentum, constant, rmsprop, none);
/* Nadam */
DEFINE_ALG(nadam, nesterov, constant, rmsprop, none);

/* Gradient descent */
#define GRADIENT(executor, abbrv)                                              \
  value_t CAT3(gradient, _, abbrv)(                                            \
      value_t * xbegin, value_t * xend, loss_t<value_t> loss_fcn,              \
      void *loss_data, termination_t termination_fcn, void *termination_data,  \
      log_t logger_fcn, void *logger_data, const value_t gamma) {              \
    algorithm::proxgradient<value_t, index_t, boosting::none, step::constant,  \
                            smoothing::none, prox::none, execution::executor>  \
        alg;                                                                   \
    alg.step_parameters(gamma);                                                \
    alg.initialize(xbegin, xend);                                              \
    alg.solve(                                                                 \
        [=](const value_t *xbegin, value_t *gbegin) -> value_t {               \
          return loss_fcn(xbegin, gbegin, loss_data);                          \
        },                                                                     \
        termination(termination_fcn, termination_data),                        \
        logger(logger_fcn, logger_data));                                      \
    const auto x = alg.getx();                                                 \
    std::copy(x.begin(), x.end(), xbegin);                                     \
    return alg.getf();                                                         \
  }

GRADIENT(serial, s)
GRADIENT(consistent, mtc)
value_t gradient_mti(value_t *xbegin, value_t *xend, loss_t<value_t> loss_fcn,
                     void *loss_data, const value_t gamma) {
  algorithm::proxgradient<value_t, index_t, boosting::none, step::constant,
                          smoothing::none, prox::none, execution::inconsistent>
      alg;
  alg.step_parameters(gamma);
  alg.initialize(xbegin, xend);
  alg.solve([=](const value_t *xbegin, value_t *gbegin) -> value_t {
    return loss_fcn(xbegin, gbegin, loss_data);
  });
  const auto x = alg.getx();
  std::copy(x.begin(), x.end(), xbegin);
  return alg.getf();
}
#define PARAMSERVER_GRADIENT(executor, abbrv)                                  \
  value_t CAT3(gradient, _, abbrv)(                                            \
      value_t * xbegin, value_t * xend, loss_t<value_t> loss_fcn,              \
      void *loss_data, option_t option_data, const value_t gamma) {            \
    auto psopts = get_psopts(option_data);                                     \
    algorithm::proxgradient<value_t, index_t, boosting::none, step::constant,  \
                            smoothing::none, prox::none, execution::executor>  \
        alg;                                                                   \
    alg.step_parameters(gamma);                                                \
    alg.execution_parameters(psopts);                                          \
    alg.initialize(xbegin, xend);                                              \
    alg.solve([=](const value_t *xbegin, value_t *gbegin) -> value_t {         \
      return loss_fcn(xbegin, gbegin, loss_data);                              \
    });                                                                        \
    const auto x = alg.getx();                                                 \
    std::copy(x.begin(), x.end(), xbegin);                                     \
    return alg.getf();                                                         \
  }
PARAMSERVER_GRADIENT(paramserver::master, psm)
PARAMSERVER_GRADIENT(paramserver::worker, psw)
PARAMSERVER_GRADIENT(paramserver::scheduler, pss)

/* Gradient descent with momentum */
#define MOMENTUM(momentum, executor, abbrv)                                    \
  value_t CAT3(momentum, _, abbrv)(                                            \
      value_t * xbegin, value_t * xend, loss_t<value_t> loss_fcn,              \
      void *loss_data, termination_t termination_fcn, void *termination_data,  \
      log_t logger_fcn, void *logger_data, const value_t mu,                   \
      const value_t epsilon, const value_t gamma) {                            \
    algorithm::proxgradient<value_t, index_t, boosting::momentum,              \
                            step::constant, smoothing::none, prox::none,       \
                            execution::executor>                               \
        alg;                                                                   \
    alg.boosting_parameters(mu, epsilon);                                      \
    alg.step_parameters(gamma);                                                \
    alg.initialize(xbegin, xend);                                              \
    alg.solve(                                                                 \
        [=](const value_t *xbegin, value_t *gbegin) -> value_t {               \
          return loss_fcn(xbegin, gbegin, loss_data);                          \
        },                                                                     \
        termination(termination_fcn, termination_data),                        \
        logger(logger_fcn, logger_data));                                      \
    const auto x = alg.getx();                                                 \
    std::copy(x.begin(), x.end(), xbegin);                                     \
    return alg.getf();                                                         \
  }
MOMENTUM(momentum, serial, s)
MOMENTUM(momentum, consistent, mtc)
#define INCONSISTENT_MOMENTUM(momentum)                                        \
  value_t CAT3(momentum, _, mti)(value_t * xbegin, value_t * xend,             \
                                 loss_t<value_t> loss_fcn, void *loss_data,    \
                                 const value_t mu, const value_t epsilon,      \
                                 const value_t gamma) {                        \
    algorithm::proxgradient<value_t, index_t, boosting::momentum,              \
                            step::constant, smoothing::none, prox::none,       \
                            execution::inconsistent>                           \
        alg;                                                                   \
    alg.boosting_parameters(mu, epsilon);                                      \
    alg.step_parameters(gamma);                                                \
    alg.initialize(xbegin, xend);                                              \
    alg.solve([=](const value_t *xbegin, value_t *gbegin) -> value_t {         \
      return loss_fcn(xbegin, gbegin, loss_data);                              \
    });                                                                        \
    const auto x = alg.getx();                                                 \
    std::copy(x.begin(), x.end(), xbegin);                                     \
    return alg.getf();                                                         \
  }
INCONSISTENT_MOMENTUM(momentum)
#define PARAMSERVER_MOMENTUM(momentum, executor, abbrv)                        \
  value_t CAT3(momentum, _, abbrv)(                                            \
      value_t * xbegin, value_t * xend, loss_t<value_t> loss_fcn,              \
      void *loss_data, option_t option_data, const value_t mu,                 \
      const value_t epsilon, const value_t gamma) {                            \
    auto psopts = get_psopts(option_data);                                     \
    algorithm::proxgradient<value_t, index_t, boosting::momentum,              \
                            step::constant, smoothing::none, prox::none,       \
                            execution::executor>                               \
        alg;                                                                   \
    alg.boosting_parameters(mu, epsilon);                                      \
    alg.step_parameters(gamma);                                                \
    alg.execution_parameters(psopts);                                          \
    alg.initialize(xbegin, xend);                                              \
    alg.solve([=](const value_t *xbegin, value_t *gbegin) -> value_t {         \
      return loss_fcn(xbegin, gbegin, loss_data);                              \
    });                                                                        \
    const auto x = alg.getx();                                                 \
    std::copy(x.begin(), x.end(), xbegin);                                     \
    return alg.getf();                                                         \
  }
PARAMSERVER_MOMENTUM(momentum, paramserver::master, psm)
PARAMSERVER_MOMENTUM(momentum, paramserver::worker, psw)
PARAMSERVER_MOMENTUM(momentum, paramserver::scheduler, pss)

/* Nesterov */
MOMENTUM(nesterov, serial, s)
MOMENTUM(nesterov, consistent, mtc)
INCONSISTENT_MOMENTUM(nesterov)
PARAMSERVER_MOMENTUM(nesterov, paramserver::master, psm)
PARAMSERVER_MOMENTUM(nesterov, paramserver::worker, psw)
PARAMSERVER_MOMENTUM(nesterov, paramserver::scheduler, pss)

/* Adagrad */
#define ADAGRAD(executor, abbrv)                                               \
  value_t CAT3(adagrad, _, abbrv)(                                             \
      value_t * xbegin, value_t * xend, loss_t<value_t> loss_fcn,              \
      void *loss_data, termination_t termination_fcn, void *termination_data,  \
      log_t logger_fcn, void *logger_data, const value_t gamma,                \
      const value_t epsilon) {                                                 \
    algorithm::proxgradient<value_t, index_t, boosting::none, step::constant,  \
                            smoothing::adagrad, prox::none,                    \
                            execution::executor>                               \
        alg;                                                                   \
    alg.step_parameters(gamma);                                                \
    alg.smoothing_parameters(epsilon);                                         \
    alg.initialize(xbegin, xend);                                              \
    alg.solve(                                                                 \
        [=](const value_t *xbegin, value_t *gbegin) -> value_t {               \
          return loss_fcn(xbegin, gbegin, loss_data);                          \
        },                                                                     \
        termination(termination_fcn, termination_data),                        \
        logger(logger_fcn, logger_data));                                      \
    const auto x = alg.getx();                                                 \
    std::copy(x.begin(), x.end(), xbegin);                                     \
    return alg.getf();                                                         \
  }
ADAGRAD(serial, s)
ADAGRAD(consistent, mtc)
value_t adagrad_mti(value_t *xbegin, value_t *xend, loss_t<value_t> loss_fcn,
                    void *loss_data, const value_t gamma,
                    const value_t epsilon) {
  algorithm::proxgradient<value_t, index_t, boosting::none, step::constant,
                          smoothing::adagrad, prox::none,
                          execution::inconsistent>
      alg;
  alg.step_parameters(gamma);
  alg.smoothing_parameters(epsilon);
  alg.initialize(xbegin, xend);
  alg.solve([=](const value_t *xbegin, value_t *gbegin) -> value_t {
    return loss_fcn(xbegin, gbegin, loss_data);
  });
  const auto x = alg.getx();
  std::copy(x.begin(), x.end(), xbegin);
  return alg.getf();
}
#define PARAMSERVER_ADAGRAD(executor, abbrv)                                   \
  value_t CAT3(adagrad, _, abbrv)(value_t * xbegin, value_t * xend,            \
                                  loss_t<value_t> loss_fcn, void *loss_data,   \
                                  option_t option_data, const value_t gamma,   \
                                  const value_t epsilon) {                     \
    auto psopts = get_psopts(option_data);                                     \
    algorithm::proxgradient<value_t, index_t, boosting::none, step::constant,  \
                            smoothing::adagrad, prox::none,                    \
                            execution::executor>                               \
        alg;                                                                   \
    alg.step_parameters(gamma);                                                \
    alg.smoothing_parameters(epsilon);                                         \
    alg.execution_parameters(psopts);                                          \
    alg.initialize(xbegin, xend);                                              \
    alg.solve([=](const value_t *xbegin, value_t *gbegin) -> value_t {         \
      return loss_fcn(xbegin, gbegin, loss_data);                              \
    });                                                                        \
    const auto x = alg.getx();                                                 \
    std::copy(x.begin(), x.end(), xbegin);                                     \
    return alg.getf();                                                         \
  }
PARAMSERVER_ADAGRAD(paramserver::master, psm)
PARAMSERVER_ADAGRAD(paramserver::worker, psw)
PARAMSERVER_ADAGRAD(paramserver::scheduler, pss)

/* Adadelta */
#define ADADELTA(executor, abbrv)                                              \
  value_t CAT3(adadelta, _, abbrv)(                                            \
      value_t * xbegin, value_t * xend, loss_t<value_t> loss_fcn,              \
      void *loss_data, termination_t termination_fcn, void *termination_data,  \
      log_t logger_fcn, void *logger_data, const value_t gamma,                \
      const value_t rho, const value_t epsilon) {                              \
    algorithm::proxgradient<value_t, index_t, boosting::none, step::constant,  \
                            smoothing::adadelta, prox::none,                   \
                            execution::executor>                               \
        alg;                                                                   \
    alg.step_parameters(gamma);                                                \
    alg.smoothing_parameters(rho, epsilon);                                    \
    alg.initialize(xbegin, xend);                                              \
    alg.solve(                                                                 \
        [=](const value_t *xbegin, value_t *gbegin) -> value_t {               \
          return loss_fcn(xbegin, gbegin, loss_data);                          \
        },                                                                     \
        termination(termination_fcn, termination_data),                        \
        logger(logger_fcn, logger_data));                                      \
    const auto x = alg.getx();                                                 \
    std::copy(x.begin(), x.end(), xbegin);                                     \
    return alg.getf();                                                         \
  }

ADADELTA(serial, s)
ADADELTA(consistent, mtc)
value_t adadelta_mti(value_t *xbegin, value_t *xend, loss_t<value_t> loss_fcn,
                     void *loss_data, const value_t gamma, const value_t rho,
                     const value_t epsilon) {
  algorithm::proxgradient<value_t, index_t, boosting::none, step::constant,
                          smoothing::adadelta, prox::none,
                          execution::inconsistent>
      alg;
  alg.step_parameters(gamma);
  alg.smoothing_parameters(rho, epsilon);
  alg.initialize(xbegin, xend);
  alg.solve([=](const value_t *xbegin, value_t *gbegin) -> value_t {
    return loss_fcn(xbegin, gbegin, loss_data);
  });
  const auto x = alg.getx();
  std::copy(x.begin(), x.end(), xbegin);
  return alg.getf();
}
#define PARAMSERVER_ADADELTA(executor, abbrv)                                  \
  value_t CAT3(adadelta, _, abbrv)(value_t * xbegin, value_t * xend,           \
                                   loss_t<value_t> loss_fcn, void *loss_data,  \
                                   option_t option_data, const value_t gamma,  \
                                   const value_t rho, const value_t epsilon) { \
    auto psopts = get_psopts(option_data);                                     \
    algorithm::proxgradient<value_t, index_t, boosting::none, step::constant,  \
                            smoothing::adadelta, prox::none,                   \
                            execution::executor>                               \
        alg;                                                                   \
    alg.step_parameters(gamma);                                                \
    alg.smoothing_parameters(rho, epsilon);                                    \
    alg.execution_parameters(psopts);                                          \
    alg.initialize(xbegin, xend);                                              \
    alg.solve([=](const value_t *xbegin, value_t *gbegin) -> value_t {         \
      return loss_fcn(xbegin, gbegin, loss_data);                              \
    });                                                                        \
    const auto x = alg.getx();                                                 \
    std::copy(x.begin(), x.end(), xbegin);                                     \
    return alg.getf();                                                         \
  }
PARAMSERVER_ADADELTA(paramserver::master, psm)
PARAMSERVER_ADADELTA(paramserver::worker, psw)
PARAMSERVER_ADADELTA(paramserver::scheduler, pss)

/* Adam */
#define ADAM(name, momentum, executor, abbrv)                                  \
  value_t CAT3(name, _, abbrv)(                                                \
      value_t * xbegin, value_t * xend, loss_t<value_t> loss_fcn,              \
      void *loss_data, termination_t termination_fcn, void *termination_data,  \
      log_t logger_fcn, void *logger_data, const value_t mu,                   \
      const value_t epsilon, const value_t gamma, const value_t rho,           \
      const value_t epsilon_rms) {                                             \
    algorithm::proxgradient<value_t, index_t, boosting::momentum,              \
                            step::constant, smoothing::rmsprop, prox::none,    \
                            execution::executor>                               \
        alg;                                                                   \
    alg.boosting_parameters(mu, epsilon);                                      \
    alg.step_parameters(gamma);                                                \
    alg.smoothing_parameters(rho, epsilon_rms);                                \
    alg.initialize(xbegin, xend);                                              \
    alg.solve(                                                                 \
        [=](const value_t *xbegin, value_t *gbegin) -> value_t {               \
          return loss_fcn(xbegin, gbegin, loss_data);                          \
        },                                                                     \
        termination(termination_fcn, termination_data),                        \
        logger(logger_fcn, logger_data));                                      \
    const auto x = alg.getx();                                                 \
    std::copy(x.begin(), x.end(), xbegin);                                     \
    return alg.getf();                                                         \
  }

ADAM(adam, momentum, serial, s)
ADAM(adam, momentum, consistent, mtc)
#define INCONSISTENT_ADAM(name, momentum)                                      \
  value_t CAT3(name, _, mti)(                                                  \
      value_t * xbegin, value_t * xend, loss_t<value_t> loss_fcn,              \
      void *loss_data, const value_t mu, const value_t epsilon,                \
      const value_t gamma, const value_t rho, const value_t epsilon_rms) {     \
    algorithm::proxgradient<value_t, index_t, boosting::momentum,              \
                            step::constant, smoothing::rmsprop, prox::none,    \
                            execution::inconsistent>                           \
        alg;                                                                   \
    alg.boosting_parameters(mu, epsilon);                                      \
    alg.step_parameters(gamma);                                                \
    alg.smoothing_parameters(rho, epsilon_rms);                                \
    alg.initialize(xbegin, xend);                                              \
    alg.solve([=](const value_t *xbegin, value_t *gbegin) -> value_t {         \
      return loss_fcn(xbegin, gbegin, loss_data);                              \
    });                                                                        \
    const auto x = alg.getx();                                                 \
    std::copy(x.begin(), x.end(), xbegin);                                     \
    return alg.getf();                                                         \
  }
INCONSISTENT_ADAM(adam, momentum)
#define PARAMSERVER_ADAM(name, momentum, executor, abbrv)                      \
  value_t CAT3(name, _, abbrv)(value_t * xbegin, value_t * xend,               \
                               loss_t<value_t> loss_fcn, void *loss_data,      \
                               option_t option_data, const value_t mu,         \
                               const value_t epsilon, const value_t gamma,     \
                               const value_t rho, const value_t epsilon_rms) { \
    auto psopts = get_psopts(option_data);                                     \
    algorithm::proxgradient<value_t, index_t, boosting::momentum,              \
                            step::constant, smoothing::rmsprop, prox::none,    \
                            execution::executor>                               \
        alg;                                                                   \
    alg.boosting_parameters(mu, epsilon);                                      \
    alg.step_parameters(gamma);                                                \
    alg.smoothing_parameters(rho, epsilon);                                    \
    alg.execution_parameters(psopts);                                          \
    alg.initialize(xbegin, xend);                                              \
    alg.solve([=](const value_t *xbegin, value_t *gbegin) -> value_t {         \
      return loss_fcn(xbegin, gbegin, loss_data);                              \
    });                                                                        \
    const auto x = alg.getx();                                                 \
    std::copy(x.begin(), x.end(), xbegin);                                     \
    return alg.getf();                                                         \
  }

PARAMSERVER_ADAM(adam, momentum, paramserver::master, psm)
PARAMSERVER_ADAM(adam, momentum, paramserver::worker, psw)
PARAMSERVER_ADAM(adam, momentum, paramserver::scheduler, pss)

/* Nadam */
ADAM(nadam, nesterov, serial, s)
ADAM(nadam, nesterov, consistent, mtc)
INCONSISTENT_ADAM(nadam, nesterov)
PARAMSERVER_ADAM(nadam, nesterov, paramserver::master, psm)
PARAMSERVER_ADAM(nadam, nesterov, paramserver::worker, psw)
PARAMSERVER_ADAM(nadam, nesterov, paramserver::scheduler, pss)

}
