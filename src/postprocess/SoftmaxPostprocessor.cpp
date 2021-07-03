#include "SoftmaxPostprocessor.h"

namespace capsuleGene
{
    std::vector<std::vector<double>> SoftmaxPostprocessor::process(const std::vector<std::vector<double>> &input)
    {
        return MathUtils::softmax(input);
    };
};