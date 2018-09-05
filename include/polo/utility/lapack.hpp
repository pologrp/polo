#ifndef POLO_UTILITY_LAPACK_HPP_
#define POLO_UTILITY_LAPACK_HPP_

extern "C" {
// Single Precision
void sgeqrfp_(const int *, const int *, float *, const int *, float *, float *,
              const int *, int *);
void sormqr_(const char *, const char *, const int *, const int *, const int *,
             const float *, const int *, const float *, float *, const int *,
             float *, const int *, int *);
void sposv_(const char *, const int *, const int *, float *, const int *,
            float *, const int *, int *);
void spotrf_(const char *, const int *, float *, const int *, int *);
void spotrs_(const char *, const int *, const int *, const float *, const int *,
             float *, const int *, int *);
void sppsv_(const char *, const int *, const int *, float *, float *,
            const int *, int *);
void spptrf_(const char *, const int *, float *, int *);
void spptrs_(const char *, const int *, const int *, const float *, float *,
             const int *, int *);
void spteqr_(const char *, const int *, float *, float *, float *, const int *,
             float *, int *);
void ssptrd_(const char *, const int *, float *, float *, float *, float *,
             int *);
void ssytrd_(const char *, const int *, float *, const int *, float *, float *,
             float *, float *, const int *, int *);
// Double Precision
void dgeqrfp_(const int *, const int *, double *, const int *, double *,
              double *, const int *, int *);
void dormqr_(const char *, const char *, const int *, const int *, const int *,
             const double *, const int *, const double *, double *, const int *,
             double *, const int *, int *);
void dposv_(const char *, const int *, const int *, double *, const int *,
            double *, const int *, int *);
void dpotrf_(const char *, const int *, double *, const int *, int *);
void dpotrs_(const char *, const int *, const int *, const double *,
             const int *, double *, const int *, int *);
void dppsv_(const char *, const int *, const int *, double *, double *,
            const int *, int *);
void dpptrf_(const char *, const int *, double *, int *);
void dpptrs_(const char *, const int *, const int *, const double *, double *,
             const int *, int *);
void dpteqr_(const char *, const int *, double *, double *, double *,
             const int *, double *, int *);
void dsptrd_(const char *, const int *, double *, double *, double *, double *,
             int *);
void dsytrd_(const char *, const int *, double *, const int *, double *,
             double *, double *, double *, const int *, int *);
}

namespace polo {
namespace utility {
namespace matrix {
template <class value_t> struct lapack;

template <> struct lapack<float> {
  static int geqrfp(const int m, const int n, float *a, const int lda,
                    float *tau, float *work, const int lwork) {
    int info;
    sgeqrfp_(&m, &n, a, &lda, tau, work, &lwork, &info);
    return info;
  }
  static int ormqr(const char side, const char trans, const int m, const int n,
                   const int k, const float *a, const int lda, const float *tau,
                   float *c, const int ldc, float *work, const int lwork) {
    int info;
    sormqr_(&side, &trans, &m, &n, &k, a, &lda, tau, c, &ldc, work, &lwork,
            &info);
    return info;
  }
  static int posv(const char uplo, const int n, const int nrhs, float *a,
                  const int lda, float *b, const int ldb) {
    int info;
    sposv_(&uplo, &n, &nrhs, a, &lda, b, &ldb, &info);
    return info;
  }
  static int potrf(const char uplo, const int n, float *a, const int lda) {
    int info;
    spotrf_(&uplo, &n, a, &lda, &info);
    return info;
  }
  static int potrs(const char uplo, const int n, const int nrhs, const float *a,
                   const int lda, float *b, const int ldb) {
    int info;
    spotrs_(&uplo, &n, &nrhs, a, &lda, b, &ldb, &info);
    return info;
  }
  static int ppsv(const char uplo, const int n, const int nrhs, float *ap,
                  float *b, const int ldb) {
    int info;
    sppsv_(&uplo, &n, &nrhs, ap, b, &ldb, &info);
    return info;
  }
  static int pptrf(const char uplo, const int n, float *ap) {
    int info;
    spptrf_(&uplo, &n, ap, &info);
    return info;
  }
  static int pptrs(const char uplo, const int n, const int nrhs,
                   const float *ap, float *b, const int ldb) {
    int info;
    spptrs_(&uplo, &n, &nrhs, ap, b, &ldb, &info);
    return info;
  }
  static int pteqr(const char compz, const int n, float *d, float *e, float *z,
                   const int ldz, float *work) {
    int info;
    spteqr_(&compz, &n, d, e, z, &ldz, work, &info);
    return info;
  }
  static int sptrd(const char uplo, const int n, float *ap, float *d, float *e,
                   float *tau) {
    int info;
    ssptrd_(&uplo, &n, ap, d, e, tau, &info);
    return info;
  }
  static int sytrd(const char uplo, const int n, float *a, const int lda,
                   float *d, float *e, float *tau, float *work,
                   const int lwork) {
    int info;
    ssytrd_(&uplo, &n, a, &lda, d, e, tau, work, &lwork, &info);
    return info;
  }
};

template <> struct lapack<double> {
  static int geqrfp(const int m, const int n, double *a, const int lda,
                    double *tau, double *work, const int lwork) {
    int info;
    dgeqrfp_(&m, &n, a, &lda, tau, work, &lwork, &info);
    return info;
  }
  static int ormqr(const char side, const char trans, const int m, const int n,
                   const int k, const double *a, const int lda,
                   const double *tau, double *c, const int ldc, double *work,
                   const int lwork) {
    int info;
    dormqr_(&side, &trans, &m, &n, &k, a, &lda, tau, c, &ldc, work, &lwork,
            &info);
    return info;
  }
  static int posv(const char uplo, const int n, const int nrhs, double *a,
                  const int lda, double *b, const int ldb) {
    int info;
    dposv_(&uplo, &n, &nrhs, a, &lda, b, &ldb, &info);
    return info;
  }
  static int potrf(const char uplo, const int n, double *a, const int lda) {
    int info;
    dpotrf_(&uplo, &n, a, &lda, &info);
    return info;
  }
  static int potrs(const char uplo, const int n, const int nrhs,
                   const double *a, const int lda, double *b, const int ldb) {
    int info;
    dpotrs_(&uplo, &n, &nrhs, a, &lda, b, &ldb, &info);
    return info;
  }
  static int ppsv(const char uplo, const int n, const int nrhs, double *ap,
                  double *b, const int ldb) {
    int info;
    dppsv_(&uplo, &n, &nrhs, ap, b, &ldb, &info);
    return info;
  }
  static int pptrf(const char uplo, const int n, double *ap) {
    int info;
    dpptrf_(&uplo, &n, ap, &info);
    return info;
  }
  static int pptrs(const char uplo, const int n, const int nrhs,
                   const double *ap, double *b, const int ldb) {
    int info;
    dpptrs_(&uplo, &n, &nrhs, ap, b, &ldb, &info);
    return info;
  }
  static int pteqr(const char compz, const int n, double *d, double *e,
                   double *z, const int ldz, double *work) {
    int info;
    dpteqr_(&compz, &n, d, e, z, &ldz, work, &info);
    return info;
  }
  static int sptrd(const char uplo, const int n, double *ap, double *d,
                   double *e, double *tau) {
    int info;
    dsptrd_(&uplo, &n, ap, d, e, tau, &info);
    return info;
  }
  static int sytrd(const char uplo, const int n, double *a, const int lda,
                   double *d, double *e, double *tau, double *work,
                   const int lwork) {
    int info;
    dsytrd_(&uplo, &n, a, &lda, d, e, tau, work, &lwork, &info);
    return info;
  }
};
} // namespace matrix
} // namespace utility
} // namespace polo

#endif
