// -*- C++ -*-
#ifndef UTILITIES_H
#define UTILITIES_H

#include <cuda_runtime.h>

// header files for kokkos types
#include <Kokkos_Core.hpp>

// this silly thing is to avoid unused variable warnings from the compiler
// with -Wall on.
// here we disable compiler warnings...because that's the whole idea.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
template <class T>
void ignoreUnusedVariables(T & t) {
}
#pragma GCC diagnostic pop

// stolen from http://stackoverflow.com/questions/14038589/what-is-the-canonical-way-to-check-for-errors-using-the-cuda-runtime-api
#define checkCudaError(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline
void
gpuAssert(const cudaError_t code, const char *file, const int line, bool abort=true) {
  if (code != cudaSuccess) {
    fprintf(stderr,"GPU Error: %s %s %d\n", cudaGetErrorString(code), file, line);
    if (abort == true) {
      exit(code);
    }
  }
}

enum CpuOrGpuType {Cpu, Gpu};

namespace Utilities {

__global__
void
countThreads_kernel(unsigned int * totalCount) {
  atomicAdd(totalCount, 1);
}

void
resetProcessorForTiming(const CpuOrGpuType processorType) {

  if (processorType == Gpu) {
    // allocate somewhere for the threads to count
    unsigned int *dev_junkDataCounter;
    checkCudaError(cudaMalloc((void **) &dev_junkDataCounter,
                              1*sizeof(unsigned int)));

    const unsigned int resetGpuNumberOfThreadsPerBlock = 1024;
    const unsigned int resetGpuNumberOfBlocks = 1e8 /
      resetGpuNumberOfThreadsPerBlock;
    countThreads_kernel<<<resetGpuNumberOfBlocks,
      resetGpuNumberOfThreadsPerBlock>>>(dev_junkDataCounter);

    // pull the junk data counter back from the device, for fun.
    unsigned int junkDataCounter;
    checkCudaError(cudaMemcpy(&junkDataCounter, dev_junkDataCounter,
                              1*sizeof(unsigned int),
                              cudaMemcpyDeviceToHost));
    volatile unsigned int deOptimizer = junkDataCounter;
    ignoreUnusedVariables(deOptimizer);

    // clean up
    checkCudaError(cudaFree(dev_junkDataCounter));
  } else {
    volatile double junkSum = 0;
    const size_t sizeOfJunk = 1e7;
    vector<double> junk(sizeOfJunk, 1);
    std::accumulate(junk.begin(), junk.end(), junkSum);
  }
}

template <class DeviceType>
struct KokkosProcessorTypeConverter {
};

template <>
struct KokkosProcessorTypeConverter<Kokkos::OpenMP> {
  static const CpuOrGpuType ProcessorType = Cpu;
};

template <>
struct KokkosProcessorTypeConverter<Kokkos::Cuda> {
  static const CpuOrGpuType ProcessorType = Gpu;
};

template <class DeviceType>
struct KokkosDeviceNameConverter {
};

template <>
struct KokkosDeviceNameConverter<Kokkos::OpenMP> {
  static
  string
  getName() {
    return string("OpenMP");
  }
};

template <>
struct KokkosDeviceNameConverter<Kokkos::Cuda> {
  static
  string
  getName() {
    return string("Cuda");
  }
};

}

#endif // UTILITIES_H
