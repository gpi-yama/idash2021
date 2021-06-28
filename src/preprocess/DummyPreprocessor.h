// #include "PreProcessor.h"
#pragma once
#include <vector>
#include <string>
#include "../utils/IOUtils.h"

namespace capsuleGene
{
    class DummyPreprocessor //: public Preprocessor
    {
    public:
        static std::vector<std::vector<double>> process(const std::vector<std::string> &input); // override;
    };
}