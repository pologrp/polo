#ifndef POLO_UTILITY_READER_HPP_
#define POLO_UTILITY_READER_HPP_

#include <fstream>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

#include "polo/loss/data.hpp"
#include "polo/matrix/dmatrix.hpp"
#include "polo/matrix/smatrix.hpp"

namespace polo {
namespace utility {
template <class value_t, class index_t> struct reader {
  static loss::data<value_t, index_t>
  svm(const std::vector<std::string> &filenames) {
    char delim;
    index_t colidxmax{0}, colidx;
    value_t label, value;
    std::vector<value_t> labels, values;
    std::vector<index_t> row_ptr{0}, cols;

    for (const auto &file : filenames) {
      std::string line;
      std::ifstream input{file};
      if (input) {
        while (std::getline(input, line)) {
          std::stringstream ss{line};
          ss >> label;
          labels.push_back(label);
          while (ss >> colidx >> delim >> value) {
            if (colidx > colidxmax)
              colidxmax = colidx;
            cols.push_back(colidx - 1);
            values.push_back(value);
          }
          row_ptr.push_back(cols.size());
        }
      }
    }

    std::shared_ptr<const polo::matrix::amatrix<value_t, index_t>> A(
        new polo::matrix::smatrix<value_t, index_t>(
            row_ptr.size() - 1, colidxmax, std::move(row_ptr), std::move(cols),
            std::move(values)));
    auto b = std::shared_ptr<const std::vector<value_t>>(
        new std::vector<value_t>(std::move(labels)));

    return loss::data<value_t, index_t>(A, b);
  }
  static loss::data<value_t, index_t>
  svm(const std::vector<std::string> &filenames, const index_t nsamples,
      const index_t nfeatures) {
    char delim;
    index_t rowidx{0}, colidx;
    value_t value;
    std::vector<value_t> labels(nsamples),
        values(std::size_t(nsamples) * nfeatures);

    for (const auto &file : filenames) {
      std::string line;
      std::ifstream input{file};
      if (input) {
        while (std::getline(input, line)) {
          std::stringstream ss{line};
          ss >> labels[rowidx];
          while (ss >> colidx >> delim >> value)
            values[(colidx - 1) * nsamples + rowidx] = value;
          rowidx++;
          if (rowidx == nsamples)
            break;
        }
      }
      if (rowidx == nsamples)
        break;
    }
    std::shared_ptr<const polo::matrix::amatrix<value_t, index_t>> A(
        new polo::matrix::dmatrix<value_t, index_t>(nsamples, nfeatures,
                                                    std::move(values)));
    auto b = std::shared_ptr<const std::vector<value_t>>(
        new std::vector<value_t>(std::move(labels)));

    return loss::data<value_t, index_t>(A, b);
  }
};
} // namespace utility
} // namespace polo

#endif
