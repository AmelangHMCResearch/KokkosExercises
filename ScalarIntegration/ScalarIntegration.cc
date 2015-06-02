// -*- C++ -*-
// ScalarIntegrator.cc
// an exercise for getting to know Kokkos
// here we integrate sin over some interval using various methods.
// i love integrating sin!

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include <chrono>

using std::string;
using std::vector;
using std::array;
using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;

// header files for various implementations
#include "ScalarIntegration_serial.h"
#include "ScalarIntegration_tbb.h"
#include "ScalarIntegration_omp.h"
#include "ScalarIntegration_cuda.h"
#include "ScalarIntegration_kokkos.h"

// header file for kokkos so that i can specify device types here
#include <Kokkos_Core.hpp>

// header file so that i can set the number of threads for tbb
#include <tbb/task_scheduler_init.h>

// header file so that i can set the number of threads for omp
#include <omp.h>

template <class TestFunctor>
void
runTimingTest(const TestFunctor & testFunctor,
              const unsigned int numberOfRepeats,
              const unsigned int numberOfExtraRepeats,
              double * answer,
              double * elapsedTime) {

  high_resolution_clock::time_point tic;

  double thisTestsTime = 0;
  for (unsigned int repeatIndex = 0;
       repeatIndex < numberOfRepeats + numberOfExtraRepeats; ++repeatIndex) {

    // reset the device
    Utilities::resetProcessorForTiming(TestFunctor::ProcessorType);

    // start timing
    if (repeatIndex >= numberOfExtraRepeats) {
      tic = high_resolution_clock::now();
    }

    // run the test
    testFunctor.computeAnswer(answer);

    // stop timing
    if (repeatIndex >= numberOfExtraRepeats) {
      const high_resolution_clock::time_point toc =
        high_resolution_clock::now();
      thisTestsTime += duration_cast<duration<double> >(toc - tic).count();
    }
  }

  *elapsedTime = thisTestsTime / numberOfRepeats;
}

template <class TestFunctor>
void
runTimingTestAndCheckAnswer(const TestFunctor & testFunctor,
                            const unsigned int numberOfRepeats,
                            const unsigned int numberOfExtraRepeats,
                            const double & correctAnswer,
                            double * elapsedTime) {

  // compute the answer and measure elapsed time
  double answer;
  runTimingTest(testFunctor,
                numberOfRepeats,
                numberOfExtraRepeats,
                &answer,
                elapsedTime);

  // check the answer
  Utilities::checkAnswer(correctAnswer, answer,
                         testFunctor.getName());
}

int main() {

  // change the numberOfIntervals to control the amount
  //  of work done.
  const unsigned long numberOfIntervals = 1e8;
  // the arbitrary integration bounds
  const array<double, 2> integrationBounds = {{0, 1.314}};
  // some controls for how many times the test is performed
  const unsigned int numberOfRepeats = 3;
  const unsigned int numberOfExtraRepeats = 1;

  // calculate analytic solution
  const double libraryAnswer =
    std::cos(integrationBounds[0]) - std::cos(integrationBounds[1]);

  // ===============================================================
  // ********************** < do serial> ***************************
  // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

  // perform serial test
  const SerialTestFunctor serialTestFunctor(integrationBounds,
                                            numberOfIntervals);
  double serialElapsedTime;
  runTimingTestAndCheckAnswer(serialTestFunctor,
                              numberOfRepeats,
                              numberOfExtraRepeats,
                              libraryAnswer,
                              &serialElapsedTime);

  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // ********************** </do serial> ***************************
  // ===============================================================

  // we will repeat the computation for each of the numbers of threads
  vector<unsigned int> numberOfThreadsArray;
  numberOfThreadsArray.push_back(1);
  numberOfThreadsArray.push_back(2);
  numberOfThreadsArray.push_back(4);
  numberOfThreadsArray.push_back(8);
  numberOfThreadsArray.push_back(16);
  numberOfThreadsArray.push_back(24);

  // ===============================================================
  // ********************** < do tbb> ******************************
  // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

  printf("performing calculations with tbb\n");
  // for each number of threads
  for (const unsigned int numberOfThreads :
         numberOfThreadsArray) {

    // initialize tbb's threading system for this number of threads
    tbb::task_scheduler_init init(numberOfThreads);

    // perform tbb test
    const TbbTestFunctor tbbTestFunctor(integrationBounds,
                                        numberOfIntervals);
    double tbbElapsedTime;
    runTimingTestAndCheckAnswer(tbbTestFunctor,
                                numberOfRepeats,
                                numberOfExtraRepeats,
                                libraryAnswer,
                                &tbbElapsedTime);

    // output speedup
    printf("%3u : time %8.2e speedup %8.2e (%%%5.1f of ideal)\n",
           numberOfThreads,
           tbbElapsedTime,
           serialElapsedTime / tbbElapsedTime,
           100. * serialElapsedTime / tbbElapsedTime / numberOfThreads);
  }

  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // ********************** </do tbb> ******************************
  // ===============================================================

  // ===============================================================
  // ********************** < do openmp> ***************************
  // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

  printf("performing calculations with openmp\n");
  // for each number of threads
  for (const unsigned int numberOfThreads :
         numberOfThreadsArray) {

    // initialize omp's threading system for this number of threads
    omp_set_num_threads(numberOfThreads);

    // perform tbb test
    const OmpTestFunctor ompTestFunctor(integrationBounds,
                                        numberOfIntervals);
    double ompElapsedTime;
    runTimingTestAndCheckAnswer(ompTestFunctor,
                                numberOfRepeats,
                                numberOfExtraRepeats,
                                libraryAnswer,
                                &ompElapsedTime);

    // output speedup
    printf("%3u : time %8.2e speedup %8.2e (%%%5.1f of ideal)\n",
           numberOfThreads,
           ompElapsedTime,
           serialElapsedTime / ompElapsedTime,
           100. * serialElapsedTime / ompElapsedTime / numberOfThreads);
  }

  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // ********************** </do openmp> ***************************
  // ===============================================================

  // ===============================================================
  // ********************** < do cuda> *****************************
  // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

  // we will repeat the computation for each of the numbers of threads
  vector<unsigned int> threadsPerBlockArray;
  threadsPerBlockArray.push_back(4);
  threadsPerBlockArray.push_back(8);
  threadsPerBlockArray.push_back(16);
  threadsPerBlockArray.push_back(32);
  threadsPerBlockArray.push_back(64);
  threadsPerBlockArray.push_back(128);
  threadsPerBlockArray.push_back(256);
  threadsPerBlockArray.push_back(512);

  printf("performing calculations with cuda\n");
  // for each number of threads per block
  for (const unsigned int numberOfThreadsPerBlock :
         threadsPerBlockArray) {

    // perform cuda test
    const CudaTestFunctor cudaTestFunctor(integrationBounds,
                                          numberOfIntervals,
                                          numberOfThreadsPerBlock);
    double cudaElapsedTime;
    runTimingTestAndCheckAnswer(cudaTestFunctor,
                                numberOfRepeats,
                                numberOfExtraRepeats,
                                libraryAnswer,
                                &cudaElapsedTime);

    // output speedup
    printf("%3u : time %8.2e speedup %8.2e\n",
           numberOfThreadsPerBlock,
           cudaElapsedTime,
           serialElapsedTime / cudaElapsedTime);
  }

  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // ********************** </do cuda> *****************************
  // ===============================================================

  Kokkos::initialize();

  // ===============================================================
  // ********************** < do kokkos> ***************************
  // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

  printf("performing calculations with kokkos omp\n");
  {
    // perform kokkos omp test
    const KokkosTestFunctor<Kokkos::OpenMP> kokkosTestFunctor(integrationBounds,
                                                              numberOfIntervals);
    double kokkosElapsedTime;
    runTimingTestAndCheckAnswer(kokkosTestFunctor,
                                numberOfRepeats,
                                numberOfExtraRepeats,
                                libraryAnswer,
                                &kokkosElapsedTime);

    // output speedup
    printf("%s time %8.2e speedup %8.2e\n",
           "kokkos omp",
           kokkosElapsedTime,
           serialElapsedTime / kokkosElapsedTime);
  }

  printf("performing calculations with kokkos cuda\n");
  {
    // perform kokkos cuda test
    const KokkosTestFunctor<Kokkos::Cuda> kokkosTestFunctor(integrationBounds,
                                                            numberOfIntervals);
    double kokkosElapsedTime;
    runTimingTestAndCheckAnswer(kokkosTestFunctor,
                                numberOfRepeats,
                                numberOfExtraRepeats,
                                libraryAnswer,
                                &kokkosElapsedTime);

    // output speedup
    printf("%s time %8.2e speedup %8.2e\n",
           "kokkos cuda",
           kokkosElapsedTime,
           serialElapsedTime / kokkosElapsedTime);
  }

  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // ********************** </do kokkos> ***************************
  // ===============================================================

  Kokkos::finalize();

  return 0;
}
