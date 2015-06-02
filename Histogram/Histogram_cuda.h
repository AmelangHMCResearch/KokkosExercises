// -*- C++ -*-
#ifndef HISTOGRAM_CUDA_H
#define HISTOGRAM_CUDA_H

#include "../Utilities.h"

#include <cuda_runtime.h>

class CudaTestFunctor {
public:

  static const CpuOrGpuType ProcessorType = Cpu;

  CudaTestFunctor(const vector<unsigned int> & input,
                  const unsigned int numberOfBuckets,
                  const unsigned int numberOfThreadsPerBlock) :
    _input(input),
    _numberOfBuckets(numberOfBuckets),
    _numberOfThreadsPerBlock(numberOfThreadsPerBlock) {

  }

  void
  computeAnswer(vector<unsigned int> * answer) const {

    vector<unsigned int> & histogram = *answer;

  }

  string
  getName() const {
    return string("cuda");
  }

private:
  const vector<unsigned int> & _input;
  const unsigned int _numberOfBuckets;
  const unsigned int _numberOfThreadsPerBlock;
};

#endif // HISTOGRAM_CUDA_H
