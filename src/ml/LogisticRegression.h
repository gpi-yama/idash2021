// #include "ML.h"
#include "algebra.h"
#include "../postprocess/SigmoidPostprocessor.h"

namespace capsuleGene
{
    class LogisticRegression // : public ML
    {
    private:
        std::vector<std::vector<double>> weight;
        std::vector<double> bias;
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
        LogisticRegression(uint32_t input_dim, uint32_t output_dim, double scale, std::shared_ptr<Evaluator> evaluator, std::shared_ptr<Encryptor> encryptor, std::shared_ptr<CKKSEncoder> encoder, std::shared_ptr<GaloisKeys> gal_keys, std::shared_ptr<RelinKeys> rel_keys);
        std::vector<std::vector<Ciphertext>> predict(std::vector<Ciphertext> &x); //override;
        static std::vector<std::vector<double>> sigmoid(std::vector<std::vector<double>> &x);
        // Postprocessor gen_postprocessor() override;
        SigmoidPostprocessor gen_postprocessor();

        // setter
        void set_bias(std::vector<double> bias);
        void set_weight(std::vector<std::vector<double>> weight);
    };
}
