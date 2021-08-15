#include "../common.h"
#include "../ml/LogisticRegression.h"
#include "../ml/CoefLogisticRegression.h"
#include "../ml/MiharaLogisticRegression.hpp"

namespace capsuleGene
{
    class ServerSide
    {
    private:
        ML *ml;

    public:
        ServerSide(ML &ml);
        std::vector<std::vector<Ciphertext>> process(const std::vector<Ciphertext> &x);
        void load_weight(const std::string file_path);
        void load_bias(const std::string file_path);
    };
}