#ifndef POLO_ENCODER_DYNAMIC_HPP_
#define POLO_ENCODER_DYNAMIC_HPP_

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <queue>
#include <utility>
#include <vector>

#include "cereal/types/vector.hpp"

namespace polo {
namespace encoder {
template <class value_t, class index_t, class bit_t = uint8_t> class dynamic {
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
        *(xb + idx - ib) = (val == 1) ? -norm : norm;
        k++;
      }

      return xe;
    }

  private:
    value_t norm;
    std::vector<index_t> nzind;
    std::vector<bit_t> signs;
  };

public:
  using result_type = result_t;

  template <class RandomIt>
  result_type operator()(RandomIt xb, RandomIt xe) const {
    const auto cmp = [xb](const index_t left, const index_t right) {
      return std::abs(*(xb + left)) > std::abs(*(xb + right));
    };
    std::priority_queue<index_t, std::vector<index_t>, decltype(cmp)> heap(cmp);

    index_t k{0};
    const size_t K(std::ceil(std::sqrt(std::distance(xb, xe))));
    value_t norm{0};
    RandomIt xtemp{xb};
    while (xtemp != xe) {
      const value_t val = *xtemp++;
      if (heap.size() < K)
        heap.push(k);
      else if (std::abs(val) > std::abs(*(xb + heap.top()))) {
        heap.pop();
        heap.push(k);
      }
      norm += val * val;
      k++;
    }
    norm = std::sqrt(norm);

    std::vector<index_t> indices(K);
    std::generate(indices.rbegin(), indices.rend(), [&heap]() {
      const index_t idx = heap.top();
      heap.pop();
      return idx;
    });

    const size_t nbits = CHAR_BIT * sizeof(bit_t);
    const size_t N = nbits / 2;

    std::vector<index_t> nzind;
    std::vector<bit_t> signs;

    k = 0;
    value_t acc{0};
    while (acc < norm) {
      const index_t idx = indices[k];
      const value_t val = *(xb + idx);
      acc += std::abs(val);
      k++;
    }
    std::sort(std::begin(indices), std::begin(indices) + k);

    for (index_t curr = 0; curr < k; curr++) {
      const index_t idx = indices[curr];
      nzind.push_back(idx);
      const value_t val = *(xb + idx);
      if (curr % N == 0)
        signs.push_back(0);
      signs[curr / N] |=
          (val < 0) ? (1 << (2 * (curr % N))) : (2 << (2 * (curr % N)));
    }

    return result_type(norm, std::move(nzind), std::move(signs));
  }

  template <class RandomIt, class ForwardIt>
  result_type operator()(RandomIt xb, RandomIt xe, ForwardIt ib,
                         ForwardIt ie) const {
    const auto cmp = [xb](const index_t left, const index_t right) {
      return std::abs(*(xb + left)) > std::abs(*(xb + right));
    };
    std::priority_queue<index_t, std::vector<index_t>, decltype(cmp)> heap(cmp);

    const size_t K(std::ceil(std::sqrt(std::distance(ib, ie))));
    value_t norm{0};
    ForwardIt itemp{ib};
    while (itemp != ie) {
      const index_t idx = *itemp++;
      const value_t val = *(xb + idx);
      if (heap.size() < K)
        heap.push(idx);
      else if (std::abs(val) > std::abs(*(xb + heap.top()))) {
        heap.pop();
        heap.push(idx);
      }
      norm += val * val;
    }
    norm = std::sqrt(norm);

    std::vector<index_t> indices(K);
    std::generate(indices.rbegin(), indices.rend(), [&heap]() {
      const index_t idx = heap.top();
      heap.pop();
      return idx;
    });

    const size_t nbits = CHAR_BIT * sizeof(bit_t);
    const size_t N = nbits / 2;

    std::vector<index_t> nzind;
    std::vector<bit_t> signs;

    index_t k{0};
    value_t acc{0};
    while (acc < norm) {
      const index_t idx = indices[k];
      const value_t val = *(xb + idx);
      acc += std::abs(val);
      k++;
    }
    std::sort(std::begin(indices), std::begin(indices) + k);

    for (index_t curr = 0; curr < k; curr++) {
      const index_t idx = indices[curr];
      nzind.push_back(idx);
      const value_t val = *(xb + idx);
      if (curr % N == 0)
        signs.push_back(0);
      signs[curr / N] |=
          (val < 0) ? (1 << (2 * (curr % N))) : (2 << (2 * (curr % N)));
    }

    return result_type(norm, std::move(nzind), std::move(signs));
  }
};
} // namespace encoder
} // namespace polo

#endif
