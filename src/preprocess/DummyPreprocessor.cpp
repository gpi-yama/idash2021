#include "DummyPreprocessor.h"

namespace capsuleGene
{
    std::vector<std::vector<double>> DummyPreprocessor::process(const std::vector<std::string> &input)
    {
        return IOUtils::read_csv("inp_tests.csv", false, false);
    }
}