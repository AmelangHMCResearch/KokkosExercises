// -*- C++ -*-
#ifndef MATRIXMULTIPLICATION_CUDA_H
#define MATRIXMULTIPLICATION_CUDA_H

#include "../Utilities.h"

class CudaTestFunctor {
public:

  static const CpuOrGpuType ProcessorType = Cpu;

  CudaTestFunctor(const vector<double> & leftMatrix,
                  const vector<double> & rightMatrix,
                  const unsigned int matrixSize,
                  const unsigned int numberOfThreadsPerBlock) :
    _leftMatrix(leftMatrix),
    _rightMatrix(rightMatrix),
    _matrixSize(matrixSize),
    _numberOfThreadsPerBlock(numberOfThreadsPerBlock) {

  }

  void
  computeAnswer(vector<double> * answer) const {

    vector<double> & resultMatrix = *answer;

  }

  string
  getName() const {
    return string("cuda");
  }

private:
  const vector<double> & _leftMatrix;
  const vector<double> & _rightMatrix;
  const unsigned int _matrixSize;
  const unsigned int _numberOfThreadsPerBlock;
};

#endif // MATRIXMULTIPLICATION_CUDA_H
