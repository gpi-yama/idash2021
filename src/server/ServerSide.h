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
        ServerSide(LogisticRegression &ml, std::shared_ptr<Evaluator> evaluator, std::shared_ptr<Encryptor> encryptor, std::shared_ptr<GaloisKeys> gal_keys, std::shared_ptr<RelinKeys> relKeys, std::shared_ptr<CKKSEncoder> encoder);
        std::vector<std::vector<Ciphertext>> process(std::vector<Ciphertext> &x);
        void load_weight(std::string file_path);
        void load_bias(std::string file_path);
    };
}