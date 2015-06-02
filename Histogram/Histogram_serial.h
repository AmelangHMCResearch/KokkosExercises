// -*- C++ -*-
#ifndef HISTOGRAM_SERIAL_H
#define HISTOGRAM_SERIAL_H

#include "../Utilities.h"

class SerialTestFunctor {
public:

  static const CpuOrGpuType ProcessorType = Cpu;

  SerialTestFunctor(const vector<unsigned int> & input,
                    const unsigned int numberOfBuckets) :
    _input(input),
    _numberOfBuckets(numberOfBuckets) {

  }

  void
  computeAnswer(vector<unsigned int> * answer) const {

    vector<unsigned int> & histogram = *answer;

    const unsigned int numberOfElements = _input.size();
    const unsigned int bucketSize = numberOfElements / _numberOfBuckets;
    for (unsigned int index = 0; index < numberOfElements; ++index) {
      const unsigned int value = _input[index];
      const unsigned int bucketNumber = value / bucketSize;
      ++histogram[bucketNumber];
    }

  }

  string
  getName() const {
    return string("serial");
  }

private:
  const vector<unsigned int> & _input;
  const unsigned int _numberOfBuckets;
};

#endif // HISTOGRAM_SERIAL_H
