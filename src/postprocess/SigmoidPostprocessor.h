#include "PostProcessor.h"
#include "../utils/mathUtils.h"

namespace capsuleGene
{
    class SigmoidPostprocessor : public Postprocessor
    {
    public:
        std::vector<std::vector<double>> process(std::vector<std::vector<double>> &input) override;
    };
}
