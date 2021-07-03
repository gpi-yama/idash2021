#include "LogisticRegression.h"

namespace capsuleGene
{
    LogisticRegression::LogisticRegression(const uint32_t input_dim, const uint32_t output_dim, const double scale, const int slot_size, const std::shared_ptr<Evaluator> evaluator, const std::shared_ptr<Encryptor> encryptor, const std::shared_ptr<CKKSEncoder> encoder, const std::shared_ptr<GaloisKeys> gal_keys, const std::shared_ptr<RelinKeys> rel_keys)
    {
        this->evaluator = evaluator;
        this->encryptor = encryptor;
        this->encoder = encoder;
        this->gal_keys = gal_keys;
        this->rel_keys = rel_keys;
        this->scale = scale;
        this->input_dim = input_dim;
        this->output_dim = output_dim;
        this->slot_size = slot_size;
        weight.resize(output_dim, std::vector<double>(input_dim * 2, 0));
    };

    std::vector<std::vector<Ciphertext>> LogisticRegression::predict(const std::vector<Ciphertext> &x)
    {
        uint32_t i, j, size = x.size();
        std::vector<std::vector<Ciphertext>> result(size);
#pragma omp parallel for private(i, j)
        for (i = 0; i < size; i++)
        {
            std::vector<Ciphertext> likelihood(this->output_dim);
            for (j = 0; j < this->output_dim; j++)
            {
                likelihood[j] = AlgebraUtils::multiply(x[i], this->weight[j], this->evaluator, this->rel_keys, this->encoder, this->scale);
                likelihood[j] = AlgebraUtils::rotate_and_sum_in_col(likelihood[j], this->input_dim, this->evaluator, this->gal_keys, this->encoder, this->slot_size, this->scale);
                likelihood[j] = AlgebraUtils::add(likelihood[j], bias[j], this->evaluator, this->encoder, this->scale);
            }
            result[i] = likelihood;
        }
        return result;
    };

    void LogisticRegression::set_bias(std::vector<std::vector<double>> bias)
    {
        this->bias = bias;
    };

    void LogisticRegression::set_weight(std::vector<std::vector<double>> weight)
    {
        uint32_t i, j;

#pragma omp parallel for
        for (i = 0; i < output_dim; i++)
        {
            for (j = 0; j < input_dim; j++)
            {
                this->weight[i][j] = weight[i][j];
            }
        }
    }

    void LogisticRegression::set_decryptor(std::shared_ptr<Decryptor> decryptor)
    {
        this->decryptor = decryptor;
    }
}