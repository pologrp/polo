#include <iostream>
#include <iterator>

#include "pbopt.hpp"

using namespace pbopt;

int main(int argc, char *argv[]) {
  // Problem definition
  problem::qp<float> myqp{{1, 2, 3, 4, 5}, {0.1, 0.2, 0.3, 0.4, 0.5}};

  std::vector<float> x0{100, 100, 50, 50, 20};
  std::vector<float> xopt(x0);

  float fopt = myqp.optimum(xopt);

  // Algorithm specifications
  algorithm::singlestage<float, gradient::nesterov, step::constant,
                         smoothing::rmsprop>
      alg;

  alg.step_params(0.1);
  alg.initialize(x0);

  // Problem solution
  alg.solve(myqp);
  float fval = alg.getf();
  std::vector<float> x = alg.getx();

  // Problem printing
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
