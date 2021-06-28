#pragma once
#include <vector>
#include <cmath>
#include <cstdint>
#include <omp.h>
#include <iostream>

namespace capsuleGene
{
    class MathUtils
    {
    public:
        static std::vector<std::vector<double>> sigmoid(const std::vector<std::vector<double>> &x);
        static std::vector<double> sigmoid(const std::vector<double> &x);
        static double sigmoid(const double &x);
    };
}