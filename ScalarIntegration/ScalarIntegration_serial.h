// -*- C++ -*-
#ifndef SCALARINTEGRATOR_SERIAL_H
#define SCALARINTEGRATOR_SERIAL_H

#include "../Utilities.h"

class SerialTestFunctor {
public:

  static const CpuOrGpuType ProcessorType = Cpu;

  SerialTestFunctor(const array<double, 2> & integrationBounds,
                    const unsigned int numberOfIntervals) :
    _integrationBounds(integrationBounds),
    _numberOfIntervals(numberOfIntervals) {

  }

  void
  computeAnswer(double * answer) const {

    const size_t numberOfIntervals = _numberOfIntervals;
    const double dx =
      (_integrationBounds[1] - _integrationBounds[0]) / numberOfIntervals;
    const double integrationBounds0 = _integrationBounds[0];

    double totalIntegral = 0;
    for (size_t intervalIndex = 0;
         intervalIndex < numberOfIntervals; ++intervalIndex) {
      const double evaluationPoint =
        integrationBounds0 + (double(intervalIndex) + 0.5) * dx;
      totalIntegral += std::sin(evaluationPoint);
    }
    totalIntegral *= dx;

    *answer = totalIntegral;
  }

  string
  getName() const {
    return string("serial");
  }

private:
  const array<double, 2> _integrationBounds;
  const unsigned int _numberOfIntervals;
};

#endif // SCALARINTEGRATOR_SERIAL_H
