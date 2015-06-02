// -*- C++ -*-
#ifndef SCALARINTEGRATOR_TBB_H
#define SCALARINTEGRATOR_TBB_H

#include "../Utilities.h"

// header files for tbb
#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>
#include <tbb/parallel_for.h>

class TbbTestFunctor {
public:

  static const CpuOrGpuType ProcessorType = Cpu;

  TbbTestFunctor(const array<double, 2> & integrationBounds,
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
    return string("tbb");
  }

private:
  const array<double, 2> _integrationBounds;
  const unsigned int _numberOfIntervals;
};

#endif // SCALARINTEGRATOR_TBB_H
