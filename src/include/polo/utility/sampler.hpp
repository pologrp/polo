#ifndef POLO_UTILITY_SAMPLER_HPP_
#define POLO_UTILITY_SAMPLER_HPP_

#include <iterator>
#include <random>
#include <set>
#include <utility>

namespace polo {
namespace utility {
namespace sampler {
namespace detail {
template <class index_t, template <class> class distribution_t,
          class generator_t = std::mt19937>
struct sampler : private distribution_t<index_t> {
  using param_type = typename distribution_t<index_t>::param_type;
  using result_type = typename distribution_t<index_t>::result_type;

  sampler() = default;
  sampler(distribution_t<index_t> dist, generator_t gen = std::mt19937{})
      : distribution_t<index_t>(std::move(dist)), gen(std::move(gen)) {}
  sampler(const sampler &s)
      : distribution_t<index_t>(s), gen(std::random_device{}()) {}
  sampler &operator=(const sampler &rhs) {
    distribution_t<index_t>::operator=(rhs);
    gen = generator_t(std::random_device{}());
    return *this;
  }
  sampler(sampler &&) = default;
  sampler &operator=(sampler &&) = default;

  template <class... Ts> void seed(const Ts &... seed) { gen.seed(seed...); }

  template <class... Ts> void parameters(const Ts &... params) {
    distribution_t<index_t>::param(param_type(params...));
  }
  void parameters(const param_type &params) {
    distribution_t<index_t>::param(params);
  }
  param_type parameters() const { return distribution_t<index_t>::param(); }

  template <class OutputIt>
  OutputIt operator()(OutputIt sbegin, OutputIt send) {
    index_t val;
    std::set<index_t> values;
    const size_t d = std::distance(sbegin, send);
    while (values.size() != d) {
      do {
        val = distribution_t<index_t>::operator()(gen);
      } while (values.count(val) == 1);
      values.insert(val);
    }
    for (const index_t val : values)
      *sbegin++ = val;
    return sbegin;
  }

private:
  generator_t gen;
};

struct coordinate_sampler_t {};
struct component_sampler_t {};
} // namespace detail

template <class index_t = int, class generator_t = std::mt19937>
using uniform =
    detail::sampler<index_t, std::uniform_int_distribution, generator_t>;
template <class index_t = int, class generator_t = std::mt19937>
using custom =
    detail::sampler<index_t, std::discrete_distribution, generator_t>;

constexpr detail::coordinate_sampler_t coordinate;
constexpr detail::component_sampler_t component;
} // namespace sampler
} // namespace utility
} // namespace polo

#endif
