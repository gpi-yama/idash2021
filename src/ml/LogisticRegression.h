#include "algebra.h"
#include "../postprocess/SigmoidPostprocessor.h"

namespace capsuleGene
{
    class LogisticRegression
    {
    private:
        std::vector<std::vector<double>> weight;
        std::vector<std::vector<double>> bias;
        uint32_t output_dim;
        uint32_t input_dim;
        std::shared_ptr<Evaluator> evaluator;
        std::shared_ptr<Encryptor> encryptor;
        std::shared_ptr<CKKSEncoder> encoder;
        std::shared_ptr<GaloisKeys> gal_keys;
        std::shared_ptr<RelinKeys> rel_keys;
        double scale;
        int slot_size;

    public:
        LogisticRegression(){};
        ~LogisticRegression()
        {
        }
        LogisticRegression(const uint32_t input_dim, const uint32_t output_dim, const double scale, const int slot_size, const std::shared_ptr<Evaluator> evaluator, const std::shared_ptr<CKKSEncoder> encoder, const std::shared_ptr<GaloisKeys> gal_keys, const std::shared_ptr<RelinKeys> rel_keys);
        std::vector<std::vector<Ciphertext>> predict(const std::vector<Ciphertext> &x);

        // setter
        void set_bias(const std::vector<float> bias);
        void set_weight(const std::vector<float> weight);
    };
}
