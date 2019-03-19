#ifndef POLO_API_H_
#define POLO_API_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Type definitions

typedef double polo_value_t;
typedef int polo_index_t;

typedef struct {
  int id;
  char what[256];
} polo_error_t;

typedef struct {
  int linger;
  long mtimeout, wtimeout, stimeout;
  int32_t num_masters;
  char saddress[256], maddress[256];
  uint16_t sports[3];
  uint16_t mworker;
} polo_ps_options_t;

typedef void (*polo_init_t)(const polo_value_t *xbegin,
                            const polo_value_t *xend, void *init_d);

typedef polo_value_t *(*polo_boost_t)(const polo_index_t widx,
                                      const polo_index_t klocal,
                                      const polo_index_t kglobal,
                                      const polo_value_t *gold_b,
                                      const polo_value_t *gold_e,
                                      polo_value_t *gcurr_b, void *boost_d);

typedef polo_value_t (*polo_step_t)(const polo_index_t klocal,
                                    const polo_index_t kglobal,
                                    const polo_value_t fval,
                                    const polo_value_t *xbegin,
                                    const polo_value_t *xend,
                                    const polo_value_t *gbegin, void *step_d);

typedef polo_value_t *(*polo_smooth_t)(const polo_index_t klocal,
                                       const polo_index_t kglobal,
                                       const polo_value_t *xbegin,
                                       const polo_value_t *xend,
                                       const polo_value_t *gold_b,
                                       polo_value_t *gcurr_b, void *smooth_d);

typedef polo_value_t *(*polo_prox_t)(const polo_value_t step,
                                     const polo_value_t *xold_b,
                                     const polo_value_t *xold_e,
                                     const polo_value_t *gbegin,
                                     polo_value_t *xcurr_b, void *prox_d);

typedef polo_value_t (*polo_loss_t)(const polo_value_t *xbegin,
                                    polo_value_t *gbegin, void *loss_d);

typedef void (*polo_logger_t)(const polo_index_t k, const polo_value_t fval,
                              const polo_value_t *xbegin,
                              const polo_value_t *xend,
                              const polo_value_t *gbegin, void *log_d);

typedef int (*polo_terminator_t)(const polo_index_t k, const polo_value_t fval,
                                 const polo_value_t *xbegin,
                                 const polo_value_t *xend,
                                 const polo_value_t *gbegin, void *term_d);

// Executors with custom policies

polo_error_t polo_proxgrad_s(const polo_value_t *xbegin,
                             const polo_value_t *xend, polo_loss_t loss_f,
                             void *loss_d, polo_logger_t log_f, void *log_d,
                             polo_terminator_t term_f, void *term_d,
                             polo_init_t init_f, polo_boost_t boost_f,
                             void *boost_d, polo_step_t step_f, void *step_d,
                             polo_smooth_t smooth_f, void *smooth_d,
                             polo_prox_t prox_f, void *prox_d);

polo_error_t polo_proxgrad_cr(
    const polo_value_t *xbegin, const polo_value_t *xend, polo_loss_t loss_f,
    void *loss_d, polo_logger_t log_f, void *log_d, polo_terminator_t term_f,
    void *term_d, const unsigned int nthreads, polo_init_t init_f,
    polo_boost_t boost_f, void *boost_d, polo_step_t step_f, void *step_d,
    polo_smooth_t smooth_f, void *smooth_d, polo_prox_t prox_f, void *prox_d);

polo_error_t polo_proxgrad_psm(polo_logger_t log_f, void *log_d,
                               polo_terminator_t term_f, void *term_d,
                               const polo_ps_options_t opts, polo_init_t init_f,
                               polo_boost_t boost_f, void *boost_d,
                               polo_step_t step_f, void *step_d,
                               polo_smooth_t smooth_f, void *smooth_d,
                               polo_prox_t prox_f, void *prox_d);

polo_error_t polo_proxgrad_pss(const polo_value_t *xbegin,
                               const polo_value_t *xend, polo_logger_t log_f,
                               void *log_d, polo_terminator_t term_f,
                               void *term_d, const polo_ps_options_t opts);

polo_error_t polo_proxgrad_psw(const polo_value_t *xbegin,
                               const polo_value_t *xend, polo_loss_t loss_f,
                               void *loss_d, polo_logger_t log_f, void *log_d,
                               const polo_ps_options_t opts);

// Specific solvers

polo_error_t polo_gradient_s(polo_value_t *fval, polo_value_t *xbegin,
                             polo_value_t *xend, polo_loss_t loss_f,
                             void *loss_d, polo_logger_t log_f, void *log_d,
                             polo_terminator_t term_f, void *term_d,
                             const polo_value_t gamma);

polo_error_t polo_gradient_cr(polo_value_t *fval, polo_value_t *xbegin,
                              polo_value_t *xend, polo_loss_t loss_f,
                              void *loss_d, polo_logger_t log_f, void *log_d,
                              polo_terminator_t term_f, void *term_d,
                              const polo_value_t gamma,
                              const unsigned int nthreads);

polo_error_t polo_gradient_psm(polo_logger_t log_f, void *log_d,
                               polo_terminator_t term_f, void *term_d,
                               const polo_value_t gamma,
                               const polo_ps_options_t opts);

polo_error_t polo_gradient_pss(const polo_value_t *xbegin,
                               const polo_value_t *xend, polo_logger_t log_f,
                               void *log_d, polo_terminator_t term_f,
                               void *term_d, const polo_ps_options_t opts);

polo_error_t polo_gradient_psw(const polo_value_t *xbegin,
                               const polo_value_t *xend, polo_loss_t loss_f,
                               void *loss_d, polo_logger_t log_f, void *log_d,
                               const polo_ps_options_t opts);

polo_error_t polo_momentum_s(polo_value_t *fval, polo_value_t *xbegin,
                             polo_value_t *xend, polo_loss_t loss_f,
                             void *loss_d, polo_logger_t log_f, void *log_d,
                             polo_terminator_t term_f, void *term_d,
                             const polo_value_t mu, const polo_value_t epsilon);

polo_error_t polo_momentum_cr(polo_value_t *fval, polo_value_t *xbegin,
                              polo_value_t *xend, polo_loss_t loss_f,
                              void *loss_d, polo_logger_t log_f, void *log_d,
                              polo_terminator_t term_f, void *term_d,
                              const polo_value_t mu, const polo_value_t epsilon,
                              const unsigned int nthreads);

polo_error_t polo_momentum_psm(polo_logger_t log_f, void *log_d,
                               polo_terminator_t term_f, void *term_d,
                               const polo_value_t mu,
                               const polo_value_t epsilon,
                               const polo_ps_options_t opts);

polo_error_t polo_momentum_pss(const polo_value_t *xbegin,
                               const polo_value_t *xend, polo_logger_t log_f,
                               void *log_d, polo_terminator_t term_f,
                               void *term_d, const polo_ps_options_t opts);

polo_error_t polo_momentum_psw(const polo_value_t *xbegin,
                               const polo_value_t *xend, polo_loss_t loss_f,
                               void *loss_d, polo_logger_t log_f, void *log_d,
                               const polo_ps_options_t opts);

polo_error_t polo_nesterov_s(polo_value_t *fval, polo_value_t *xbegin,
                             polo_value_t *xend, polo_loss_t loss_f,
                             void *loss_d, polo_logger_t log_f, void *log_d,
                             polo_terminator_t term_f, void *term_d,
                             const polo_value_t mu, const polo_value_t epsilon);

polo_error_t polo_nesterov_cr(polo_value_t *fval, polo_value_t *xbegin,
                              polo_value_t *xend, polo_loss_t loss_f,
                              void *loss_d, polo_logger_t log_f, void *log_d,
                              polo_terminator_t term_f, void *term_d,
                              const polo_value_t mu, const polo_value_t epsilon,
                              const unsigned int nthreads);

polo_error_t polo_nesterov_psm(polo_logger_t log_f, void *log_d,
                               polo_terminator_t term_f, void *term_d,
                               const polo_value_t mu,
                               const polo_value_t epsilon,
                               const polo_ps_options_t opts);

polo_error_t polo_nesterov_pss(const polo_value_t *xbegin,
                               const polo_value_t *xend, polo_logger_t log_f,
                               void *log_d, polo_terminator_t term_f,
                               void *term_d, const polo_ps_options_t opts);

polo_error_t polo_nesterov_psw(const polo_value_t *xbegin,
                               const polo_value_t *xend, polo_loss_t loss_f,
                               void *loss_d, polo_logger_t log_f, void *log_d,
                               const polo_ps_options_t opts);

polo_error_t polo_adagrad_s(polo_value_t *fval, polo_value_t *xbegin,
                            polo_value_t *xend, polo_loss_t loss_f,
                            void *loss_d, polo_logger_t log_f, void *log_d,
                            polo_terminator_t term_f, void *term_d,
                            const polo_value_t gamma,
                            const polo_value_t epsilon);

polo_error_t polo_adagrad_cr(polo_value_t *fval, polo_value_t *xbegin,
                             polo_value_t *xend, polo_loss_t loss_f,
                             void *loss_d, polo_logger_t log_f, void *log_d,
                             polo_terminator_t term_f, void *term_d,
                             const polo_value_t gamma,
                             const polo_value_t epsilon,
                             const unsigned int nthreads);

polo_error_t polo_adagrad_psm(polo_logger_t log_f, void *log_d,
                              polo_terminator_t term_f, void *term_d,
                              const polo_value_t gamma,
                              const polo_value_t epsilon,
                              const polo_ps_options_t opts);

polo_error_t polo_adagrad_pss(const polo_value_t *xbegin,
                              const polo_value_t *xend, polo_logger_t log_f,
                              void *log_d, polo_terminator_t term_f,
                              void *term_d, const polo_ps_options_t opts);

polo_error_t polo_adagrad_psw(const polo_value_t *xbegin,
                              const polo_value_t *xend, polo_loss_t loss_f,
                              void *loss_d, polo_logger_t log_f, void *log_d,
                              const polo_ps_options_t opts);

polo_error_t polo_adadelta_s(polo_value_t *fval, polo_value_t *xbegin,
                             polo_value_t *xend, polo_loss_t loss_f,
                             void *loss_d, polo_logger_t log_f, void *log_d,
                             polo_terminator_t term_f, void *term_d,
                             const polo_value_t rho,
                             const polo_value_t epsilon);

polo_error_t polo_adadelta_cr(polo_value_t *fval, polo_value_t *xbegin,
                              polo_value_t *xend, polo_loss_t loss_f,
                              void *loss_d, polo_logger_t log_f, void *log_d,
                              polo_terminator_t term_f, void *term_d,
                              const polo_value_t rho,
                              const polo_value_t epsilon,
                              const unsigned int nthreads);

polo_error_t polo_adadelta_psm(polo_logger_t log_f, void *log_d,
                               polo_terminator_t term_f, void *term_d,
                               const polo_value_t rho,
                               const polo_value_t epsilon,
                               const polo_ps_options_t opts);

polo_error_t polo_adadelta_pss(const polo_value_t *xbegin,
                               const polo_value_t *xend, polo_logger_t log_f,
                               void *log_d, polo_terminator_t term_f,
                               void *term_d, const polo_ps_options_t opts);

polo_error_t polo_adadelta_psw(const polo_value_t *xbegin,
                               const polo_value_t *xend, polo_loss_t loss_f,
                               void *loss_d, polo_logger_t log_f, void *log_d,
                               const polo_ps_options_t opts);

polo_error_t polo_adam_s(polo_value_t *fval, polo_value_t *xbegin,
                         polo_value_t *xend, polo_loss_t loss_f, void *loss_d,
                         polo_logger_t log_f, void *log_d,
                         polo_terminator_t term_f, void *term_d,
                         const polo_value_t gamma, const polo_value_t beta1,
                         const polo_value_t beta2, const polo_value_t epsilon);

polo_error_t polo_adam_cr(polo_value_t *fval, polo_value_t *xbegin,
                          polo_value_t *xend, polo_loss_t loss_f, void *loss_d,
                          polo_logger_t log_f, void *log_d,
                          polo_terminator_t term_f, void *term_d,
                          const polo_value_t gamma, const polo_value_t beta1,
                          const polo_value_t beta2, const polo_value_t epsilon,
                          const unsigned int nthreads);

polo_error_t polo_adam_psm(polo_logger_t log_f, void *log_d,
                           polo_terminator_t term_f, void *term_d,
                           const polo_value_t gamma, const polo_value_t beta1,
                           const polo_value_t beta2, const polo_value_t epsilon,
                           const polo_ps_options_t opts);

polo_error_t polo_adam_pss(const polo_value_t *xbegin, const polo_value_t *xend,
                           polo_logger_t log_f, void *log_d,
                           polo_terminator_t term_f, void *term_d,
                           const polo_ps_options_t opts);

polo_error_t polo_adam_psw(const polo_value_t *xbegin, const polo_value_t *xend,
                           polo_loss_t loss_f, void *loss_d,
                           polo_logger_t log_f, void *log_d,
                           const polo_ps_options_t opts);

polo_error_t polo_nadam_s(polo_value_t *fval, polo_value_t *xbegin,
                          polo_value_t *xend, polo_loss_t loss_f, void *loss_d,
                          polo_logger_t log_f, void *log_d,
                          polo_terminator_t term_f, void *term_d,
                          const polo_value_t gamma, const polo_value_t beta1,
                          const polo_value_t beta2, const polo_value_t epsilon);

polo_error_t polo_nadam_cr(polo_value_t *fval, polo_value_t *xbegin,
                           polo_value_t *xend, polo_loss_t loss_f, void *loss_d,
                           polo_logger_t log_f, void *log_d,
                           polo_terminator_t term_f, void *term_d,
                           const polo_value_t gamma, const polo_value_t beta1,
                           const polo_value_t beta2, const polo_value_t epsilon,
                           const unsigned int nthreads);

polo_error_t polo_nadam_psm(polo_logger_t log_f, void *log_d,
                            polo_terminator_t term_f, void *term_d,
                            const polo_value_t gamma, const polo_value_t beta1,
                            const polo_value_t beta2,
                            const polo_value_t epsilon,
                            const polo_ps_options_t opts);

polo_error_t polo_nadam_pss(const polo_value_t *xbegin,
                            const polo_value_t *xend, polo_logger_t log_f,
                            void *log_d, polo_terminator_t term_f, void *term_d,
                            const polo_ps_options_t opts);

polo_error_t polo_nadam_psw(const polo_value_t *xbegin,
                            const polo_value_t *xend, polo_loss_t loss_f,
                            void *loss_d, polo_logger_t log_f, void *log_d,
                            const polo_ps_options_t opts);

#ifdef __cplusplus
}
#endif

#endif
