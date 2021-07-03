#include "DummyPreprocessor.h"

namespace capsuleGene
{
    std::vector<std::vector<double>> DummyPreprocessor::process(const std::vector<std::string> &input)
    {
        std::vector<std::vector<double>> csv = IOUtils::read_csv("/home/yamaguchi/idash2021/inp_tests.csv", false, false);
        return csv;
    }
}