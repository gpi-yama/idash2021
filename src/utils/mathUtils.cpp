#include "mathUtils.h"

namespace capsuleGene
{
    std::vector<std::vector<double>> MathUtils::sigmoid(std::vector<std::vector<double>> &x)
    {
        const uint32_t N = x.size();
        const uint32_t M = x[0].size();
        uint32_t i, j;

        std::vector<std::vector<double>> output(N, std::vector<double>(M));
#pragma omp parallel for private(i, j)
        for (i = 0; i < N; ++i)
        {
            for (j = 0; j < M; j++)
            {
                output[i][j] = MathUtils::sigmoid(x[i][j]);
            }
        }
    }

    std::vector<double> MathUtils::sigmoid(std::vector<double> &x)
    {
        const uint32_t N = x.size();
        uint32_t i;

        std::vector<double> output(N);
#pragma omp parallel for private(i)
        for (i = 0; i < N; i++)
        {
            output[i] = MathUtils::sigmoid(x[i]);
        }
        return output;
    }

    double MathUtils::sigmoid(double &x)
    {
        return 1.0 / (1.0 + exp(-x));
    }
}