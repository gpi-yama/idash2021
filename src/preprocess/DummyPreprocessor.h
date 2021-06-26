#include "PreProcessor.h"

namespace capsuleGene
{
    class DummyPreprocessor : public Preprocessor
    {
    public:
        std::vector<std::vector<double>> process(std::vector<std::string> &input) override;
    };
}