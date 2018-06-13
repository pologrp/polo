#include <cstddef>
#include <iterator>
#include <random>
#include <vector>

#include "gtest/gtest.h"

#include "polo/boosting/aggregated.hpp"

class BoostingAggregated : public polo::boosting::aggregated<double, int>,
                           public ::testing::Test {
protected:
  BoostingAggregated()
      : x{1, 2, 3}, g1{1, 2, 3}, g2{2, 4, 6}, gnew{0, 0, 0},
        gen{std::random_device{}()}, dist(1, std::numeric_limits<int>::max()) {}
  void SetUp() override {
    polo::boosting::aggregated<double, int>::initialize(std::begin(x),
                                                        std::end(x));
  }
  void TearDown() override {}
  ~BoostingAggregated() override = default;

  const std::vector<double> x, g1, g2;
  std::vector<double> gnew;
  std::mt19937 gen;
  std::uniform_int_distribution<int> dist;
};

TEST_F(BoostingAggregated, ReceiveGradient) {
  const double *g1_b = g1.data();
  const double *g1_e = g1_b + g1.size();
  const double *g2_b = g2.data();
  const double *g2_e = g2_b + g2.size();
  double *gcurr = gnew.data();

  int klocal = dist(gen);
  int kglobal = dist(gen);
  boost(0, klocal, kglobal, g1_b, g1_e, gcurr);
  for (std::size_t idx = 0; idx < g1.size(); idx++)
    EXPECT_DOUBLE_EQ(g1[idx], gnew[idx]);

  klocal = dist(gen);
  kglobal = dist(gen);
  boost(1, klocal, kglobal, g2_b, g2_e, gcurr);
  for (std::size_t idx = 0; idx < g1.size(); idx++)
    EXPECT_DOUBLE_EQ(g1[idx] + g2[idx], gnew[idx]);
}

TEST_F(BoostingAggregated, UpdateGradient) {
  const double *g1_b = g1.data();
  const double *g1_e = g1_b + g1.size();
  const double *g2_b = g2.data();
  const double *g2_e = g2_b + g2.size();
  double *gcurr = gnew.data();

  int klocal = dist(gen);
  int kglobal = dist(gen);
  boost(0, klocal, kglobal, g1_b, g1_e, gcurr);

  klocal = dist(gen);
  kglobal = dist(gen);
  boost(1, klocal, kglobal, g2_b, g2_e, gcurr);

  klocal = dist(gen);
  kglobal = dist(gen);
  boost(0, klocal, kglobal, g2_b, g2_e, gcurr);
  for (std::size_t idx = 0; idx < g1.size(); idx++)
    EXPECT_DOUBLE_EQ(2 * g2[idx], gnew[idx]);

  klocal = dist(gen);
  kglobal = dist(gen);
  boost(1, klocal, kglobal, g1_b, g1_e, gcurr);
  for (std::size_t idx = 0; idx < g1.size(); idx++)
    EXPECT_DOUBLE_EQ(g1[idx] + g2[idx], gnew[idx]);
}
