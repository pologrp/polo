#ifndef POLO_ENCODER_IDENTITY_HPP_
#define POLO_ENCODER_IDENTITY_HPP_

#include <algorithm>
#include <iterator>
#include <vector>

#include "cereal/types/vector.hpp"

namespace polo {
namespace encoder {
template <class value_t, class index_t> class identity {
  struct result_t {
    result_t() = default;
    result_t(std::vector<value_t> x,
             std::vector<index_t> indices = std::vector<index_t>{})
        : x(std::move(x)), indices(std::move(indices)) {
      const size_t d = this->x.size();
      xb = this->x.data();
      xe = xb + d;
      ib = (this->indices.size() == d) ? indices.data() : nullptr;
      ie = (this->indices.size() == d) ? ib + d : nullptr;
    }
    result_t(const value_t *xb, const value_t *xe) : xb(xb), xe(xe) {}
    result_t(const value_t *xb, const value_t *xe, const index_t *ib,
             const index_t *ie)
        : xb(xb), xe(xe), ib(ib), ie(ie) {}

    size_t size() const noexcept {
      return sizeof(value_t) * std::distance(xb, xe) +
             sizeof(index_t) * std::distance(ib, ie);
    }

    template <class Archive> void load(Archive &archive) {
      archive(x, indices);
      const size_t d = x.size();
      xb = x.data();
      xe = xb + d;
      ib = (indices.size() == d) ? indices.data() : nullptr;
      ie = (indices.size() == d) ? ib + d : nullptr;
    }

    template <class Archive> void save(Archive &archive) const {
      const size_t d =
          (ib == nullptr) ? std::distance(xb, xe) : std::distance(ib, ie);

      if (!x.empty() | (d == 0)) {
        archive(x, indices);
        return;
      }

      std::vector<value_t> x(d);
      std::vector<index_t> indices;

      if (ib != nullptr) {
        indices = std::vector<index_t>(d);
        const index_t *itemp{ib};
        index_t k{0};
        while (itemp != ie) {
          const index_t idx = *itemp++;
          indices[k] = idx;
          x[k] = *(xb + idx);
          k++;
        }
      } else
        std::copy(xb, xe, std::begin(x));

      archive(x, indices);
    }

    result_t slice(const index_t ib, const index_t ie) const {
      if (!indices.empty()) {
        std::vector<index_t> newind;
        std::vector<value_t> newval;
        if (ib <= indices.back()) {
          index_t k{0};
          for (const index_t idx : indices) {
            if ((idx >= ib) & (idx < ie)) {
              newind.push_back(idx);
              newval.push_back(x[k]);
            } else if (idx >= ie)
              break;
            k++;
          }
        }
        return result_t(std::move(newval), std::move(newind));
      } else if (!x.empty()) {
        std::vector<index_t> newind;
        std::vector<value_t> newval;
        for (index_t idx = ib; idx < ie; idx++) {
          newind.push_back(idx);
          newval.push_back(x[idx]);
        }
        return result_t(std::move(newval), std::move(newind));
      } else if (this->ib != nullptr) {
        const index_t *newb{this->ib};
        while (*newb < ib)
          newb++;
        const index_t *newe{newb};
        while ((*newe < ie) & (newe != this->ie))
          newe++;
        return result_t(xb, xe, newb, newe);
      } else {
        std::vector<index_t> newind(ie - ib);
        index_t idx = ib;
        std::generate(std::begin(newind), std::end(newind),
                      [&]() { return idx++; });
        std::vector<value_t> newval(xb + ib, xb + ie);
        return result_t(std::move(newval), std::move(newind));
      }
    }

    template <class OutputIt>
    OutputIt operator()(OutputIt xb, OutputIt xe, const index_t ib = 0) const {
      if (!indices.empty()) {
        std::fill(xb, xe, 0);
        index_t k{0};
        for (const index_t idx : indices)
          *(xb + idx - ib) = x[k++];
      } else if (!x.empty())
        std::copy(std::begin(x), std::end(x), xb);
      else if (this->ib != nullptr) {
        std::fill(xb, xe, 0);
        const index_t *itemp{this->ib};
        while (itemp != ie) {
          const index_t idx = *itemp++;
          *(xb + idx - ib) = *(this->xb + idx - ib);
        }
      } else
        std::copy(this->xb, this->xe, xb);
      return xe;
    }

  private:
    const value_t *xb{nullptr};
    const value_t *xe{nullptr};
    const index_t *ib{nullptr};
    const index_t *ie{nullptr};
    std::vector<value_t> x;
    std::vector<index_t> indices;
  };

public:
  using result_type = result_t;

  identity() = default;

  result_type operator()(const value_t *xb, const value_t *xe) const {
    return result_type(xb, xe);
  }

  result_type operator()(const value_t *xb, const value_t *xe,
                         const index_t *ib, const index_t *ie) const {
    return result_type(xb, xe, ib, ie);
  }
};
} // namespace encoder
} // namespace polo

#endif
