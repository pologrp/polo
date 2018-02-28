#ifndef LAPACK_HPP_
#define LAPACK_HPP_

extern "C" {
// Single Precision
void spotrf_(const char *, const int *, float *, const int *, int *);
void spotrs_(const char *, const int *, const int *, const float *, const int *,
             float *, const int *, int *);
void spptrf_(const char *, const int *, float *, int *);
void spptrs_(const char *, const int *, const int *, const float *, float *,
             const int *, int *);
// Double Precision
void dpotrf_(const char *, const int *, double *, const int *, int *);
void dpotrs_(const char *, const int *, const int *, const double *,
             const int *, double *, const int *, int *);
void dpptrf_(const char *, const int *, double *, int *);
void dpptrs_(const char *, const int *, const int *, const double *, double *,
             const int *, int *);
}

namespace polo {
namespace utility {
namespace matrix {

template <class float_t> struct lapack;

template <> struct lapack<float> {
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
};

template <> struct lapack<double> {
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
};

} // namespace matrix
} // namespace utility
} // namespace polo

#endif
