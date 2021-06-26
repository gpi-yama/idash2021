#include "DummyPreprocessor.h"

namespace capsuleGene
{
    std::vector<std::vector<double>> DummyPreprocessor::process(std::vector<std::string> &input)
    {
        std::vector<std::vector<double>> feature(10, std::vector<double>(100, 1.0));
        return feature;
    }
}