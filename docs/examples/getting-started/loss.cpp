/* include system libraries */
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

/* include polo */
#include <polo/polo.hpp>
using namespace polo;

struct simple_loss {
  simple_loss(const int N) : N{N} {}

  double operator()(const double *x, double *g) const {
    double loss{0};
    for (int n = 1; n <= N; n++) {
      const double residual{x[n - 1] - n};
      loss += residual * residual / n;
      g[n - 1] = 2 * residual / n;
    }
    return loss;
  }

private:
  const int N;
};

int main(int argc, char *argv[]) {
  /* define the smooth loss */
  const int N{10};
  simple_loss loss(N);
  const double mu{2. / N};
  const double L{2.};

  /* select and configure the desired solver */
  algorithm::gd<double, int> alg;
  alg.step_parameters(2 / (mu + L));

  /* pick a state logger */
  utility::logger::value<double, int> logger;

  /* pick a terminator */
  terminator::value<double, int> terminator(1E-8, 1E-13);

  /* provide an initial vector to the solver, and solve the problem */
  const vector<double> x0(N);
  alg.initialize(x0);
  alg.solve(loss, logger, terminator);

  /* open a csv file for writing */
  ofstream file("loss.csv");
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
