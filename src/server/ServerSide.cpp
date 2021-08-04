#include "ServerSide.h"
#include "../utils/IOUtils.h"

namespace capsuleGene
{
    ServerSide::ServerSide(const LogisticRegression &ml)
    {
        this->ml = ml;
    }

    ServerSide::ServerSide(const CoefLogisticRegression &ml)
    {
        this->use_coef = true;
        this->ml_coef = ml;
    }

    std::vector<std::vector<Ciphertext>> ServerSide::process(const std::vector<Ciphertext> &x)
    {
        if (this->use_coef) return this->ml_coef.predict(x);
        return this->ml.predict(x);
    }

    void ServerSide::load_weight(const std::string file_path)
    {
        std::vector<std::vector<double>> weight = IOUtils::read_csv(file_path, false, false);
        this->ml.set_weight(IOUtils::read_csv(file_path, false, false));
    }

    void ServerSide::load_bias(const std::string file_path)
    {
        std::vector<std::vector<double>> csv = IOUtils::read_csv(file_path, false, false);
        this->ml.set_bias(csv);
    }
}