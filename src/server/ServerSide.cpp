#include "ServerSide.h"
#include "../utils/IOUtils.h"

namespace capsuleGene
{
    ServerSide::ServerSide(LogisticRegression &ml, std::shared_ptr<Evaluator> evaluator, std::shared_ptr<Encryptor> encryptor, std::shared_ptr<GaloisKeys> gal_keys, std::shared_ptr<RelinKeys> relKeys, std::shared_ptr<CKKSEncoder> encoder)
    {
        this->ml = ml;
        this->evaluator = evaluator;
        this->encryptor = encryptor;
        this->gal_keys = gal_keys;
        this->rel_keys = rel_keys;
        this->encoder = encoder;
    }

    std::vector<std::vector<Ciphertext>> ServerSide::process(std::vector<Ciphertext> &x)
    {
        return this->ml.predict(x);
    }

    void ServerSide::load_weight(std::string file_path)
    {
        this->ml.set_weight(IOUtils::read_csv(file_path, false, false));
    }

    void ServerSide::load_bias(std::string file_path)
    {
        std::vector<std::vector<double>> csv = IOUtils::read_csv(file_path, false, false);
        this->ml.set_bias(csv[0]);
    }
}