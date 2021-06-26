#include "SigmoidPostprocessor.h"

namespace capsuleGene
{
    std::vector<std::vector<double>> SigmoidPostprocessor::process(std::vector<std::vector<double>> &input)
    {
        std::vector<std::vector<double>> result = MathUtils::sigmoid(input);
        return result;
    };
};