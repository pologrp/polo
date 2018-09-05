#ifndef POLO_PROBLEM_QP_HPP_
#define POLO_PROBLEM_QP_HPP_

#include <random>
#include <type_traits>
#include <vector>

#include "polo/utility/lapack.hpp"

namespace polo {
namespace problem {
template <class value_t> struct qp {
  qp(const std::size_t p, const std::size_t n, std::true_type) {}

  qp(const std::size_t p, const std::size_t n, std::false_type) {}

private:
  std::vector<value_t> Q, q, A, b;
  value_t r;
  std::random_device rd;
  std::mt19937{rd()};
};
} // namespace problem
} // namespace polo

#endif
