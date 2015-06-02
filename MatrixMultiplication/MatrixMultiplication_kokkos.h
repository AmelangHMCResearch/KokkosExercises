// -*- C++ -*-
#ifndef MATRIXMULTIPLICATION_KOKKOS_H
#define MATRIXMULTIPLICATION_KOKKOS_H

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

  KokkosTestFunctor(const vector<double> & leftMatrix,
                    const vector<double> & rightMatrix,
                    const unsigned int matrixSize) :
    _leftMatrix(leftMatrix),
    _rightMatrix(rightMatrix),
    _matrixSize(matrixSize) {

  }

  void
  computeAnswer(vector<double> * answer) const {

    vector<double> & resultMatrix = *answer;

  }

  string
  getName() const {
    return string("kokkos ") +
      Utilities::KokkosDeviceNameConverter<DeviceType>().getName();
  }

private:
  const vector<double> & _leftMatrix;
  const vector<double> & _rightMatrix;
  const unsigned int _matrixSize;
};

#endif // MATRIXMULTIPLICATION_KOKKOS_H
