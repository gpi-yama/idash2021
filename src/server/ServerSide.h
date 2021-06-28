#include "../common.h"
#include "../ml/LogisticRegression.h"
// #include "../ml/ML.h"

namespace capsuleGene
{
    class ServerSide
    {
    private:
        LogisticRegression ml;
        std::shared_ptr<Evaluator> evaluator;
        std::shared_ptr<Encryptor> encryptor;
        std::shared_ptr<CKKSEncoder> encoder;
        std::shared_ptr<GaloisKeys> gal_keys;
        std::shared_ptr<RelinKeys> rel_keys;

    public:
        ServerSide(const LogisticRegression &ml, const std::shared_ptr<Evaluator> evaluator, const std::shared_ptr<Encryptor> encryptor, const std::shared_ptr<GaloisKeys> gal_keys, const std::shared_ptr<RelinKeys> rel_keys, const std::shared_ptr<CKKSEncoder> encoder);
        std::vector<std::vector<Ciphertext>> process(const std::vector<Ciphertext> &x);
        void load_weight(const std::string file_path);
        void load_bias(const std::string file_path);
    };
}