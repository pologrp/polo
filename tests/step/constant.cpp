#include <algorithm>
#include <cstddef>
#include <iterator>
#include <random>
#include <vector>

#include "gtest/gtest.h"

#include "polo/step/constant.hpp"

class StepConstant : public polo::step::constant<double, int>,
                     public ::testing::Test {
protected:
  StepConstant()
      : x(3), g(3), gen{std::random_device{}()},
        dist1(1, std::numeric_limits<int>::max()) {}
  void SetUp() override {
    polo::step::constant<double, int>::initialize(std::begin(x), std::end(x));
  }
  void TearDown() override {}
  ~StepConstant() override = default;

  std::vector<double> x, g;
  std::mt19937 gen;
  std::uniform_int_distribution<int> dist1;
  std::normal_distribution<double> dist2;
};

TEST_F(StepConstant, Step) {
  const double *x_b = x.data();
  const double *x_e = x_b + x.size();
  const double *g_b = g.data();

  for (const auto stepsize : {1, 2, 3}) {
    parameters(stepsize);

    const int klocal = dist1(gen);
    const int kglobal = dist1(gen);
    const double fval = dist2(gen);
    std::generate(std::begin(x), std::end(x), [&]() { return dist2(gen); });
    std::generate(std::begin(g), std::end(g), [&]() { return dist2(gen); });

    EXPECT_DOUBLE_EQ(step(klocal, kglobal, fval, x_b, x_e, g_b), stepsize);
  }
}
