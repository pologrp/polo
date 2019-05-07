/* include system libraries */
#include <fstream>
#include <iostream>
#include <string>
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
#ifdef MOMENTUM
  algorithm::momentum<double, int> alg;
  alg.boosting_parameters(0.9, 1 / L);
  const string filename{"logistic-momentum.csv"};
#elif defined NESTEROV
  algorithm::nesterov<double, int> alg;
  alg.boosting_parameters(0.9, 1 / L);
  const string filename{"logistic-nesterov.csv"};
#elif defined ADAGRAD
  algorithm::adagrad<double, int> alg;
  const string filename{"logistic-adagrad.csv"};
#elif defined ADAM
  algorithm::adam<double, int> alg;
  alg.boosting_parameters(0.9, 0.1);
  alg.smoothing_parameters(0.999, 1E-8);
  alg.step_parameters(0.001);
  const string filename{"logistic-adam.csv"};
#endif

  /* pick a state logger */
  utility::logger::value<double, int> logger;

  /* pick a terminator */
  terminator::value<double, int> terminator(5E-2);

  /* provide an initial vector to the solver, and solve the problem */
  const vector<double> x0(data.nfeatures());
  alg.initialize(x0);
  alg.solve(loss, logger, terminator);

  /* open a csv file for writing */
  ofstream file(filename);
  if (file) { /* if successfully opened for writing */
    file << "k,t,f\n";
    for (const auto &log : logger)
      file << fixed << log.getk() << ',' << log.gett() << ',' << log.getf()
           << '\n';
  }

  /* print the result */
  cout << "Optimum: " << fixed << alg.getf() << '\n';
  cout << "Optimizer: [";
  for (const auto val : alg.getx())
    cout << val << ',';
  cout << "].\n";

  return 0;
}
