#pragma once
#include <vector>
#include <iostream>
#include <cstdint>
#include <cmath>

namespace capsuleGene
{
    class Preprocessor
    {
    public:
        virtual ~Preprocessor(){};
        virtual std::vector<std::vector<double>> process(std::vector<std::string> &input);
    };
}