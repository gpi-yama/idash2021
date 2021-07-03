#include "ServerSide.h"
#include "../utils/IOUtils.h"

namespace capsuleGene
{
    ServerSide::ServerSide(const LogisticRegression &ml, const std::shared_ptr<Evaluator> evaluator, const std::shared_ptr<Encryptor> encryptor, const std::shared_ptr<GaloisKeys> gal_keys, const std::shared_ptr<RelinKeys> rel_keyss, const std::shared_ptr<CKKSEncoder> encoder)
    {
        this->ml = ml;
        this->evaluator = evaluator;
        this->encryptor = encryptor;
        this->gal_keys = gal_keys;
        this->rel_keys = rel_keys;
        this->encoder = encoder;
    }

    std::vector<std::vector<Ciphertext>> ServerSide::process(const std::vector<Ciphertext> &x)
    {
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