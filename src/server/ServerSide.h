#include "../common.h"
#include "../ml/ML.h"

namespace capsuleGene
{
    class ServerSide
    {
    private:
        ML ml;
        std::shared_ptr<Evaluator> evaluator;
        std::shared_ptr<Encryptor> encryptor;
        std::shared_ptr<CKKSEncoder> encoder;
        std::shared_ptr<GaloisKeys> gal_keys;
        std::shared_ptr<RelinKeys> rel_keys;

    public:
        ServerSide(ML &ml, std::shared_ptr<Evaluator> evaluator, std::shared_ptr<Encryptor> encryptor, std::shared_ptr<GaloisKeys> gal_keys, std::shared_ptr<RelinKeys> relKeys, std::shared_ptr<CKKSEncoder> encoder);
        std::vector<Ciphertext> process(std::vector<Ciphertext> &x);
    };
}