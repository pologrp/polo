/* include system libraries */
#include <iostream>
#include <vector>
using namespace std;

/* include polo */
#include <polo/polo.hpp>
using namespace polo;

int main(int argc, char *argv[]) {
  /* define the problem data */
  auto data =
      utility::reader<double, int>::svm({"../data/australian_scale"}, 690, 14);

  /* define the smooth loss */
  loss::logistic<double, int> loss(data);

  /* estimate smoothness of the loss */
  double rowmax{0};
  for (int row = 0; row < data.nsamples(); row++) {
    double rowsquare{0};
    for (const auto val : data.matrix()->getrow(row))
      rowsquare += val * val;
    if (rowsquare > rowmax)
      rowmax = rowsquare;
  }
  const double L = 0.25 * data.nsamples() * rowmax;

  /* select and configure the desired solver */
  algorithm::gd<double, int> alg;
  alg.step_parameters(2 / L);

  /* provide an initial vector to the solver, and solve the problem */
  const vector<double> x0(data.nfeatures());
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
