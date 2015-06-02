// -*- C++ -*-
#ifndef SCALARINTEGRATOR_OMP_H
#define SCALARINTEGRATOR_OMP_H

#include "Utilities.h"

// header files for omp
#include <omp.h>

class OmpTestFunctor {
public:

  static const CpuOrGpuType ProcessorType = Cpu;

  OmpTestFunctor(const array<double, 2> & integrationBounds,
                 const unsigned int numberOfIntervals) :
    _integrationBounds(integrationBounds),
    _numberOfIntervals(numberOfIntervals) {

  }

  void
  computeAnswer(double * answer) const {

    double totalIntegral = 0;

    *answer = totalIntegral;
  }

  string
  getName() const {
    return string("omp");
  }

private:
  const array<double, 2> _integrationBounds;
  const unsigned int _numberOfIntervals;
};

#endif // SCALARINTEGRATOR_OMP_H
