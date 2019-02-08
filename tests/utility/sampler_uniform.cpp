#include <algorithm>
#include <iterator>
#include <vector>

#include "polo/utility/sampler.hpp"
#include "gtest/gtest.h"

class UniformSampler : public polo::utility::sampler::uniform<int>,
                       public ::testing::Test {
protected:
  UniformSampler() : indices(1000) {}
  void SetUp() override { parameters(0, 10000); }
  void TearDown() override {}
  ~UniformSampler() override = default;

  std::vector<int> indices;
};

TEST_F(UniformSampler, Sorted) {
  operator()(std::begin(indices), std::end(indices));
  EXPECT_TRUE(std::is_sorted(std::begin(indices), std::end(indices)));
}

TEST_F(UniformSampler, Bounded) {
  for (int n = 0; n < 100; n++) {
    operator()(std::begin(indices), std::end(indices));
    EXPECT_GE(indices.front(), 0);
    EXPECT_LE(indices.back(), 10000);
  }
}
