#ifndef POLO_PS_API_H_
#define POLO_PS_API_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef double value_t;
typedef int index_t;

typedef void (*init_t)(const value_t *, const value_t *, void *);
typedef value_t *(*boost_t)(const index_t, const index_t, const index_t,
                            const value_t *, const value_t *, value_t *,
                            void *);
typedef value_t (*step_t)(const index_t, const index_t, const value_t,
                          const value_t *, const value_t *, const value_t *,
                          void *);
typedef value_t *(*smoothing_t)(const index_t, const index_t, const value_t *,
                                const value_t *, const value_t *, value_t *,
                                void *);
typedef value_t *(*prox_t)(const value_t, const value_t *, const value_t *,
                           const value_t *, value_t *, void *);

typedef void (*master_log_t)(const index_t, const value_t, const value_t *,
                             const value_t *, const value_t *, void *);

typedef value_t (*loss_t)(const value_t *, value_t *, void *);

typedef struct {
  int id;
  char what[256];
} err_t;

typedef struct {
  int linger;
  long mtimeout, wtimeout, stimeout;
  int32_t num_masters;
  char saddress[256], maddress[256];
  uint16_t sports[3];
  uint16_t mworker;
} ps_opt_t;

err_t paramserver_m(master_log_t logger, const ps_opt_t options, init_t init,
                    boost_t boost_compute, void *boost_data,
                    step_t step_compute, void *step_data,
                    smoothing_t smoothing_compute, void *smoothing_data,
                    prox_t prox_compute, void *prox_data);

err_t paramserver_s(const value_t *xbegin, const value_t *xend, const index_t K,
                    const ps_opt_t options);

err_t paramserver_w(const value_t *xbegin, const value_t *xend, loss_t loss,
                    void *loss_data, const ps_opt_t options);

#ifdef __cplusplus
}
#endif

#endif
