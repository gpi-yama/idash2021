#include "DummyPreprocessor.h"

namespace capsuleGene
{
    std::vector<std::vector<double>> DummyPreprocessor::process(const std::vector<std::string> &input)
    {
        return IOUtils::read_csv("/home/yamaguchi/idash2021/inp_tests.csv", false, false);
    }
}