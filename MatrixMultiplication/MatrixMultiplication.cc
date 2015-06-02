// -*- C++ -*-
// MatrixMultiplication.cc
// an exercise for getting to know Kokkos
// here we do naive matrix multiplication

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
#include "MatrixMultiplication_serial.h"
#include "MatrixMultiplication_tbb.h"
#include "MatrixMultiplication_omp.h"
#include "MatrixMultiplication_cuda.h"
#include "MatrixMultiplication_kokkos.h"

// header file for kokkos so that i can specify device types here
#include <Kokkos_Core.hpp>

// header file so that i can set the number of threads for tbb
#include <tbb/task_scheduler_init.h>

// header file so that i can set the number of threads for omp
#include <omp.h>

// NOTE: all matrices in this program are a vector<double> and are assumed
//  to be in row-major order, though i guess it doesn't actually matter as long
//  as you're consistent

void
checkAnswer(const vector<double> & correctAnswer,
            const vector<double> & testAnswer,
            const string & testName) {
  if (correctAnswer.size() != testAnswer.size()) {
    fprintf(stderr, "%s answer has the wrong size: %zu instead of %zu\n",
            testName.c_str(), testAnswer.size(),
            correctAnswer.size());
    exit(1);
  }

  for (unsigned int entryIndex = 0;
       entryIndex < correctAnswer.size(); ++entryIndex) {
    if (std::abs(correctAnswer[entryIndex] -
                 testAnswer[entryIndex]) > 1e-4) {
      fprintf(stderr, "%s answer[%u] is wrong: %lf instead of %lf\n",
              testName.c_str(), entryIndex,
              testAnswer[entryIndex], correctAnswer[entryIndex]);
      exit(1);
    }
  }
}

template <class TestFunctor>
void
runTimingTest(const TestFunctor & testFunctor,
              const unsigned int numberOfRepeats,
              const unsigned int numberOfExtraRepeats,
              vector<double> * rowMajorAnswer,
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
    testFunctor.computeAnswer(rowMajorAnswer);

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
                            const vector<double> & correctAnswer,
                            double * elapsedTime) {

  // compute the answer and measure elapsed time
  vector<double> rowMajorAnswer;
  runTimingTest(testFunctor,
                numberOfRepeats,
                numberOfExtraRepeats,
                &rowMajorAnswer,
                elapsedTime);

  // check the answer
  checkAnswer(correctAnswer, rowMajorAnswer, testFunctor.getName());
}

int main() {

  // change the matrixSize to control the amount
  //  of work done.
  const unsigned int matrixSize = 512 * 3;
  // some controls for how many times the test is performed
  const unsigned int numberOfRepeats = 3;
  const unsigned int numberOfExtraRepeats = 1;

  // create a c++11 random number generator
  std::mt19937 randomNumberEngine;
  std::uniform_real_distribution<double> randomNumberGenerator(0, 1);

  vector<double> leftMatrix(matrixSize * matrixSize);
  vector<double> rightMatrix(matrixSize * matrixSize);
  for (unsigned int row = 0; row < matrixSize; ++row) {
    for (unsigned int col = 0; col < matrixSize; ++col) {
      leftMatrix[row * matrixSize + col] =
        randomNumberGenerator(randomNumberEngine);
      rightMatrix[row * matrixSize + col] =
        randomNumberGenerator(randomNumberEngine);
    }
  }

  // ===============================================================
  // ********************** < do serial> ***************************
  // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

  // perform serial test
  const SerialTestFunctor serialTestFunctor(leftMatrix,
                                            rightMatrix,
                                            matrixSize);
  vector<double> serialResultMatrix(matrixSize * matrixSize);
  double serialElapsedTime;
  runTimingTest(serialTestFunctor,
                numberOfRepeats,
                numberOfExtraRepeats,
                &serialResultMatrix,
                &serialElapsedTime);

  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // ********************** </do serial> ***************************
  // ===============================================================

  // we will repeat the computation for each of the numbers of threads
  vector<unsigned int> numberOfThreadsArray;
  numberOfThreadsArray.push_back(sysconf(_SC_NPROCESSORS_ONLN));

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
    const TbbTestFunctor tbbTestFunctor(leftMatrix,
                                        rightMatrix,
                                        matrixSize);
    double tbbElapsedTime;
    runTimingTestAndCheckAnswer(tbbTestFunctor,
                                numberOfRepeats,
                                numberOfExtraRepeats,
                                serialResultMatrix,
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
    const OmpTestFunctor ompTestFunctor(leftMatrix,
                                        rightMatrix,
                                        matrixSize);
    double ompElapsedTime;
    runTimingTestAndCheckAnswer(ompTestFunctor,
                                numberOfRepeats,
                                numberOfExtraRepeats,
                                serialResultMatrix,
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
  threadsPerBlockArray.push_back(256);

  printf("performing calculations with cuda\n");
  // for each number of threads per block
  for (const unsigned int numberOfThreadsPerBlock :
         threadsPerBlockArray) {

    // perform cuda test
    const CudaTestFunctor cudaTestFunctor(leftMatrix,
                                          rightMatrix,
                                          matrixSize,
                                          numberOfThreadsPerBlock);
    double cudaElapsedTime;
    runTimingTestAndCheckAnswer(cudaTestFunctor,
                                numberOfRepeats,
                                numberOfExtraRepeats,
                                serialResultMatrix,
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
    const KokkosTestFunctor<Kokkos::OpenMP> kokkosTestFunctor(leftMatrix,
                                                              rightMatrix,
                                                              matrixSize);
    double kokkosElapsedTime;
    runTimingTestAndCheckAnswer(kokkosTestFunctor,
                                numberOfRepeats,
                                numberOfExtraRepeats,
                                serialResultMatrix,
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
    const KokkosTestFunctor<Kokkos::Cuda> kokkosTestFunctor(leftMatrix,
                                                            rightMatrix,
                                                            matrixSize);
    double kokkosElapsedTime;
    runTimingTestAndCheckAnswer(kokkosTestFunctor,
                                numberOfRepeats,
                                numberOfExtraRepeats,
                                serialResultMatrix,
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
