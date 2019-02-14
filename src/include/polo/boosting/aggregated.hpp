#ifndef POLO_BOOSTING_AGGREGATED_HPP_
#define POLO_BOOSTING_AGGREGATED_HPP_

#include <algorithm>
#include <iterator>
#include <map>
#include <vector>

namespace polo {
namespace boosting {
template <class value_t, class index_t> struct aggregated {
  aggregated() = default;

  aggregated(const aggregated &) = default;
  aggregated &operator=(const aggregated &) = default;
  aggregated(aggregated &&) = default;
  aggregated &operator=(aggregated &&) = default;

  template <class InputIt, class OutputIt>
  OutputIt boost(const index_t wid, const index_t, const index_t,
                 InputIt gprev_b, InputIt gprev_e, OutputIt gcurr) {
    if (gradients.count(wid) > 0)
      std::copy(gprev_b, gprev_e, std::begin(gradients[wid]));
    else
      gradients[wid] = std::vector<value_t>(gprev_b, gprev_e);
    for (auto &g : gradient)
      g = 0;
    for (const auto &p : gradients)
      std::transform(
          std::begin(p.second), std::end(p.second), std::begin(gradient),
          std::begin(gradient),
          [](const value_t lhs, const value_t rhs) { return lhs + rhs; });
    return std::copy(std::begin(gradient), std::end(gradient), gcurr);
  }

protected:
  template <class InputIt> void initialize(InputIt begin, InputIt end) {
    gradient = std::vector<value_t>(std::distance(begin, end));
  }

  ~aggregated() = default;

private:
  std::map<index_t, std::vector<value_t>> gradients;
  std::vector<value_t> gradient;
};
} // namespace boosting
} // namespace polo

#endif
