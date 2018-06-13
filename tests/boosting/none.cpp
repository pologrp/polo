#include <algorithm>
#include <cstddef>
#include <iterator>
#include <random>
#include <vector>

#include "gtest/gtest.h"

#include "polo/boosting/none.hpp"

class BoostingNone : public polo::boosting::none<double, int>,
                     public ::testing::Test {
protected:
  BoostingNone()
      : x(3), g(3), gnew(3), gen{std::random_device{}()},
        dist1(1, std::numeric_limits<int>::max()) {}
  void SetUp() override {
    polo::boosting::none<double, int>::initialize(std::begin(x), std::end(x));
  }
  void TearDown() override {}
  ~BoostingNone() override = default;

  std::vector<double> x, g, gnew;
  std::mt19937 gen;
  std::uniform_int_distribution<int> dist1;
  std::normal_distribution<double> dist2;
};

TEST_F(BoostingNone, ReceiveGradient) {
  const double *g_b = g.data();
  const double *g_e = g_b + g.size();
  double *gcurr = gnew.data();

  const int widx = dist1(gen);
  const int klocal = dist1(gen);
  const int kglobal = dist1(gen);
  std::generate(std::begin(g), std::end(g), [&]() { return dist2(gen); });

  boost(widx, klocal, kglobal, g_b, g_e, gcurr);
  for (std::size_t idx = 0; idx < g.size(); idx++)
    EXPECT_DOUBLE_EQ(g[idx], gnew[idx]);
}
