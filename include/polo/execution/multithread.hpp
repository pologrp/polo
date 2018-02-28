#ifndef MULTITHREAD_HPP_
#define MULTITHREAD_HPP_

#include <atomic>
#include <iterator>
#include <mutex>
#include <thread>
#include <vector>

#include "polo/utility/atomic.hpp"

namespace polo {
namespace execution {

template <class value_t> struct consistent {
  consistent() = default;

  consistent(const consistent &) = default;
  consistent &operator=(const consistent &) = default;
  consistent(consistent &&) = default;
  consistent &operator=(consistent &&) = default;

protected:
  void params(const unsigned int nthreads) { this->nthreads = nthreads; }

  template <class InputIt> void initialize(InputIt xbegin, InputIt xend) {
    x = std::vector<value_t>(xbegin, xend);
    g = std::vector<value_t>(x.size());
  }

  template <class T, class Func1, class Func2>
  void solve(T *algptr, Func1 &&loss, Func2 &&terminate) {
    std::vector<std::thread> workers(nthreads);
    for (auto &worker : workers)
      worker = std::thread([&]() { kernel(algptr, loss, terminate); });

    for (auto &worker : workers)
      worker.join();
  }

  value_t getf() const { return fval; }
  std::vector<value_t> getx() const { return x; }

  ~consistent() = default;

private:
  template <class T, class Func1, class Func2>
  void kernel(T *algptr, Func1 &&loss, Func2 &&terminate) {
    value_t step, flocal;
    const std::size_t dim{x.size()};

    std::vector<value_t> xlocal(dim);
    const value_t *xbegin{&xlocal[0]};

    std::vector<value_t> glocal(dim);

    while (true) {
      {
        std::lock_guard<std::mutex> lock(sync);
        std::copy(std::begin(x), std::end(x), std::begin(xlocal));
      }

      flocal = loss(xbegin, &glocal[0]);

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
  value_t fval{0};
  std::vector<value_t> x, g;
  unsigned int nthreads{std::thread::hardware_concurrency()};
  std::mutex sync;
};

template <class value_t> struct _select_atomic;

template <> struct _select_atomic<float> {
  using type = utility::atomic_float;
};

template <> struct _select_atomic<double> {
  using type = utility::atomic_double;
};

template <class value_t> struct inconsistent {
  using atomic_value_t = typename _select_atomic<value_t>::type;

  inconsistent() = default;

  inconsistent(const inconsistent &) = delete;
  inconsistent &operator=(const inconsistent &) = delete;
  inconsistent(inconsistent &&) = default;
  inconsistent &operator=(inconsistent &&) = default;

protected:
  void params(const unsigned int nthreads) { this->nthreads = nthreads; }

  template <class InputIt> void initialize(InputIt xbegin, InputIt xend) {
    x = std::vector<atomic_value_t>(xbegin, xend);
    g = std::vector<atomic_value_t>(x.size());
  }

  template <class T, class Func1, class Func2>
  void solve(T *algptr, Func1 &&loss, Func2 &&terminate) {
    std::vector<std::thread> workers(nthreads);
    for (auto &worker : workers)
      worker = std::thread([&]() { kernel(algptr, loss, terminate); });

    for (auto &worker : workers)
      worker.join();
  }

  value_t getf() const { return fval; }
  std::vector<value_t> getx() const {
    return std::vector<value_t>(std::begin(x), std::end(x));
  }

  ~inconsistent() = default;

private:
  template <class T, class Func1, class Func2>
  void kernel(T *algptr, Func1 &&loss, Func2 &&terminate) {
    value_t step, flocal;
    const std::size_t dim{x.size()};

    std::vector<value_t> xlocal(dim);
    const value_t *xbegin{&xlocal[0]};

    std::vector<value_t> glocal(dim);

    while (!terminate(k, fval, std::begin(x), std::end(x), std::begin(g))) {
      std::copy(std::begin(x), std::end(x), std::begin(xlocal));

      flocal = loss(xbegin, &glocal[0]);

      algptr->grad(std::begin(glocal), std::end(glocal), std::begin(g));
      algptr->smooth(k, std::begin(xlocal), std::end(xlocal), std::begin(g),
                     std::begin(g));
      step = algptr->step(k, fval, std::begin(xlocal), std::end(xlocal),
                          std::begin(g));
      algptr->project(step, std::begin(xlocal), std::end(xlocal), std::begin(g),
                      std::begin(x));
      k++;
    }
  }

  std::atomic<std::size_t> k;
  atomic_value_t fval;
  std::vector<atomic_value_t> x, g;
  unsigned int nthreads{std::thread::hardware_concurrency()};
};

} // namespace execution
} // namespace polo

#endif
