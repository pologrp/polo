#include <iostream>

#include "algorithm.cpp"
#include "execution.cpp"
#include "gradient.cpp"
#include "problem.cpp"
#include "projection.cpp"
#include "smoothing.cpp"
#include "step.cpp"
#include "utility.cpp"

using namespace pbopt;

int main(int argc, char *argv[]) {
  // Problem definition
  problem::qp<float> myqp{{1, 2, 3, 4, 5}, {0.1, 0.2, 0.3, 0.4, 0.5}};

  std::vector<float> x{100, 100, 50, 50, 20}, xopt(x);
  float fopt = myqp.optimum(xopt.begin());

  algorithm::singlestage<float, gradient::nesterov, step::constant,
                         smoothing::rmsprop>
      alg;

  alg.step_params(0.1);
  alg.initialize(x.cbegin(), x.cend());

  alg.solve(myqp);

  float fval = alg.getf();
  x = alg.getx();

  std::cout << "fopt: " << fopt << ", xopt: [";
  for (const auto xval : xopt)
    std::cout << xval << ',';
  std::cout << "].\n";

  std::cout << "fval: " << fval << ", x: [";
  for (const auto xval : x)
    std::cout << xval << ',';
  std::cout << "].\n";

  return 0;
}
