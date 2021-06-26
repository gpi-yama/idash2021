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
        static std::vector<std::vector<double>> sigmoid(std::vector<std::vector<double>> &x);
        static std::vector<double> sigmoid(std::vector<double> &x);
        static double sigmoid(double &x);
    };
}