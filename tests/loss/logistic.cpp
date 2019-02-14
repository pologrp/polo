#include <vector>

#include "polo/loss/logistic.hpp"
#include "gtest/gtest.h"

class Logistic : public polo::loss::logistic<double, int>,
                 public ::testing::Test {
protected:
  Logistic() = default;
  void SetUp() override {}
  void TearDown() override {}
  ~Logistic() override = default;
};

TEST_F(Logistic, Full) {
  const int nrows{3};
  const int ncols{3};
  const std::vector<double> x{8, 9, 10};
  const std::vector<double> b{-1, 1, -1};

  const std::vector<double> g_expected{
      0.9999999999993086, -5.638586449606651e-12, 5.999999999998617};
  const double fval_expected{68.00000000000257};

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

TEST_F(Logistic, Incremental) {
  const int nrows{3};
  const int ncols{3};
  const std::vector<double> x{8, 9, 10};
  const std::vector<double> b{-1, 1, -1};

  const std::vector<int> indices{0, 1};
  const std::vector<double> g_expected{
      0.9999999999993086, -5.638586449606651e-12, 1.999999999998617};
  const double fval_expected{28.000000000002572};

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
