// -*- C++ -*-
#ifndef HISTOGRAM_TBB_H
#define HISTOGRAM_TBB_H

#include "../Utilities.h"

// header files for tbb
#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>
#include <tbb/parallel_for.h>

class TbbTestFunctor {
public:

  static const CpuOrGpuType ProcessorType = Cpu;

  TbbTestFunctor(const vector<unsigned int> & input,
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
    return string("tbb");
  }

private:
  const vector<unsigned int> & _input;
  const unsigned int _numberOfBuckets;
};

#endif // HISTOGRAM_TBB_H
