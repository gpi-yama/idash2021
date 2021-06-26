#include "SigmoidPostprocessor.h"

namespace capsuleGene
{
    std::vector<std::vector<double>> SigmoidPostprocessor::process(std::vector<std::vector<double>> &input)
    {
        return MathUtils::sigmoid(input);
    };
};