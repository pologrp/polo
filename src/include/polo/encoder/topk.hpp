#ifndef POLO_ENCODER_TOPK_HPP_
#define POLO_ENCODER_TOPK_HPP_

#include <algorithm>
#include <cmath>
#include <queue>
#include <utility>
#include <vector>

#include "cereal/types/vector.hpp"

namespace polo {
namespace encoder {
template <class value_t, class index_t> class topk {
  struct result_t {
    result_t() = default;
    result_t(std::vector<index_t> nzind, std::vector<value_t> nzval)
        : nzind(std::move(nzind)), nzval(std::move(nzval)) {}

    size_t size() const noexcept {
      return sizeof(index_t) * nzind.size() + sizeof(value_t) * nzval.size();
    }

    template <class Archive> void serialize(Archive &archive) {
      archive(nzind, nzval);
    }

    result_t slice(const index_t ib, const index_t ie) const {
      std::vector<index_t> newind;
      std::vector<value_t> newval;

      if (!nzind.empty() && ib <= nzind.back()) {
        index_t k{0};
        for (const index_t idx : nzind) {
          if ((idx >= ib) & (idx < ie)) {
            newind.push_back(idx);
            newval.push_back(nzval[k]);
          } else if (idx >= ie)
            break;
          k++;
        }
      }

      return result_t(std::move(newind), std::move(newval));
    }

    template <class OutputIt>
    OutputIt operator()(OutputIt xb, OutputIt xe, const index_t ib = 0) const {
      std::fill(xb, xe, 0);

      index_t k{0};
      for (const index_t idx : nzind)
        *(xb + idx - ib) = nzval[k++];

      return xe;
    }

  private:
    std::vector<index_t> nzind;
    std::vector<value_t> nzval;
  };

  index_t K;

public:
  using result_type = result_t;

  topk(const index_t K) : K{K} {}

  template <class RandomIt>
  result_type operator()(RandomIt xb, RandomIt xe) const {
    const auto cmp = [xb](const index_t left, const index_t right) {
      return std::abs(*(xb + left)) > std::abs(*(xb + right));
    };
    std::priority_queue<index_t, std::vector<index_t>, decltype(cmp)> heap(cmp);

    index_t k{0};
    RandomIt xtemp{xb};
    while (xtemp != xe) {
      const value_t val = *xtemp++;
      if (heap.size() < size_t(K))
        heap.push(k);
      else if (std::abs(val) > std::abs(*(xb + heap.top()))) {
        heap.pop();
        heap.push(k);
      }
      k++;
    }

    const size_t len = heap.size();
    std::vector<index_t> nzind(len);
    std::vector<value_t> nzval(len);

    std::generate(std::begin(nzind), std::end(nzind), [&heap]() {
      const index_t idx = heap.top();
      heap.pop();
      return idx;
    });
    std::sort(std::begin(nzind), std::end(nzind));

    k = 0;
    for (const index_t idx : nzind)
      nzval[k++] = *(xb + idx);

    return result_type(std::move(nzind), std::move(nzval));
  }

  template <class RandomIt, class ForwardIt>
  result_type operator()(RandomIt xb, RandomIt xe, ForwardIt ib,
                         ForwardIt ie) const {
    const auto cmp = [xb](const index_t left, const index_t right) {
      return std::abs(*(xb + left)) > std::abs(*(xb + right));
    };
    std::priority_queue<index_t, std::vector<index_t>, decltype(cmp)> heap(cmp);

    ForwardIt itemp{ib};
    while (itemp != ie) {
      const index_t idx = *itemp++;
      const value_t val = *(xb + idx);
      if (heap.size() < size_t(K))
        heap.push(idx);
      else if (std::abs(val) > std::abs(*(xb + heap.top()))) {
        heap.pop();
        heap.push(idx);
      }
    }

    const size_t len = heap.size();
    std::vector<index_t> nzind(len);
    std::vector<value_t> nzval(len);

    std::generate(std::begin(nzind), std::end(nzind), [&heap]() {
      const index_t idx = heap.top();
      heap.pop();
      return idx;
    });
    std::sort(std::begin(nzind), std::end(nzind));

    index_t k{0};
    for (const index_t idx : nzind)
      nzval[k++] = *(xb + idx);

    return result_type(std::move(nzind), std::move(nzval));
  }
};
} // namespace encoder
} // namespace polo

#endif
