#include "mathUtils.h"

namespace capsuleGene
{
    std::vector<std::vector<double>> MathUtils::sigmoid(const std::vector<std::vector<double>> &x)
    {
        const uint32_t N = x.size();
        const uint32_t M = x[0].size();
        uint32_t i, j;
        std::vector<std::vector<double>> output(N, std::vector<double>(M, 0.0));
#pragma omp parallel for private(i, j)
        for (i = 0; i < N; i++)
        {
            for (j = 0; j < M; j++)
            {
                output[i][j] = 1.0 / (1.0 + exp(-x[i][j]));
            }
        }
        return output;
    }

    std::vector<double> MathUtils::sigmoid(const std::vector<double> &x)
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

    double MathUtils::sigmoid(const double &x)
    {
        return 1.0 / (1.0 + exp(-x));
    }

    std::vector<std::vector<double>> MathUtils::softmax(const std::vector<std::vector<double>> &x)
    {
        const uint32_t N = x.size();
        const uint32_t M = x[0].size();
        double s;
        uint32_t i, j;
        std::vector<std::vector<double>> output(N, std::vector<double>(M, 0.0));

#pragma omp parallel for private(i, j, s)
        for (i = 0; i < N; i++)
        {
            s = 0;
            for (j = 0; j < M; j++)
            {
                s += exp(x[i][j]);
            };
            for (j = 0; j < M; j++)
            {
                output[i][j] = exp(x[i][j]) / s;
            };
        }
        return output;
    }
}