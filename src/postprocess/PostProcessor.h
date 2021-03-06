#pragma once
#include <vector>
#include <iostream>
#include <cstdint>
#include <cmath>

namespace capsuleGene
{
    class Postprocessor
    {
    public:
        virtual ~Postprocessor(){};
        virtual std::vector<std::vector<double>> process(std::vector<std::vector<double>> &input);
    };
}