// -*- C++ -*-
// Histogram.cc
// an exercise for getting to know Kokkos
// here we do a histogram calculation over unsigned ints

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

// header file containing some uninteresting ickies
#include "../Utilities.h"

// header files for various implementations
#include "Histogram_serial.h"
#include "Histogram_tbb.h"
#include "Histogram_omp.h"
#include "Histogram_cuda.h"
#include "Histogram_kokkos.h"

// header file for kokkos so that i can specify device types here
#include <Kokkos_Core.hpp>

// header file so that i can set the number of threads for tbb
#include <tbb/task_scheduler_init.h>

// header file so that i can set the number of threads for omp
#include <omp.h>

void
checkAnswer(const vector<unsigned int> & correctAnswer,
            const vector<unsigned int> & testAnswer,
            const string & testName) {
  if (correctAnswer.size() != testAnswer.size()) {
    fprintf(stderr, "%s answer has the wrong size: %zu instead of %zu\n",
            testName.c_str(), testAnswer.size(), correctAnswer.size());
    exit(1);
  }

  for (unsigned int bucketIndex = 0;
       bucketIndex < correctAnswer.size(); ++bucketIndex) {
    if (testAnswer[bucketIndex] != correctAnswer[bucketIndex]) {
      fprintf(stderr, "%s answer[%u] is wrong: %u instead of %u\n",
              testName.c_str(), bucketIndex,
              testAnswer[bucketIndex], correctAnswer[bucketIndex]);
      exit(1);
    }
  }
}

template <class TestFunctor>
void
runTimingTest(const TestFunctor & testFunctor,
              const unsigned int numberOfRepeats,
              const unsigned int numberOfExtraRepeats,
              vector<unsigned int> * answer,
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
                            const vector<unsigned int> & correctAnswer,
                            double * elapsedTime) {

  // compute the answer and measure elapsed time
  vector<unsigned int> answer;
  runTimingTest(testFunctor,
                numberOfRepeats,
                numberOfExtraRepeats,
                &answer,
                elapsedTime);

  // check the answer
  checkAnswer(correctAnswer, answer, testFunctor.getName());
}

int main() {

  // change the numberOfElements to control the amount
  //  of work done.
  const unsigned int numberOfElements = 1e7;
  // The number of buckets in our histogram
  const unsigned int numberOfBuckets = 1e3;
  // some controls for how many times the test is performed
  const unsigned int numberOfRepeats = 3;
  const unsigned int numberOfExtraRepeats = 1;

  printf("Creating the input vector \n");
  vector<unsigned int> input(numberOfElements);
  std::iota(input.begin(), input.end(), 0);
  std::random_shuffle(input.begin(), input.end());

  // ===============================================================
  // ********************** < do serial> ***************************
  // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

  // perform serial test
  const SerialTestFunctor serialTestFunctor(input,
                                            numberOfBuckets);
  vector<unsigned int> serialHistogram;
  double serialElapsedTime;
  runTimingTest(serialTestFunctor,
                numberOfRepeats,
                numberOfExtraRepeats,
                &serialHistogram,
                &serialElapsedTime);

  const unsigned int bucketSize = numberOfElements / numberOfBuckets;
  for (unsigned int bucketIndex = 0;
       bucketIndex < numberOfBuckets; ++bucketIndex) {
    if (serialHistogram[bucketIndex] != bucketSize) {
      fprintf(stderr, "bucket %u has the wrong value: %u instead of %u\n",
              bucketIndex, serialHistogram[bucketIndex], bucketSize);
      exit(1);
    }
  }

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
    const TbbTestFunctor tbbTestFunctor(input,
                                        numberOfBuckets);
    double tbbElapsedTime;
    runTimingTestAndCheckAnswer(tbbTestFunctor,
                                numberOfRepeats,
                                numberOfExtraRepeats,
                                serialHistogram,
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
    const OmpTestFunctor ompTestFunctor(input,
                                        numberOfBuckets);
    double ompElapsedTime;
    runTimingTestAndCheckAnswer(ompTestFunctor,
                                numberOfRepeats,
                                numberOfExtraRepeats,
                                serialHistogram,
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
    const CudaTestFunctor cudaTestFunctor(input,
                                          numberOfBuckets,
                                          numberOfThreadsPerBlock);
    double cudaElapsedTime;
    runTimingTestAndCheckAnswer(cudaTestFunctor,
                                numberOfRepeats,
                                numberOfExtraRepeats,
                                serialHistogram,
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
    const KokkosTestFunctor<Kokkos::OpenMP> kokkosTestFunctor(input,
                                                              numberOfBuckets);
    double kokkosElapsedTime;
    runTimingTestAndCheckAnswer(kokkosTestFunctor,
                                numberOfRepeats,
                                numberOfExtraRepeats,
                                serialHistogram,
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
    const KokkosTestFunctor<Kokkos::Cuda> kokkosTestFunctor(input,
                                                            numberOfBuckets);
    double kokkosElapsedTime;
    runTimingTestAndCheckAnswer(kokkosTestFunctor,
                                numberOfRepeats,
                                numberOfExtraRepeats,
                                serialHistogram,
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
