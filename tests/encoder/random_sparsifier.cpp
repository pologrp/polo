#include <iterator>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "cereal/archives/portable_binary.hpp"
#include "polo/encoder/random_sparsifier.hpp"
#include "gtest/gtest.h"

class EncoderRandomS : public polo::encoder::random_sparsifier<double, int>,
                       public ::testing::Test {
protected:
  EncoderRandomS()
      : polo::encoder::random_sparsifier<double, int>(std::vector<double>{
            0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0}),
        x{-5, 1, 12, -7, 0, 0, -100, 500, 6, -30} {}
  void SetUp() override {}
  void TearDown() override {}
  ~EncoderRandomS() override = default;

  const std::vector<double> x;
};

TEST_F(EncoderRandomS, FullGradient) {
  const std::vector<double> probs{0.1, 0.2, 0.3, 0.4, 0.5,
                                  0.6, 0.7, 0.8, 0.9, 1.0};
  std::vector<double> actual(x.size());
  const auto v = operator()(std::begin(x), std::end(x));
  v(std::begin(actual), std::end(actual));

  std::mt19937 gen;
  std::bernoulli_distribution dist;
  std::vector<double> expected(x.size());
  int k{0};
  for (const double val : x) {
    const double prob = probs[k];
    dist.param(std::bernoulli_distribution::param_type{prob});
    if (dist(gen))
      expected[k] = val / prob;
    k++;
  }

  for (size_t idx = 0; idx < x.size(); idx++)
    EXPECT_DOUBLE_EQ(actual[idx], expected[idx]);
}

TEST_F(EncoderRandomS, BlockCoordinate) {
  const std::vector<double> probs{0.1, 0.2, 0.3, 0.4, 0.5,
                                  0.6, 0.7, 0.8, 0.9, 1.0};
  std::vector<double> actual(x.size());
  const std::vector<int> block{0, 1, 6, 9};
  const auto v = operator()(std::begin(x), std::end(x), std::begin(block),
                            std::end(block));
  v(std::begin(actual), std::end(actual));

  std::mt19937 gen;
  std::bernoulli_distribution dist(0.8);
  std::vector<double> expected(x.size());
  for (const int idx : block) {
    const double prob = probs[idx];
    dist.param(std::bernoulli_distribution::param_type{prob});
    if (dist(gen))
      expected[idx] = x[idx] / prob;
  }

  for (size_t idx = 0; idx < x.size(); idx++)
    EXPECT_DOUBLE_EQ(actual[idx], expected[idx]);
}

TEST_F(EncoderRandomS, Serialization) {
  std::vector<double> res1(x.size()), res2(x.size());
  const auto v1 = operator()(std::begin(x), std::end(x));
  v1(std::begin(res1), std::end(res1));

  std::ostringstream oss{std::ios_base::binary};
  {
    cereal::PortableBinaryOutputArchive out{oss};
    out(v1);
  }
  std::string serializeddata{oss.str()};

  result_type v2;
  std::istringstream iss{serializeddata, std::ios_base::binary};
  {
    cereal::PortableBinaryInputArchive in{iss};
    in(v2);
  }
  v2(std::begin(res2), std::end(res2));

  for (size_t idx = 0; idx < x.size(); idx++)
    EXPECT_DOUBLE_EQ(res1[idx], res2[idx]);
}

TEST_F(EncoderRandomS, Slicing) {
  std::vector<double> expected(x.size());
  const auto v1 = operator()(std::begin(x), std::end(x));
  v1(std::begin(expected), std::end(expected));

  const size_t W{4};
  const size_t split = x.size() / W;
  const size_t remain = x.size() % W;
  int indstart{0}, indend;
  for (size_t w = 0; w < W; w++) {
    const size_t ndata = (w < remain) ? split + 1 : split;
    indend = indstart + ndata;
    std::vector<double> actual(ndata);

    const auto v = v1.slice(indstart, indend);
    v(std::begin(actual), std::end(actual), indstart);

    for (size_t idx = 0; idx < actual.size(); idx++)
      EXPECT_DOUBLE_EQ(actual[idx], expected[idx + indstart]);

    indstart = indend;
  }
}
