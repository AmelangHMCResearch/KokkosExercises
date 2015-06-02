// -*- C++ -*-
#ifndef HISTOGRAM_KOKKOS_H
#define HISTOGRAM_KOKKOS_H

#include "../Utilities.h"

template <class DeviceType>
struct KokkosWorkerFunctor {

  typedef DeviceType device_type;

  KokkosWorkerFunctor(const unsigned int somethingMaybeHere) {
  }

  KOKKOS_INLINE_FUNCTION
  void operator()(const unsigned int index) const {
  }

private:
  KokkosWorkerFunctor();

};

template <class DeviceType>
class KokkosTestFunctor {
public:

  // yes this is fishy, there's nothing to see here, move along.
  static const CpuOrGpuType ProcessorType =
    Utilities::KokkosProcessorTypeConverter<DeviceType>::ProcessorType;

  KokkosTestFunctor(const vector<unsigned int> & input,
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
    return string("kokkos ") +
      Utilities::KokkosDeviceNameConverter<DeviceType>().getName();
  }

private:
  const vector<unsigned int> & _input;
  const unsigned int _numberOfBuckets;
};

#endif // HISTOGRAM_KOKKOS_H
