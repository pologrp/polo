#ifndef BLAS_HPP_
#define BLAS_HPP_

extern "C" {
// Single-precision, L1
void srotg_(const float *, const float *, float *, float *);
void srotmg_(float *, float *, float *, const float *, float *);
void srot_(const int *, float *, const int *, float *, const int *,
           const float *, const float *);
void srotm_(const int *, float *, const int *, float *, const int *,
            const int *);
void sswap_(const int *, float *, const int *, float *, const int *);
void sscal_(const int *, const float *, float *, const int *);
void scopy_(const int *, const float *, const int *, float *, const int *);
void saxpy_(const int *, const float *, const float *, const int *, float *,
            const int *);
float sdot_(const int *, const float *, const int *, const float *,
            const int *);
float sdsdot_(const int *, const float *, const float *, const int *,
              const float *, const int *);
float snrm2_(const int *, const float *, const int *);
float sasum_(const int *, const float *, const int *);
int isamax_(const int *, const float *, const int *);
// Single-precision, L2
void sgemv_(const char *, const int *, const int *, const float *,
            const float *, const int *, const float *, const int *,
            const float *, float *, const int *);
void sgbmv_(const char *, const int *, const int *, const int *, const int *,
            const float *, const float *, const int *, const float *,
            const int *, const float *, float *, const int *);
void ssymv_(const char *, const int *, const float *, const float *,
            const int *, const float *, const int *, const float *, float *,
            const int *);
void ssbmv_(const char *, const int *, const int *, const float *,
            const float *, const int *, const int *, const int *, const float *,
            float *, const int *);
void sspmv_(const char *, const int *, const float *, const float *,
            const float *, const int *, const float *, float *, const int *);
void strmv_(const char *, const char *, const char *, const int *,
            const float *, const int *, float *, const int *);
void stbmv_(const char *, const char *, const char *, const int *, const int *,
            const float *, const int *, float *, const int *);
void stpmv_(const char *, const char *, const char *, const int *,
            const float *, float *, const int *);
void strsv_(const char *, const char *, const char *, const int *,
            const float *, const int *, float *, const int *);
void stbsv_(const char *, const char *, const char *, const int *, const int *,
            const float *, const int *, float *, const int *);
void stpsv_(const char *, const char *, const char *, const int *,
            const float *, float *, const int *);
void sger_(const int *, const int *, const float *, const float *, const int *,
           const float *, const int *, float *, const int *);
void ssyr_(const char *, const int *, const float *, const float *, const int *,
           float *, const int *);
void sspr_(const char *, const int *, const float *, const float *, const int *,
           float *);
void ssyr2_(const char *, const int *, const float *, const float *,
            const int *, const float *, const int *, float *, const int *);
void sspr2_(const char *, const int *, const float *, const float *,
            const int *, const float *, const int *, float *);
// Single-precision, L3
void sgemm_(const char *, const char *, const int *, const int *, const int *,
            const float *, const float *, const int *, const float *,
            const int *, const float *, float *, const int *);
void ssymm_(const char *, const char *, const int *, const int *, const float *,
            const float *, const int *, const float *, const int *,
            const float *, float *, const int *);
void ssyrk_(const char *, const char *, const int *, const int *, const float *,
            const float *, const int *, const float *, float *, const int *);
void ssyr2k_(const char *, const char *, const int *, const int *,
             const float *, const float *, const int *, const float *,
             const int *, const float *, float *, const int *);
void strmm_(const char *, const char *, const char *, const char *, const int *,
            const int *, const float *, const float *, const int *, float *,
            const int *);
void strsm_(const char *, const char *, const char *, const char *, const int *,
            const int *, const float *, const float *, const int *, float *,
            const int *);
// Double-precision, L1
void drotg_(const double *, const double *, double *, double *);
void drotmg_(double *, double *, double *, const double *, double *);
void drot_(const int *, double *, const int *, double *, const int *,
           const double *, const double *);
void drotm_(const int *, double *, const int *, double *, const int *,
            const int *);
void dswap_(const int *, double *, const int *, double *, const int *);
void dscal_(const int *, const double *, double *, const int *);
void dcopy_(const int *, const double *, const int *, double *, const int *);
void daxpy_(const int *, const double *, const double *, const int *, double *,
            const int *);
double ddot_(const int *, const double *, const int *, const double *,
             const int *);
double dsdot_(const int *, const double *, const double *, const int *,
              const double *, const int *);
double dnrm2_(const int *, const double *, const int *);
double dasum_(const int *, const double *, const int *);
int idamax_(const int *, const double *, const int *);
// Double-precision, L2
void dgemv_(const char *, const int *, const int *, const double *,
            const double *, const int *, const double *, const int *,
            const double *, double *, const int *);
void dgbmv_(const char *, const int *, const int *, const int *, const int *,
            const double *, const double *, const int *, const double *,
            const int *, const double *, double *, const int *);
void dsymv_(const char *, const int *, const double *, const double *,
            const int *, const double *, const int *, const double *, double *,
            const int *);
void dsbmv_(const char *, const int *, const int *, const double *,
            const double *, const int *, const int *, const int *,
            const double *, double *, const int *);
void dspmv_(const char *, const int *, const double *, const double *,
            const double *, const int *, const double *, double *, const int *);
void dtrmv_(const char *, const char *, const char *, const int *,
            const double *, const int *, double *, const int *);
void dtbmv_(const char *, const char *, const char *, const int *, const int *,
            const double *, const int *, double *, const int *);
void dtpmv_(const char *, const char *, const char *, const int *,
            const double *, double *, const int *);
void dtrsv_(const char *, const char *, const char *, const int *,
            const double *, const int *, double *, const int *);
void dtbsv_(const char *, const char *, const char *, const int *, const int *,
            const double *, const int *, double *, const int *);
void dtpsv_(const char *, const char *, const char *, const int *,
            const double *, double *, const int *);
void dger_(const int *, const int *, const double *, const double *,
           const int *, const double *, const int *, double *, const int *);
void dsyr_(const char *, const int *, const double *, const double *,
           const int *, double *, const int *);
void dspr_(const char *, const int *, const double *, const double *,
           const int *, double *);
void dsyr2_(const char *, const int *, const double *, const double *,
            const int *, const double *, const int *, double *, const int *);
void dspr2_(const char *, const int *, const double *, const double *,
            const int *, const double *, const int *, double *);
// Double-precision, L3
void dgemm_(const char *, const char *, const int *, const int *, const int *,
            const double *, const double *, const int *, const double *,
            const int *, const double *, double *, const int *);
void dsymm_(const char *, const char *, const int *, const int *,
            const double *, const double *, const int *, const double *,
            const int *, const double *, double *, const int *);
void dsyrk_(const char *, const char *, const int *, const int *,
            const double *, const double *, const int *, const double *,
            double *, const int *);
void dsyr2k_(const char *, const char *, const int *, const int *,
             const double *, const double *, const int *, const double *,
             const int *, const double *, double *, const int *);
void dtrmm_(const char *, const char *, const char *, const char *, const int *,
            const int *, const double *, const double *, const int *, double *,
            const int *);
void dtrsm_(const char *, const char *, const char *, const char *, const int *,
            const int *, const double *, const double *, const int *, double *,
            const int *);
}

namespace pbopt {
namespace utility {

template <class float_t> struct blas;

template <> struct blas<float> {
  // Level 1
  static void rotg(const float sa, const float sb, float &c, float &s) {
    srotg_(&sa, &sb, &c, &s);
  }
  static void rotmg(float &sd1, float &sd2, float &sx1, const float sy1,
                    float *sparam) {
    srotmg_(&sd1, &sd2, &sx1, &sy1, sparam);
  }
  static void rot(const int n, float *sx, const int incx, float *sy,
                  const int incy, const float c, const float s) {
    srot_(&n, sx, &incx, sy, &incy, &c, &s);
  }
  static void rotm(const int n, float *sx, const int incx, float *sy,
                   const int incy, const int *sparam) {
    srotm_(&n, sx, &incx, sy, &incy, sparam);
  }
  static void swap(const int n, float *sx, const int incx, float *sy,
                   const int incy) {
    sswap_(&n, sx, &incx, sy, &incy);
  }
  static void scal(const int n, const float sa, float *sx, const int incx) {
    sscal_(&n, &sa, sx, &incx);
  }
  static void copy(const int n, const float *sx, const int incx, float *sy,
                   const int incy) {
    scopy_(&n, sx, &incx, sy, &incy);
  }
  static void axpy(const int n, const float sa, const float *sx, const int incx,
                   float *sy, const int incy) {
    saxpy_(&n, &sa, sx, &incx, sy, &incy);
  }
  static float dot(const int n, const float *sx, const int incx,
                   const float *sy, const int incy) {
    return sdot_(&n, sx, &incx, sy, &incy);
  }
  static float dsdot(const int n, const float sb, const float *sx,
                     const int incx, const float *sy, const int incy) {
    return sdsdot_(&n, &sb, sx, &incx, sy, &incy);
  }
  static float nrm2(const int n, const float *x, const int incx) {
    return snrm2_(&n, x, &incx);
  }
  static float asum(const int n, const float *sx, const int incx) {
    return sasum_(&n, sx, &incx);
  }
  static int iamax(const int n, const float *sx, const int incx) {
    return isamax_(&n, sx, &incx);
  }
  // Level 2
  static void gemv(const char trans, const int m, const int n,
                   const float alpha, const float *a, const int lda,
                   const float *x, const int incx, const float beta, float *y,
                   const int incy) {
    sgemv_(&trans, &m, &n, &alpha, a, &lda, x, &incx, &beta, y, &incy);
  }
  static void gbmv(const char trans, const int m, const int n, const int kl,
                   const int ku, const float alpha, const float *a,
                   const int lda, const float *x, const int incx,
                   const float beta, float *y, const int incy) {
    sgbmv_(&trans, &m, &n, &kl, &ku, &alpha, a, &lda, x, &incx, &beta, y,
           &incy);
  }
  static void symv(const char uplo, const int n, const float alpha,
                   const float *a, const int lda, const float *x,
                   const int incx, const float beta, float *y, const int incy) {
    ssymv_(&uplo, &n, &alpha, a, &lda, x, &incx, &beta, y, &incy);
  }
  static void sbmv(const char uplo, const int n, const int k, const float alpha,
                   const float *a, const int lda, const int *x, const int incx,
                   const float beta, float *y, const int incy) {
    ssbmv_(&uplo, &n, &k, &alpha, a, &lda, x, &incx, &beta, y, &incy);
  }
  static void spmv(const char uplo, const int n, const float alpha,
                   const float *a, const float *x, const int incx,
                   const float beta, float *y, const int incy) {
    sspmv_(&uplo, &n, &alpha, a, x, &incx, &beta, y, &incy);
  }
  static void trmv(const char uplo, const char trans, const char diag,
                   const int n, const float *a, const int lda, float *x,
                   const int incx) {
    strmv_(&uplo, &trans, &diag, &n, a, &lda, x, &incx);
  }
  static void tbmv(const char uplo, const char trans, const char diag,
                   const int n, const int k, const float *a, const int lda,
                   float *x, const int incx) {
    stbmv_(&uplo, &trans, &diag, &n, &k, a, &lda, x, &incx);
  }
  static void tpmv(const char uplo, const char trans, const char diag,
                   const int n, const float *ap, float *x, const int incx) {
    stpmv_(&uplo, &trans, &diag, &n, ap, x, &incx);
  }
  static void trsv(const char uplo, const char trans, const char diag,
                   const int n, const float *a, const int lda, float *x,
                   const int incx) {
    strsv_(&uplo, &trans, &diag, &n, a, &lda, x, &incx);
  }
  static void tbsv(const char uplo, const char trans, const char diag,
                   const int n, const int k, const float *a, const int lda,
                   float *x, const int incx) {
    stbsv_(&uplo, &trans, &diag, &n, &k, a, &lda, x, &incx);
  }
  static void tpsv(const char uplo, const char trans, const char diag,
                   const int n, const float *ap, float *x, const int incx) {
    stpsv_(&uplo, &trans, &diag, &n, ap, x, &incx);
  }
  static void ger(const int m, const int n, const float alpha, const float *x,
                  const int incx, const float *y, const int incy, float *a,
                  const int lda) {
    sger_(&m, &n, &alpha, x, &incx, y, &incy, a, &lda);
  }
  static void syr(const char uplo, const int n, const float alpha,
                  const float *x, const int incx, float *a, const int lda) {
    ssyr_(&uplo, &n, &alpha, x, &incx, a, &lda);
  }
  static void spr(const char uplo, const int n, const float alpha,
                  const float *x, const int incx, float *ap) {
    sspr_(&uplo, &n, &alpha, x, &incx, ap);
  }
  static void syr2(const char uplo, const int n, const float alpha,
                   const float *x, const int incx, const float *y,
                   const int incy, float *a, const int lda) {
    ssyr2_(&uplo, &n, &alpha, x, &incx, y, &incy, a, &lda);
  }
  static void spr2(const char uplo, const int n, const float alpha,
                   const float *x, const int incx, const float *y,
                   const int incy, float *ap) {
    sspr2_(&uplo, &n, &alpha, x, &incx, y, &incy, ap);
  }
  // Level 3
  static void gemm(const char transa, const char transb, const int m,
                   const int n, const int k, const float alpha, const float *a,
                   const int lda, const float *b, const int ldb,
                   const float beta, float *c, const int ldc) {
    sgemm_(&transa, &transb, &m, &n, &k, &alpha, a, &lda, b, &ldb, &beta, c,
           &ldc);
  }
  static void symm(const char side, const char uplo, const int m, const int n,
                   const float alpha, const float *a, const int lda,
                   const float *b, const int ldb, const float beta, float *c,
                   const int ldc) {
    ssymm_(&side, &uplo, &m, &n, &alpha, a, &lda, b, &ldb, &beta, c, &ldc);
  }
  static void syrk(const char uplo, const char trans, const int n, const int k,
                   const float alpha, const float *a, const int lda,
                   const float beta, float *c, const int ldc) {
    ssyrk_(&uplo, &trans, &n, &k, &alpha, a, &lda, &beta, c, &ldc);
  }
  static void syr2k(const char uplo, const char trans, const int n, const int k,
                    const float alpha, const float *a, const int lda,
                    const float *b, const int ldb, const float beta, float *c,
                    const int ldc) {
    ssyr2k_(&uplo, &trans, &n, &k, &alpha, a, &lda, b, &ldb, &beta, c, &ldc);
  }
  static void trmm(const char side, const char uplo, const char transa,
                   const char diag, const int m, const int n, const float alpha,
                   const float *a, const int lda, float *b, const int ldb) {
    strmm_(&side, &uplo, &transa, &diag, &m, &n, &alpha, a, &lda, b, &ldb);
  }
  static void trsm(const char side, const char uplo, const char transa,
                   const char diag, const int m, const int n, const float alpha,
                   const float *a, const int lda, float *b, const int ldb) {
    strsm_(&side, &uplo, &transa, &diag, &m, &n, &alpha, a, &lda, b, &ldb);
  }
};

template <> struct blas<double> {
  // Level 1
  static void rotg(const double sa, const double sb, double &c, double &s) {
    drotg_(&sa, &sb, &c, &s);
  }
  static void rotmg(double &sd1, double &sd2, double &sx1, const double sy1,
                    double *sparam) {
    drotmg_(&sd1, &sd2, &sx1, &sy1, sparam);
  }
  static void rot(const int n, double *sx, const int incx, double *sy,
                  const int incy, const double c, const double s) {
    drot_(&n, sx, &incx, sy, &incy, &c, &s);
  }
  static void rotm(const int n, double *sx, const int incx, double *sy,
                   const int incy, const int *sparam) {
    drotm_(&n, sx, &incx, sy, &incy, sparam);
  }
  static void swap(const int n, double *sx, const int incx, double *sy,
                   const int incy) {
    dswap_(&n, sx, &incx, sy, &incy);
  }
  static void scal(const int n, const double sa, double *sx, const int incx) {
    dscal_(&n, &sa, sx, &incx);
  }
  static void copy(const int n, const double *sx, const int incx, double *sy,
                   const int incy) {
    dcopy_(&n, sx, &incx, sy, &incy);
  }
  static void axpy(const int n, const double sa, const double *sx,
                   const int incx, double *sy, const int incy) {
    daxpy_(&n, &sa, sx, &incx, sy, &incy);
  }
  static double dot(const int n, const double *sx, const int incx,
                    const double *sy, const int incy) {
    return ddot_(&n, sx, &incx, sy, &incy);
  }
  static double dsdot(const int n, const double sb, const double *sx,
                      const int incx, const double *sy, const int incy) {
    return dsdot_(&n, &sb, sx, &incx, sy, &incy);
  }
  static double nrm2(const int n, const double *x, const int incx) {
    return dnrm2_(&n, x, &incx);
  }
  static double asum(const int n, const double *sx, const int incx) {
    return dasum_(&n, sx, &incx);
  }
  static int iamax(const int n, const double *sx, const int incx) {
    return idamax_(&n, sx, &incx);
  }
  // Level 2
  static void gemv(const char trans, const int m, const int n,
                   const double alpha, const double *a, const int lda,
                   const double *x, const int incx, const double beta,
                   double *y, const int incy) {
    dgemv_(&trans, &m, &n, &alpha, a, &lda, x, &incx, &beta, y, &incy);
  }
  static void gbmv(const char trans, const int m, const int n, const int kl,
                   const int ku, const double alpha, const double *a,
                   const int lda, const double *x, const int incx,
                   const double beta, double *y, const int incy) {
    dgbmv_(&trans, &m, &n, &kl, &ku, &alpha, a, &lda, x, &incx, &beta, y,
           &incy);
  }
  static void symv(const char uplo, const int n, const double alpha,
                   const double *a, const int lda, const double *x,
                   const int incx, const double beta, double *y,
                   const int incy) {
    dsymv_(&uplo, &n, &alpha, a, &lda, x, &incx, &beta, y, &incy);
  }
  static void sbmv(const char uplo, const int n, const int k,
                   const double alpha, const double *a, const int lda,
                   const int *x, const int incx, const double beta, double *y,
                   const int incy) {
    dsbmv_(&uplo, &n, &k, &alpha, a, &lda, x, &incx, &beta, y, &incy);
  }
  static void spmv(const char uplo, const int n, const double alpha,
                   const double *a, const double *x, const int incx,
                   const double beta, double *y, const int incy) {
    dspmv_(&uplo, &n, &alpha, a, x, &incx, &beta, y, &incy);
  }
  static void trmv(const char uplo, const char trans, const char diag,
                   const int n, const double *a, const int lda, double *x,
                   const int incx) {
    dtrmv_(&uplo, &trans, &diag, &n, a, &lda, x, &incx);
  }
  static void tbmv(const char uplo, const char trans, const char diag,
                   const int n, const int k, const double *a, const int lda,
                   double *x, const int incx) {
    dtbmv_(&uplo, &trans, &diag, &n, &k, a, &lda, x, &incx);
  }
  static void tpmv(const char uplo, const char trans, const char diag,
                   const int n, const double *ap, double *x, const int incx) {
    dtpmv_(&uplo, &trans, &diag, &n, ap, x, &incx);
  }
  static void trsv(const char uplo, const char trans, const char diag,
                   const int n, const double *a, const int lda, double *x,
                   const int incx) {
    dtrsv_(&uplo, &trans, &diag, &n, a, &lda, x, &incx);
  }
  static void tbsv(const char uplo, const char trans, const char diag,
                   const int n, const int k, const double *a, const int lda,
                   double *x, const int incx) {
    dtbsv_(&uplo, &trans, &diag, &n, &k, a, &lda, x, &incx);
  }
  static void tpsv(const char uplo, const char trans, const char diag,
                   const int n, const double *ap, double *x, const int incx) {
    dtpsv_(&uplo, &trans, &diag, &n, ap, x, &incx);
  }
  static void ger(const int m, const int n, const double alpha, const double *x,
                  const int incx, const double *y, const int incy, double *a,
                  const int lda) {
    dger_(&m, &n, &alpha, x, &incx, y, &incy, a, &lda);
  }
  static void syr(const char uplo, const int n, const double alpha,
                  const double *x, const int incx, double *a, const int lda) {
    dsyr_(&uplo, &n, &alpha, x, &incx, a, &lda);
  }
  static void spr(const char uplo, const int n, const double alpha,
                  const double *x, const int incx, double *ap) {
    dspr_(&uplo, &n, &alpha, x, &incx, ap);
  }
  static void syr2(const char uplo, const int n, const double alpha,
                   const double *x, const int incx, const double *y,
                   const int incy, double *a, const int lda) {
    dsyr2_(&uplo, &n, &alpha, x, &incx, y, &incy, a, &lda);
  }
  static void spr2(const char uplo, const int n, const double alpha,
                   const double *x, const int incx, const double *y,
                   const int incy, double *ap) {
    dspr2_(&uplo, &n, &alpha, x, &incx, y, &incy, ap);
  }
  // Level 3
  static void gemm(const char transa, const char transb, const int m,
                   const int n, const int k, const double alpha,
                   const double *a, const int lda, const double *b,
                   const int ldb, const double beta, double *c, const int ldc) {
    dgemm_(&transa, &transb, &m, &n, &k, &alpha, a, &lda, b, &ldb, &beta, c,
           &ldc);
  }
  static void symm(const char side, const char uplo, const int m, const int n,
                   const double alpha, const double *a, const int lda,
                   const double *b, const int ldb, const double beta, double *c,
                   const int ldc) {
    dsymm_(&side, &uplo, &m, &n, &alpha, a, &lda, b, &ldb, &beta, c, &ldc);
  }
  static void syrk(const char uplo, const char trans, const int n, const int k,
                   const double alpha, const double *a, const int lda,
                   const double beta, double *c, const int ldc) {
    dsyrk_(&uplo, &trans, &n, &k, &alpha, a, &lda, &beta, c, &ldc);
  }
  static void syr2k(const char uplo, const char trans, const int n, const int k,
                    const double alpha, const double *a, const int lda,
                    const double *b, const int ldb, const double beta,
                    double *c, const int ldc) {
    dsyr2k_(&uplo, &trans, &n, &k, &alpha, a, &lda, b, &ldb, &beta, c, &ldc);
  }
  static void trmm(const char side, const char uplo, const char transa,
                   const char diag, const int m, const int n,
                   const double alpha, const double *a, const int lda,
                   double *b, const int ldb) {
    dtrmm_(&side, &uplo, &transa, &diag, &m, &n, &alpha, a, &lda, b, &ldb);
  }
  static void trsm(const char side, const char uplo, const char transa,
                   const char diag, const int m, const int n,
                   const double alpha, const double *a, const int lda,
                   double *b, const int ldb) {
    dtrsm_(&side, &uplo, &transa, &diag, &m, &n, &alpha, a, &lda, b, &ldb);
  }
};

} // namespace utility
} // namespace pbopt

#endif
