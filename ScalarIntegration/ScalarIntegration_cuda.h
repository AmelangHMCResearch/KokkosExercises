// -*- C++ -*-
#ifndef SCALARINTEGRATOR_CUDA_H
#define SCALARINTEGRATOR_CUDA_H

#include "../Utilities.h"

#include <cuda_runtime.h>

class CudaTestFunctor {
public:

  static const CpuOrGpuType ProcessorType = Gpu;

  CudaTestFunctor(const array<double, 2> & integrationBounds,
                  const unsigned int numberOfIntervals,
                  const unsigned int numberOfThreadsPerBlock) :
    _integrationBounds(integrationBounds),
    _numberOfIntervals(numberOfIntervals),
    _numberOfThreadsPerBlock(numberOfThreadsPerBlock) {

  }

  void
  computeAnswer(double * answer) const {

    double totalIntegral = 0;

    *answer = totalIntegral;
  }

  string
  getName() const {
    return string("cuda");
  }

private:
  const array<double, 2> _integrationBounds;
  const unsigned int _numberOfIntervals;
  const unsigned int _numberOfThreadsPerBlock;
};

#endif // SCALARINTEGRATOR_CUDA_H
