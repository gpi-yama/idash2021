#include "algebra.h"
#include "../postprocess/SigmoidPostprocessor.h"

namespace capsuleGene
{
    class CoefLogisticRegression
    {
    private:
        std::vector<std::vector<double>> weight;
        std::vector<Plaintext> encoded_weight;
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

        static Plaintext encode_w(const std::vector<double> &w, CKKSEncoder &encoder, const double scale);
        static std::vector<Ciphertext> mult_xs_w(const std::vector<Ciphertext> &xs, const Plaintext &w, const Evaluator &evaluator, const RelinKeys &relin_keys);

    public:
        CoefLogisticRegression(){};
        ~CoefLogisticRegression()
        {
        }
        CoefLogisticRegression(const uint32_t input_dim, const uint32_t output_dim, const double scale, const int slot_size, const std::shared_ptr<Evaluator> evaluator, const std::shared_ptr<CKKSEncoder> encoder, const std::shared_ptr<GaloisKeys> gal_keys, const std::shared_ptr<RelinKeys> rel_keys);
        std::vector<std::vector<Ciphertext>> predict(const std::vector<Ciphertext> &x);

        // setter
        void set_bias(const std::vector<std::vector<double>> bias);
        void set_weight(const std::vector<std::vector<double>> weight);
    };
}
