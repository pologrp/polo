#ifndef POLO_EXECUTION_SERIAL_HPP_
#define POLO_EXECUTION_SERIAL_HPP_

#include <iterator>
#include <utility>
#include <vector>

#include "polo/utility/sampler.hpp"

namespace polo {
namespace execution {
template <class value_t, class index_t> struct serial {
  serial() = default;

  serial(const serial &) = default;
  serial &operator=(const serial &) = default;
  serial(serial &&) = default;
  serial &operator=(serial &&) = default;

protected:
  template <class InputIt>
  std::vector<value_t> initialize(InputIt xbegin, InputIt xend) {
    x = std::vector<value_t>(xbegin, xend);
    g = std::vector<value_t>(x.size());
    return x;
  }

  template <class Algorithm, class Loss, class Terminator, class Logger>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger) {
    const value_t *xbegin{&x[0]};

    while (!std::forward<Terminator>(terminate)(k, fval, std::begin(x),
                                                std::end(x), std::begin(g))) {
      fval = std::forward<Loss>(loss)(xbegin, &g[0]);
      iterate(alg, std::forward<Logger>(logger));
    }
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Sampler>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger, utility::sampler::detail::component_sampler_t,
             Sampler &&sampler, const index_t num_components) {
    const value_t *xbegin{&x[0]};
    std::vector<index_t> components(num_components);

    while (!std::forward<Terminator>(terminate)(k, fval, std::begin(x),
                                                std::end(x), std::begin(g))) {
      std::forward<Sampler>(sampler)(std::begin(components),
                                     std::end(components));
      fval = std::forward<Loss>(loss)(xbegin, &g[0], &components[0],
                                      &components[num_components]);
      iterate(alg, std::forward<Logger>(logger));
    }
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Sampler>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger, utility::sampler::detail::coordinate_sampler_t,
             Sampler &&sampler, const index_t num_coordinates) {
    const value_t *xbegin{&x[0]};
    std::vector<index_t> coordinates(num_coordinates);
    std::vector<value_t> partial(num_coordinates);

    while (!std::forward<Terminator>(terminate)(k, fval, std::begin(x),
                                                std::end(x), std::begin(g))) {
      std::forward<Sampler>(sampler)(std::begin(coordinates),
                                     std::end(coordinates));
      fval = std::forward<Loss>(loss)(xbegin, &partial[0], &coordinates[0],
                                      &coordinates[num_coordinates]);
      for (std::size_t idx = 0; idx < num_coordinates; idx++)
        g[coordinates[idx]] = partial[idx];
      iterate(alg, std::forward<Logger>(logger));
    }
  }

  template <class Algorithm, class Loss, class Terminator, class Logger,
            class Sampler1, class Sampler2>
  void solve(Algorithm *alg, Loss &&loss, Terminator &&terminate,
             Logger &&logger, utility::sampler::detail::component_sampler_t,
             Sampler1 &&sampler1, const index_t num_components,
             utility::sampler::detail::coordinate_sampler_t,
             Sampler2 &&sampler2, const index_t num_coordinates) {
    const value_t *xbegin{&x[0]};
    std::vector<index_t> components(num_components),
        coordinates(num_coordinates);
    std::vector<value_t> partial(num_coordinates);

    while (!std::forward<Terminator>(terminate)(k, fval, std::begin(x),
                                                std::end(x), std::begin(g))) {
      std::forward<Sampler1>(sampler1)(std::begin(components),
                                       std::end(components));
      std::forward<Sampler2>(sampler2)(std::begin(coordinates),
                                       std::end(coordinates));
      fval = std::forward<Loss>(loss)(
          xbegin, &partial[0], &components[0], &components[num_components],
          &coordinates[0], &coordinates[num_coordinates]);
      for (std::size_t idx = 0; idx < num_coordinates; idx++)
        g[coordinates[idx]] = partial[idx];
      iterate(alg, std::forward<Logger>(logger));
    }
  }

  value_t getf() const { return fval; }
  std::vector<value_t> getx() const { return x; }

  ~serial() = default;

private:
  template <class Algorithm, class Logger>
  void iterate(Algorithm *alg, Logger &&logger) {
    alg->boost(std::begin(g), std::end(g), std::begin(g));
    alg->smooth(k, std::begin(x), std::end(x), std::begin(g), std::begin(g));
    step_ = alg->step(k, fval, std::begin(x), std::end(x), std::begin(g));
    alg->prox(step_, std::begin(x), std::end(x), std::begin(g), std::begin(x));
    std::forward<Logger>(logger)(k, fval, std::begin(x), std::end(x),
                                 std::begin(g), std::end(g));
    k++;
  }

  index_t k{1};
  value_t fval{0}, step_{0};
  std::vector<value_t> x, g;
};
} // namespace execution
} // namespace polo

#endif
