// -*- C++ -*-
#ifndef MATRIXMULTIPLICATION_TBB_H
#define MATRIXMULTIPLICATION_TBB_H

#include "../Utilities.h"

// header files for tbb
#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>
#include <tbb/parallel_for.h>

class TbbTestFunctor {
public:

  static const CpuOrGpuType ProcessorType = Cpu;

  TbbTestFunctor(const vector<double> & leftMatrix,
                    const vector<double> & rightMatrix,
                    const unsigned int matrixSize) :
    _leftMatrix(leftMatrix),
    _rightMatrix(rightMatrix),
    _matrixSize(matrixSize) {

  }

  void
  computeAnswer(vector<double> * answer) const {

    vector<double> & resultMatrix = *answer;

  }

  string
  getName() const {
    return string("tbb");
  }

private:
  const vector<double> & _leftMatrix;
  const vector<double> & _rightMatrix;
  const unsigned int _matrixSize;
};

#endif // MATRIXMULTIPLICATION_TBB_H
