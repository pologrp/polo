/* include system libraries */
#include <cmath>
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

  const double l2 = 1E-1 * data.nsamples();

  /* select and configure the desired solver */
#ifdef MOMENTUM
  algorithm::momentum<double, int, prox::l1norm> alg;
  alg.boosting_parameters(0.9, 2 / (L + 2 * l2));
  const string filename{"logistic-l1-l2-momentum.csv"};
#elif defined NESTEROV
  algorithm::nesterov<double, int, prox::l1norm> alg;
  alg.boosting_parameters(0.9, 2 / (L + 2 * l2));
  const string filename{"logistic-l1-l2-nesterov.csv"};
#elif defined ADAGRAD
  algorithm::adagrad<double, int, prox::l1norm> alg;
  const string filename{"logistic-l1-l2-adagrad.csv"};
#elif defined ADAM
  algorithm::adam<double, int, prox::l1norm> alg;
  alg.boosting_parameters(0.9, 0.1);
  alg.smoothing_parameters(0.999, 1E-8);
  alg.step_parameters(0.001);
  const string filename{"logistic-l1-l2-adam.csv"};
#endif

  const double l1 = 5E-2;
  alg.prox_parameters(l1);

  /* pick a state logger */
  utility::logger::decision<double, int> logger;

  /* pick a terminator */
  terminator::decision<double, int> terminator(1E-3);

  /* provide an initial vector to the solver, and solve the problem */
  const vector<double> x0(data.nfeatures());
  alg.initialize(x0);
  alg.solve(
      [&loss, l2](const double *x, double *g) {
        const double data_loss = loss(x, g);
        double l2_loss{0};
        for (int idx = 0; idx < loss.nfeatures(); idx++) {
          l2_loss += l2 * x[idx] * x[idx];
          g[idx] += l2 * x[idx];
        }
        return data_loss + 0.5 * l2_loss;
      },
      logger, terminator);

  /* open a csv file for writing */
  ofstream file(filename);
  if (file) { /* if successfully opened for writing */
    file << "k,t,f\n";
    for (const auto &log : logger) {
      double l1_loss{0};
      for (const auto val : log.getx())
        l1_loss += l1 * abs(val);

      file << fixed << log.getk() << ',' << log.gett() << ','
           << (log.getf() + l1_loss) << '\n';
    }
  }

  /* print the result */
  cout << "Optimum: " << fixed << alg.getf() << '\n';
  cout << "Optimizer: [";
  for (const auto val : alg.getx())
    cout << val << ',';
  cout << "].\n";

  return 0;
}
