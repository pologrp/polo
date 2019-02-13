#ifndef POLO_ENCODER_TERNARY_HPP_
#define POLO_ENCODER_TERNARY_HPP_

#include <algorithm>
#include <climits>
#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>

#include "cereal/types/vector.hpp"

namespace polo {
namespace encoder {
template <class value_t, class index_t, class bit_t = uint8_t> class ternary {
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

      index_t k{0}, newk{0};

      if (nzind.empty()) {
        for (k = ib; k < ie; k++) {
          newind.push_back(k);
          const int val = (signs[k / N] >> (2 * (k % N))) & 3;
          if (newk % N == 0)
            newsigns.push_back(0);
          newsigns[newk / N] |= (val << (2 * (newk % N)));
          newk++;
        }
      } else if (ib <= nzind.back()) {
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
      const size_t nbits = CHAR_BIT * sizeof(bit_t);
      const size_t N = nbits / 2;

      index_t k{0};
      if (nzind.empty()) {
        while (xb != xe) {
          const int val = (signs[k / N] >> (2 * (k % N))) & 3;
          *xb++ = (val == 1) ? -norm : (val == 2) ? norm : 0;
          k++;
        }
      } else {
        std::fill(xb, xe, 0);
        for (const index_t idx : nzind) {
          const int val = (signs[k / N] >> (2 * (k % N))) & 3;
          *(xb + idx - ib) = (val == 1) ? -norm : (val == 2) ? norm : 0;
          k++;
        }
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
    const size_t nbits = CHAR_BIT * sizeof(bit_t);
    const size_t N = nbits / 2;
    const size_t d = std::distance(xb, xe);
    const size_t split = d / N;
    const size_t remain = d % N;

    index_t k{0};
    value_t norm{0};
    std::vector<bit_t> signs(remain == 0 ? split : split + 1, 0);

    while (xb != xe) {
      const value_t val = *xb++;
      norm += val * val;
      signs[k / N] |= (val < 0) ? (1 << (2 * (k % N)))
                                : (val > 0) ? (2 << (2 * (k % N))) : 0;
      k++;
    }

    return result_type(std::sqrt(norm), std::vector<index_t>{},
                       std::move(signs));
  }

  template <class RandomIt, class ForwardIt>
  result_type operator()(RandomIt xb, RandomIt xe, ForwardIt ib,
                         ForwardIt ie) const {
    const size_t nbits = CHAR_BIT * sizeof(bit_t);
    const size_t N = nbits / 2;
    const size_t d = std::distance(ib, ie);
    const size_t split = d / N;
    const size_t remain = d % N;

    index_t k{0};
    value_t norm{0};
    std::vector<index_t> nzind;
    std::vector<bit_t> signs(remain == 0 ? split : split + 1, 0);

    while (ib != ie) {
      const index_t idx = *ib++;
      nzind.push_back(idx);
      const value_t val = *(xb + idx);
      norm += val * val;
      signs[k / N] |= (val < 0) ? (1 << (2 * (k % N)))
                                : (val > 0) ? (2 << (2 * (k % N))) : 0;
      k++;
    }

    return result_type(std::sqrt(norm), std::move(nzind), std::move(signs));
  }
};
} // namespace encoder
} // namespace polo

#endif
