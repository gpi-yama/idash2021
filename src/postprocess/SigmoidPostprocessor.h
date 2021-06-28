#pragma once
// #include "PostProcessor.h"
#include "../utils/mathUtils.h"
#include <iostream>

namespace capsuleGene
{
    class SigmoidPostprocessor // : public Postprocessor
    {
    public:
        static std::vector<std::vector<double>> process(const std::vector<std::vector<double>> &input); //override;
    };
}
