#ifndef POLO_ENCODER_RANDOM_QUANTIZER_HPP_
#define POLO_ENCODER_RANDOM_QUANTIZER_HPP_

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdint>
#include <random>
#include <utility>
#include <vector>

#include "cereal/types/vector.hpp"

namespace polo {
namespace encoder {
template <class value_t, class index_t, class bit_t = uint8_t,
          class generator_t = std::mt19937>
class random_quantizer {
  struct result_t {
    result_t() = default;
    result_t(const value_t norm, std::vector<index_t> nzind,
             std::vector<bit_t> signs)
        : norm(norm), nzind(std::move(nzind)), signs(std::move(signs)) {}

    size_t size() const noexcept {
      return sizeof(value_t) + sizeof(index_t) * nzind.size() +
             sizeof(bit_t) * signs.size();
    }

    template <class Archive> void serialize(Archive &archive) {
      archive(norm, nzind, signs);
    }

    result_t slice(const index_t ib, const index_t ie) const {
      const size_t nbits = CHAR_BIT * sizeof(bit_t);
      const size_t N = nbits / 2;

      std::vector<index_t> newind;
      std::vector<bit_t> newsigns;

      if (!nzind.empty() && ib <= nzind.back()) {
        index_t k{0}, newk{0};
        for (const index_t idx : nzind) {
          if ((idx >= ib) & (idx < ie)) {
            newind.push_back(idx);
            const int val = (signs[k / N] >> (2 * (k % N))) & 3;
            if (newk % N == 0)
              newsigns.push_back(0);
            newsigns[newk / N] |= (val << (2 * (newk % N)));
            newk++;
          } else if (idx >= ie)
            break;
          k++;
        }
      }

      return result_t(norm, std::move(newind), std::move(newsigns));
    }

    template <class OutputIt>
    OutputIt operator()(OutputIt xb, OutputIt xe, const index_t ib = 0) const {
      std::fill(xb, xe, 0);

      const size_t nbits = CHAR_BIT * sizeof(bit_t);
      const size_t N = nbits / 2;

      index_t k{0};
      for (const index_t idx : nzind) {
        const int val = (signs[k / N] >> (2 * (k % N))) & 3;
        *(xb + idx - ib) = (val == 1) ? -norm : (val == 2) ? norm : 0;
        k++;
      }

      return xe;
    }

  private:
    value_t norm;
    std::vector<index_t> nzind;
    std::vector<bit_t> signs;
  };

  generator_t gen;
  std::bernoulli_distribution dist;

public:
  using result_type = result_t;

  random_quantizer(generator_t gen = generator_t{}) : gen(std::move(gen)) {}
  random_quantizer(const random_quantizer &rhs)
      : gen(std::random_device{}()), dist(rhs.dist) {}
  random_quantizer &operator=(const random_quantizer &rhs) {
    gen = generator_t(std::random_device{}());
    dist = rhs.dist;
    return *this;
  }
  random_quantizer(random_quantizer &&) = default;
  random_quantizer &operator=(random_quantizer &&) = default;

  template <class RandomIt> result_type operator()(RandomIt xb, RandomIt xe) {
    const size_t nbits = CHAR_BIT * sizeof(bit_t);
    const size_t N = nbits / 2;

    value_t norm = std::accumulate(
        xb, xe, value_t{0},
        [](const value_t norm, const value_t val) { return norm + val * val; });
    norm = std::sqrt(norm);

    std::vector<index_t> nzind;
    std::vector<bit_t> signs;

    index_t idx{0}, k{0};
    while (xb != xe) {
      const value_t val = *xb++;

      const value_t prob = std::abs(val) / norm;
      dist.param(std::bernoulli_distribution::param_type{prob});

      if (dist(gen)) {
        nzind.push_back(idx);
        if (k % N == 0)
          signs.push_back(0);
        signs[k / N] |= (val < 0) ? (1 << (2 * (k % N)))
                                  : (val > 0) ? (2 << (2 * (k % N))) : 0;
        k++;
      }

      idx++;
    }

    return result_type(norm, std::move(nzind), std::move(signs));
  }

  template <class RandomIt, class ForwardIt>
  result_type operator()(RandomIt xb, RandomIt xe, ForwardIt ib, ForwardIt ie) {
    const size_t nbits = CHAR_BIT * sizeof(bit_t);
    const size_t N = nbits / 2;

    value_t norm = std::accumulate(ib, ie, value_t{0},
                                   [xb](const value_t norm, const index_t idx) {
                                     const value_t val = *(xb + idx);
                                     return norm + val * val;
                                   });
    norm = std::sqrt(norm);

    std::vector<index_t> nzind;
    std::vector<bit_t> signs;

    index_t k{0};
    while (ib != ie) {
      const index_t idx = *ib++;
      const value_t val = *(xb + idx);

      const value_t prob = std::abs(val) / norm;
      dist.param(std::bernoulli_distribution::param_type{prob});

      if (dist(gen)) {
        nzind.push_back(idx);
        if (k % N == 0)
          signs.push_back(0);
        signs[k / N] |= (val < 0) ? (1 << (2 * (k % N)))
                                  : (val > 0) ? (2 << (2 * (k % N))) : 0;
        k++;
      }
    }

    return result_type(norm, std::move(nzind), std::move(signs));
  }
};
} // namespace encoder
} // namespace polo

#endif
