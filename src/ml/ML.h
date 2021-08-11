#pragma once
#include "../common.h"

namespace capsuleGene
{
    class ML
    {
    public:
        // ML(){};
        virtual ~ML(){};
        virtual std::vector<std::vector<Ciphertext>> predict(const std::vector<Ciphertext> &x){};
        virtual void set_bias(const std::vector<float> &bias){};
        virtual void set_weight(const std::vector<float> &weight){};
    };
}