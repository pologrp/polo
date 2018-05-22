#ifndef READER_HPP_
#define READER_HPP_

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
  svm(const std::vector<std::string> &filenames, bool dense = false) {
    char delim;
    index_t colidxmax{0}, colidx;
    value_t label, colval;
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
          while (ss >> colidx >> delim >> colval) {
            if (colidx > colidxmax)
              colidxmax = colidx;
            cols.push_back(colidx - 1);
            values.push_back(colval);
          }
          row_ptr.push_back(cols.size());
        }
      }
    }

    polo::matrix::smatrix<value_t, index_t> mat(
        row_ptr.size() - 1, colidxmax, std::move(row_ptr), std::move(cols),
        std::move(values));

    std::shared_ptr<const polo::matrix::amatrix<value_t, index_t>> A;
    auto b = std::shared_ptr<const std::vector<value_t>>(
        new std::vector<value_t>(std::move(labels)));

    if (dense) {
      A = std::shared_ptr<const polo::matrix::amatrix<value_t, index_t>>(
          new polo::matrix::dmatrix<value_t, index_t>(mat.dense()));
    } else {
      A = std::shared_ptr<const polo::matrix::amatrix<value_t, index_t>>(
          new polo::matrix::smatrix<value_t, index_t>(std::move(mat)));
    }

    return loss::data<value_t, index_t>(A, b);
  }
};
} // namespace utility
} // namespace polo

#endif
