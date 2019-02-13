#ifndef POLO_ENCODER_RANDOM_SPARSIFIER_HPP_
#define POLO_ENCODER_RANDOM_SPARSIFIER_HPP_

#include <algorithm>
#include <random>
#include <utility>
#include <vector>

#include "cereal/types/vector.hpp"

namespace polo {
namespace encoder {
template <class value_t, class index_t, class generator_t = std::mt19937>
class random_sparsifier {
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

  std::vector<value_t> probs;
  generator_t gen;
  std::bernoulli_distribution dist;

public:
  using result_type = result_t;

  random_sparsifier(const size_t d, const value_t val,
                    generator_t gen = generator_t{})
      : random_sparsifier(std::vector<value_t>(d, val), std::move(gen)) {}
  random_sparsifier(std::vector<value_t> probs, generator_t gen = generator_t{})
      : probs(std::move(probs)), gen(std::move(gen)) {}
  random_sparsifier(const random_sparsifier &rhs)
      : probs(rhs.probs), gen(std::random_device{}()), dist(rhs.dist) {}
  random_sparsifier &operator=(const random_sparsifier &rhs) {
    probs = rhs.probs;
    gen = generator_t(std::random_device{}());
    dist = rhs.dist;
    return *this;
  }
  random_sparsifier(random_sparsifier &&) = default;
  random_sparsifier &operator=(random_sparsifier &&) = default;

  template <class RandomIt> result_type operator()(RandomIt xb, RandomIt xe) {
    std::vector<index_t> nzind;
    std::vector<value_t> nzval;
    index_t idx{0};
    while (xb != xe) {
      const value_t val = *xb++;
      const value_t prob = probs[idx];
      dist.param(std::bernoulli_distribution::param_type{prob});
      if (dist(gen)) {
        nzind.push_back(idx);
        nzval.push_back(val / prob);
      }
      idx++;
    }
    return result_type(std::move(nzind), std::move(nzval));
  }

  template <class RandomIt, class ForwardIt>
  result_type operator()(RandomIt xb, RandomIt xe, ForwardIt ib, ForwardIt ie) {
    std::vector<index_t> nzind;
    std::vector<value_t> nzval;
    while (ib != ie) {
      const index_t idx = *ib++;
      const value_t val = *(xb + idx);
      const value_t prob = probs[idx];
      dist.param(std::bernoulli_distribution::param_type{prob});
      if (dist(gen)) {
        nzind.push_back(idx);
        nzval.push_back(val / prob);
      }
    }
    return result_type(std::move(nzind), std::move(nzval));
  }
};
} // namespace encoder
} // namespace polo

#endif
