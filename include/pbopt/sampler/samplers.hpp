#ifndef SAMPLERS_HPP_
#define SAMPLERS_HPP_

#include <random>
#include <utility>

namespace pbopt {
namespace sampler {

template <class int_t, template <class> class distribution>
struct _sampler : private distribution<int_t> {
  using param_type = typename distribution<int_t>::param_type;
  using result_type = typename std::mt19937::result_type;

  _sampler() = default;
  _sampler(const result_type seed) { gen.seed(seed); }

  void seed(const result_type seed) { gen.seed(seed); }

  template <class... Ts> void param(Ts &&... args) {
    distribution<int_t>::param(param_type(std::forward<Ts>(args)...));
  }
  void param(const param_type &params) { distribution<int_t>::param(params); }

  template <class OutputIt>
  OutputIt operator()(OutputIt sbegin, OutputIt send) {
    while (sbegin != send)
      *sbegin++ = distribution<int_t>::operator()(gen);
    return sbegin;
  }

private:
  std::random_device rd;
  std::mt19937 gen;
};

template <class int_t>
using uniform = _sampler<int_t, std::uniform_int_distribution>;

template <class int_t>
using custom = _sampler<int_t, std::discrete_distribution>;

struct _coordinate_sampler_t {};
struct _component_sampler_t {};

constexpr _coordinate_sampler_t coordinate;
constexpr _component_sampler_t component;

} // namespace sampler
} // namespace pbopt

#endif
