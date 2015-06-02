// -*- C++ -*-
#ifndef SCALARINTEGRATOR_KOKKOS_H
#define SCALARINTEGRATOR_KOKKOS_H

#include "../Utilities.h"

// header files for kokkos
#include <Kokkos_Core.hpp>

template <class DeviceType>
struct KokkosWorkerFunctor {

  typedef DeviceType device_type;

  KokkosWorkerFunctor(const array<double, 2> & integrationBounds) :
    _integrationBounds(integrationBounds) {
  }

  KOKKOS_INLINE_FUNCTION
  void operator()(const unsigned int intervalIndex) const {
  }

private:
  KokkosWorkerFunctor();
  const array<double, 2> _integrationBounds;

};

template <class DeviceType>
class KokkosTestFunctor {
public:

  // yes this is fishy, there's nothing to see here, move along.
  static const CpuOrGpuType ProcessorType =
    Utilities::KokkosProcessorTypeConverter<DeviceType>::ProcessorType;

  KokkosTestFunctor(const array<double, 2> & integrationBounds,
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
    return string("kokkos ") +
      Utilities::KokkosDeviceNameConverter<DeviceType>().getName();
  }

private:
  const array<double, 2> _integrationBounds;
  const unsigned int _numberOfIntervals;
};

#endif // SCALARINTEGRATOR_KOKKOS_H
