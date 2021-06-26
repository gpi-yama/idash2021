#include "LogisticRegression.h"

namespace capsuleGene
{
    LogisticRegression::LogisticRegression(uint32_t input_dim, uint32_t output_dim, double scale, std::shared_ptr<Evaluator> evaluator, std::shared_ptr<Encryptor> encryptor, std::shared_ptr<CKKSEncoder> encoder, std::shared_ptr<GaloisKeys> gal_keys, std::shared_ptr<RelinKeys> rel_keys)
    {
        this->evaluator = evaluator;
        this->encryptor = encryptor;
        this->encoder = encoder;
        this->gal_keys = gal_keys;
        this->rel_keys = rel_keys;
        this->scale = scale;
        this->input_dim = input_dim;
        this->output_dim = output_dim;
    };

    std::vector<Ciphertext> LogisticRegression::predict(std::vector<Ciphertext> &x)
    {
        uint32_t i, j, size = x.size();
        std::vector<Ciphertext> result(size);
        for (j = 0; j < this->output_dim; j++)
        {
#pragma omp parallel for
            for (i = 0; i < size; i++)
            {
                result[i] = AlgebraUtils::multiply(x[i], this->weight[j], this->evaluator, this->rel_keys, this->encoder, this->scale);
                result[i] = AlgebraUtils::rotate_and_sum_in_col(result[i], this->input_dim, this->evaluator, this->gal_keys, this->encoder, this->slot_size, this->scale);
            }
        }
        return result;
    };

    std::vector<std::vector<double>> LogisticRegression::sigmoid(std::vector<std::vector<double>> &x)
    {
        const uint32_t N = x.size();
        const uint32_t M = x[0].size();
        uint32_t i, j;

        std::vector<std::vector<double>> output(N, std::vector<double>(M));
#pragma omp parallel for private(i, j)
        for (i = 0; i < N; ++i)
        {
            for (j = 0; j < M; j++)
            {
                output[i][j] = 1.0 / (1.0 + exp(-x[i][j]));
            }
        }
    }

    Postprocessor LogisticRegression::gen_postprocessor()
    {
        Postprocessor postprocessor = SigmoidPostprocessor();
        return postprocessor;
    }
}