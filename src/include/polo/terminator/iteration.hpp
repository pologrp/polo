#ifndef POLO_TERMINATOR_ITERATION_HPP_
#define POLO_TERMINATOR_ITERATION_HPP_

namespace polo {
namespace terminator {
template <class value_t = double, class index_t = int> struct iteration {
  iteration(const index_t K) : K{K} {}

  template <class InputIt1, class InputIt2>
  bool operator()(const index_t k, const value_t fval, InputIt1 x_begin,
                  InputIt1 x_end, InputIt2 g_begin) const {
    return k > K;
  }

private:
  const index_t K;
};
} // namespace terminator
} // namespace polo

#endif
