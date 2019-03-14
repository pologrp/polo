/* include system libraries */
#include <iostream>
#include <vector>
using namespace std;

/* include polo */
#include <polo/polo.hpp>
using namespace polo;

int main(int argc, char *argv[]) {
  /* define the problem data */
  matrix::dmatrix<double, int> A(3, 3, {1, 0, 0, 0, 1, 0, 0, 0, 1});
  vector<double> b{-1, 1, 1};
  loss::data<double, int> data(A, b);

  /* define the smooth loss */
  loss::logistic<double, int> loss(data);

  /* select and configure the desired solver */
  algorithm::gd<double, int> alg;
  alg.step_parameters(1.0);

  /* provide an initial vector to the solver, and solve the problem */
  const vector<double> x0{1, 1, 1};
  alg.initialize(x0);
  alg.solve(loss);

  /* print the result */
  cout << "Optimum: " << alg.getf() << '\n';
  cout << "Optimizer: [";
  for (const auto val : alg.getx())
    cout << val << ',';
  cout << "].\n";

  return 0;
}
