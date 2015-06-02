// -*- C++ -*-
#ifndef MATRIXMULTIPLICATION_OMP_H
#define MATRIXMULTIPLICATION_OMP_H

#include "../Utilities.h"

// header files for omp
#include <omp.h>

class OmpTestFunctor {
public:

  static const CpuOrGpuType ProcessorType = Cpu;

  OmpTestFunctor(const vector<double> & leftMatrix,
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
    return string("omp");
  }

private:
  const vector<double> & _leftMatrix;
  const vector<double> & _rightMatrix;
  const unsigned int _matrixSize;
};

#endif // MATRIXMULTIPLICATION_OMP_H
