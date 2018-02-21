#ifndef MULTITHREAD_HPP_
#define MULTITHREAD_HPP_

#include <iterator>
#include <mutex>
#include <thread>
#include <vector>

namespace pbopt {
namespace execution {

template <class float_t> struct consistent {
  consistent() = default;

  consistent(const consistent &) = default;
  consistent &operator=(const consistent &) = default;
  consistent(consistent &&) = default;
  consistent &operator=(consistent &&) = default;

protected:
  void params(const unsigned int nthreads) { this->nthreads = nthreads; }

  template <class InputIt> void initialize(InputIt xbegin, InputIt xend) {
    x = std::vector<float_t>(xbegin, xend);
    g = std::vector<float_t>(x.size());
  }

  template <class T, class Func1, class Func2>
  void solve(T *algptr, Func1 &&loss, Func2 &&terminate) {
    std::vector<std::thread> workers(nthreads);
    for (auto &worker : workers)
      worker = std::thread([&]() { kernel(algptr, loss, terminate); });

    for (auto &worker : workers)
      worker.join();
  }

  float_t getf() const { return fval; }
  std::vector<float_t> getx() const { return x; }

  ~consistent() = default;

private:
  template <class T, class Func1, class Func2>
  void kernel(T *algptr, Func1 &&loss, Func2 &&terminate) {
    float_t step, flocal;
    const std::size_t dim{x.size()};

    std::vector<float_t> xlocal(dim);
    const float_t *xbegin{xlocal.data()};
    const float_t *xend{xbegin + dim};

    std::vector<float_t> glocal(dim);
    const float_t *gbegin{glocal.data()};
    const float_t *gend{gbegin + dim};

    while (true) {
      {
        std::lock_guard<std::mutex> lock(sync);
        std::copy(std::begin(x), std::end(x), std::begin(xlocal));
      }

      flocal = loss(xbegin, xend, glocal.data());

      {
        std::lock_guard<std::mutex> lock(sync);

        if (terminate(k, fval, std::begin(x), std::end(x), std::begin(g)))
          break;

        algptr->grad(std::begin(glocal), std::end(glocal), std::begin(g));
        algptr->smooth(k, std::begin(xlocal), std::end(xlocal), std::begin(g),
                       std::begin(g));
        step = algptr->step(k, fval, std::begin(xlocal), std::end(xlocal),
                            std::begin(g));
        algptr->project(step, std::begin(xlocal), std::end(xlocal),
                        std::begin(g), std::begin(x));
        k++;
      }
    }
  }

  std::size_t k{1};
  float_t fval{0};
  std::vector<float_t> x, g;
  unsigned int nthreads{std::thread::hardware_concurrency()};
  std::mutex sync;
};

} // namespace execution
} // namespace pbopt

#endif
