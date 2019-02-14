#include <vector>

#include "polo/loss/leastsquares.hpp"
#include "gtest/gtest.h"

class LeastSquares : public polo::loss::leastsquares<double, int>,
                     public ::testing::Test {
protected:
  LeastSquares() = default;
  void SetUp() override {}
  void TearDown() override {}
  ~LeastSquares() override = default;
};

TEST_F(LeastSquares, Full) {
  const int nrows{3};
  const int ncols{3};
  const std::vector<double> x{8, 9, 10};
  const std::vector<double> b{5, 6, 7};

  const std::vector<double> g_expected{23, 63, 178};
  const double fval_expected{1029.5};

  polo::matrix::dmatrix<double, int> dmat(nrows, ncols,
                                          {1, 0, 0, 0, 3, 0, 2, 0, 4});
  polo::loss::data<double, int> data_(dmat, b);
  data(data_);

  std::vector<double> g(x.size());

  double fval = operator()(x.data(), g.data());

  EXPECT_DOUBLE_EQ(fval, fval_expected);
  for (int idx = 0; idx < 3; idx++)
    EXPECT_DOUBLE_EQ(g[idx], g_expected[idx]);

  for (double &val : g)
    val = 0;

  const std::vector<int> row_ptr{0, 2, 3, 4};
  const std::vector<int> cols{0, 2, 1, 2};
  const std::vector<double> nzvals{1, 2, 3, 4};
  polo::matrix::smatrix<double, int> smat(nrows, ncols, row_ptr, cols, nzvals);
  data_ = polo::loss::data<double, int>(smat, b);
  data(data_);

  fval = operator()(x.data(), g.data());

  EXPECT_DOUBLE_EQ(fval, fval_expected);
  for (int idx = 0; idx < 3; idx++)
    EXPECT_DOUBLE_EQ(g[idx], g_expected[idx]);
}

TEST_F(LeastSquares, Incremental) {
  const int nrows{3};
  const int ncols{3};
  const std::vector<double> x{8, 9, 10};
  const std::vector<double> b{5, 6, 7};

  const std::vector<int> indices{0, 1};
  const std::vector<double> g_expected{23, 63, 46};
  const double fval_expected{485};

  polo::matrix::dmatrix<double, int> dmat(nrows, ncols,
                                          {1, 0, 0, 0, 3, 0, 2, 0, 4});
  polo::loss::data<double, int> data_(dmat, b);
  data(data_);

  std::vector<double> g(x.size());

  double fval = operator()(x.data(), g.data(), indices.data(),
                           indices.data() + indices.size());

  EXPECT_DOUBLE_EQ(fval, fval_expected);
  for (int idx = 0; idx < 3; idx++)
    EXPECT_DOUBLE_EQ(g[idx], g_expected[idx]);

  for (double &val : g)
    val = 0;

  const std::vector<int> row_ptr{0, 2, 3, 4};
  const std::vector<int> cols{0, 2, 1, 2};
  const std::vector<double> nzvals{1, 2, 3, 4};
  polo::matrix::smatrix<double, int> smat(nrows, ncols, row_ptr, cols, nzvals);
  data_ = polo::loss::data<double, int>(smat, b);
  data(data_);

  fval = operator()(x.data(), g.data(), indices.data(),
                    indices.data() + indices.size());

  EXPECT_DOUBLE_EQ(fval, fval_expected);
  for (int idx = 0; idx < 3; idx++)
    EXPECT_DOUBLE_EQ(g[idx], g_expected[idx]);
}
