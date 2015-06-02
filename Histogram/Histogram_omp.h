// -*- C++ -*-
#ifndef HISTOGRAM_OMP_H
#define HISTOGRAM_OMP_H

#include "../Utilities.h"

// header files for omp
#include <omp.h>

class OmpTestFunctor {
public:

  static const CpuOrGpuType ProcessorType = Cpu;

  OmpTestFunctor(const vector<unsigned int> & input,
                 const unsigned int numberOfBuckets) :
    _input(input),
    _numberOfBuckets(numberOfBuckets) {

  }

  void
  computeAnswer(vector<unsigned int> * answer) const {

    vector<unsigned int> & histogram = *answer;

  }

  string
  getName() const {
    return string("omp");
  }

private:
  const vector<unsigned int> & _input;
  const unsigned int _numberOfBuckets;
};

#endif // HISTOGRAM_OMP_H
