// #include "PreProcessor.h"
#pragma once
#include <vector>
#include <string>

namespace capsuleGene
{
    class DummyPreprocessor //: public Preprocessor
    {
    public:
        DummyPreprocessor(){};
        ~DummyPreprocessor(){};
        std::vector<std::vector<double>> process(std::vector<std::string> &input); // override;
    };
}