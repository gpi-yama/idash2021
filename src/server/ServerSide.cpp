#include "ServerSide.h"

namespace capsuleGene
{
    ServerSide::ServerSide(ML &ml, std::shared_ptr<Evaluator> evaluator, std::shared_ptr<Encryptor> encryptor, std::shared_ptr<GaloisKeys> gal_keys, std::shared_ptr<RelinKeys> rel_keys, std::shared_ptr<CKKSEncoder> encoder)
    {
        this->ml = ml;
        this->evaluator = evaluator;
        this->encryptor = encryptor;
        this->gal_keys = gal_keys;
        this->rel_keys = rel_keys;
    }

    std::vector<Ciphertext> ServerSide::process(std::vector<Ciphertext> &x)
    {
        return this->ml.predict(x);
    }
}