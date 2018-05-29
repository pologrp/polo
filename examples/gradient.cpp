#include <iostream>
#include <vector>

// Define problem parameters
const double Q[] = {1, 2, 3};
const double q[] = {-0.1, 0.2, -0.3};

double quadratic_loss(const double *x, double *g) {
  // Function value
  double fval = 0;
  for (int idx = 0; idx < 3; idx++) {
    // Gradient computation
    g[idx] = Q[idx] * x[idx] + q[idx];
    // Function value computation
    fval += 0.5 * x[idx] * Q[idx] * x[idx] + q[idx] * x[idx];
  }
  return fval;
}

#include "polo/polo.hpp"
using namespace polo;

int main(int argc, char *argv[]) {
  // Select the algorithm
  algorithm::proxgradient<double> alg;
  // Set the step size
  alg.step_parameters(0.5);
  // Initialize the algorithm with x0
  std::vector<double> x0 = {10, 20, 30};
  alg.initialize(x0);
  // Solve the problem
  alg.solve(quadratic_loss);
  // Get the solution
  std::vector<double> xopt = alg.getx();
  double fopt = alg.getf();
  // Print the result
  std::cout << "Optimizer is [";
  for (int idx = 0; idx < 3; idx++)
    std::cout << xopt[idx] << ',';
  std::cout << "].\n";
  std::cout << "Optimum value is " << fopt << ".\n";
  // Exit the program
  return 0;
}
