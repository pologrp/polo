#include <iostream>

#include "algorithm.cpp"
#include "descent.cpp"
#include "execution.cpp"
#include "projection.cpp"
#include "smoothing.cpp"
#include "step.cpp"
#include "utility.cpp"

#include "problem/diagqp.cpp"

using namespace pbopt;

int main(int argc, char *argv[]) {
  // Problem definition
  problem::diagqp<float> myqp{{1, 2, 3, 4, 5}, {0.1, 0.2, 0.3, 0.4, 0.5}};

  std::vector<float> x{100, 100, 50, 50, 20}, xopt(x);
  float fopt = myqp.optimum(xopt.begin());

  algorithm::singlestage<float, descent::nesterov, step::constant,
                         smoothing::none>
      alg;

  alg.step_params(0.1);

  alg.set_loss_func(problem::diagqp<float>::wrapper);
  alg.set_loss_data(&myqp);
  alg.termination_condition(utility::maxiter{10000});

  alg.init(x.cbegin(), x.cend());

  for (auto iter : alg) {
    if (iter.k % 200 == 0) {
      std::cout << "k: " << iter.k << ", fval: " << iter.fval << ", x: [";
      for (const auto xval : iter.x)
        std::cout << xval << ',';
      std::cout << "],\n";
    }
  }

  std::cout << "fopt: " << fopt << ", xopt: [";
  for (const auto xval : xopt)
    std::cout << xval << ',';
  std::cout << "].\n";

  return 0;
}
