#pragma once
#include <vector>
#include <cmath>
#include <cstdint>
#include <omp.h>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <array>

namespace capsuleGene
{

    class MathUtils
    {
    public:
        static std::vector<std::vector<double>> sigmoid(const std::vector<std::vector<double>> &x);
        static std::vector<double> sigmoid(const std::vector<double> &x);
        static double sigmoid(const double &x);

        static std::vector<std::vector<double>> softmax(const std::vector<std::vector<double>> &x);

        static std::vector<std::string> parse_results(std::vector<std::vector<double>> &result, const int batch_size);
    };
}