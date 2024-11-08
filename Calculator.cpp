#include "Calculator.h"

#include <vector>
#include <limits>
#include <cstdint>

double Calculator::processVector(const std::vector<double>& vectorValues) {
    double sum = 0;
    double num = 0;
    double averageOfNumbers = 0;

    for (double value : vectorValues) {
        sum += value;
        num += 1;
    }
    averageOfNumbers = sum/num;

    return static_cast<double>(averageOfNumbers);
}