#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "cereal/archives/portable_binary.hpp"
#include "polo/encoder/identity.hpp"
#include "gtest/gtest.h"

class EncoderIdentity : public polo::encoder::identity<double, int>,
                        public ::testing::Test {
protected:
  EncoderIdentity() : x{-5, 1, 12, -7, 0, 0, -100, 500, 6, -30} {}
  void SetUp() override {}
  void TearDown() override {}
  ~EncoderIdentity() override = default;

  const std::vector<double> x;
};

TEST_F(EncoderIdentity, FullGradient) {
  std::vector<double> actual(x.size());
  const auto v = operator()(x.data(), x.data() + x.size());
  v(std::begin(actual), std::end(actual));

  const std::vector<double> expected{-5, 1, 12, -7, 0, 0, -100, 500, 6, -30};

  for (size_t idx = 0; idx < x.size(); idx++)
    EXPECT_DOUBLE_EQ(actual[idx], expected[idx]);
}

TEST_F(EncoderIdentity, BlockCoordinate) {
  std::vector<double> actual(x.size());
  const std::vector<int> block{0, 1, 6, 9};
  const auto v = operator()(x.data(), x.data() + x.size(), block.data(),
                            block.data() + block.size());
  v(std::begin(actual), std::end(actual));

  const std::vector<double> expected{-5, 1, 0, 0, 0, 0, -100, 0, 0, -30};

  for (size_t idx = 0; idx < x.size(); idx++)
    EXPECT_DOUBLE_EQ(actual[idx], expected[idx]);
}

TEST_F(EncoderIdentity, Serialization) {
  std::vector<double> res1(x.size()), res2(x.size());
  const auto v1 = operator()(x.data(), x.data() + x.size());
  v1(std::begin(res1), std::end(res1));

  std::ostringstream oss1{std::ios_base::binary};
  {
    cereal::PortableBinaryOutputArchive out{oss1};
    out(v1);
  }
  std::string serializeddata{oss1.str()};

  result_type v2;
  std::istringstream iss1{serializeddata, std::ios_base::binary};
  {
    cereal::PortableBinaryInputArchive in{iss1};
    in(v2);
  }
  v2(std::begin(res2), std::end(res2));

  for (size_t idx = 0; idx < x.size(); idx++)
    EXPECT_DOUBLE_EQ(res1[idx], res2[idx]);

  const std::vector<int> block{0, 1, 6, 9};
  std::vector<double> res3(x.size()), res4(x.size());
  const auto v3 = operator()(x.data(), x.data() + x.size(), block.data(),
                             block.data() + block.size());
  v3(std::begin(res3), std::end(res3));

  std::ostringstream oss2{std::ios_base::binary};
  {
    cereal::PortableBinaryOutputArchive out{oss2};
    out(v3);
  }
  serializeddata = oss2.str();

  result_type v4;
  std::istringstream iss2{serializeddata, std::ios_base::binary};
  {
    cereal::PortableBinaryInputArchive in{iss2};
    in(v4);
  }
  v4(std::begin(res4), std::end(res4));

  for (size_t idx = 0; idx < x.size(); idx++)
    EXPECT_DOUBLE_EQ(res3[idx], res4[idx]);
}

TEST_F(EncoderIdentity, Slicing) {
  std::vector<double> expected(x.size());
  const auto v1 = operator()(x.data(), x.data() + x.size());
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
