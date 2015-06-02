// -*- C++ -*-
#ifndef MATRIXMULTIPLICATION_SERIAL_H
#define MATRIXMULTIPLICATION_SERIAL_H

#include "../Utilities.h"

class SerialTestFunctor {
public:

  static const CpuOrGpuType ProcessorType = Cpu;

  SerialTestFunctor(const vector<double> & leftMatrix,
                    const vector<double> & rightMatrix,
                    const unsigned int matrixSize) :
    _leftMatrix(leftMatrix),
    _rightMatrix(rightMatrix),
    _matrixSize(matrixSize) {

  }

  void
  computeAnswer(vector<double> * answer) const {

    vector<double> & resultMatrix = *answer;
    const unsigned int matrixSize = _matrixSize;
    resultMatrix.resize(matrixSize * matrixSize);

    for (unsigned int row = 0; row < matrixSize; ++row) {
      for (unsigned int col = 0; col < matrixSize; ++col) {
        double result = 0;
        for (unsigned int dummy = 0; dummy < matrixSize; ++dummy) {
          result +=
            _leftMatrix[row * matrixSize + dummy] *
            _rightMatrix[dummy * matrixSize + col];
        }
        resultMatrix[row * matrixSize + col] = result;
      }
    }
  }

  string
  getName() const {
    return string("serial");
  }

private:
  const vector<double> & _leftMatrix;
  const vector<double> & _rightMatrix;
  const unsigned int _matrixSize;
};

#endif // MATRIXMULTIPLICATION_SERIAL_H
