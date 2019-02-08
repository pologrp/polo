#include <algorithm>
#include <iterator>
#include <vector>

#include "polo/utility/sampler.hpp"
#include "gtest/gtest.h"

class CustomSampler : public polo::utility::sampler::custom<int>,
                      public ::testing::Test {
protected:
  CustomSampler() : indices(1000) {}
  void SetUp() override {
    std::vector<double> probs(10000);
    for (int idx = 0; idx < 9000; idx++)
      probs[idx] = 1;
    parameters(std::begin(probs), std::end(probs));
  }
  void TearDown() override {}
  ~CustomSampler() override = default;

  std::vector<int> indices;
};

TEST_F(CustomSampler, Sorted) {
  operator()(std::begin(indices), std::end(indices));
  EXPECT_TRUE(std::is_sorted(std::begin(indices), std::end(indices)));
}

TEST_F(CustomSampler, Bounded) {
  for (int n = 0; n < 100; n++) {
    operator()(std::begin(indices), std::end(indices));
    EXPECT_GE(indices.front(), 0);
    EXPECT_LT(indices.back(), 9000);
  }
}
