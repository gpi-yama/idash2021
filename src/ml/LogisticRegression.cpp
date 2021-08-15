#include "LogisticRegression.h"

namespace capsuleGene
{
    LogisticRegression::LogisticRegression(const uint32_t input_dim, const uint32_t output_dim, const double scale, const int slot_size, const std::shared_ptr<Evaluator> evaluator, const std::shared_ptr<CKKSEncoder> encoder, const std::shared_ptr<GaloisKeys> gal_keys, const std::shared_ptr<RelinKeys> rel_keys)
    {
        this->evaluator = evaluator;
        this->encoder = encoder;
        this->gal_keys = gal_keys;
        this->rel_keys = rel_keys;
        this->scale = scale;
        this->input_dim = input_dim;
        this->output_dim = output_dim;
        this->slot_size = slot_size;
        bias.resize(output_dim, std::vector<double>(slot_size / 2, 0));
        weight.resize(output_dim, std::vector<double>(slot_size / 2, 0));
    };

    std::vector<std::vector<Ciphertext>> LogisticRegression::predict(const std::vector<Ciphertext> &x)
    {
        uint32_t i, j, size = x.size();
        std::vector<std::vector<Ciphertext>> result(size, std::vector<Ciphertext>(this->output_dim));
        Ciphertext tmp;
        std::cout << "pred" << std::endl;
#pragma omp parallel for private(i, j, tmp)
        for (i = 0; i < size; i++)
        {
            for (j = 0; j < this->output_dim; j++)
            {
                tmp = AlgebraUtils::multiply(x[i], this->weight[j], this->evaluator, this->rel_keys, this->encoder, this->scale);
                tmp = AlgebraUtils::rotate_and_sum_in_col(tmp, this->input_dim, this->evaluator, this->gal_keys);
                // tmp = AlgebraUtils::add(tmp, bias[j], this->evaluator, this->encoder);
                result[i][j] = tmp;
            }
        }
        // output shape is (Batch, output_dim, (each_ctxt: it is encrypted vec and wherein the corresponiding value is result))
        return result;
    };

    void LogisticRegression::set_bias(const std::vector<float> &bias)
    {
        uint32_t i, j, k;
        uint32_t slot_per_feat = pow(2, int(log2(input_dim) + 1));

#pragma omp parallel for
        for (i = 0; i < output_dim; i++)
        {
            for (j = 0; j < int(slot_size / 2 / slot_per_feat); j++)
            {
                this->bias[i][j * slot_per_feat] = bias[i];
            }
        }
    };

    void LogisticRegression::set_weight(const std::vector<float> &weight)
    {
        uint32_t i, j, k;
        uint32_t slot_per_feat = pow(2, int(log2(input_dim) + 1));

#pragma omp parallel for
        for (i = 0; i < output_dim; i++)
        {
            for (j = 0; j < int(slot_size / 2 / slot_per_feat); j++)
            {
                for (k = 0; k < input_dim; k++)
                {
                    this->weight[i][k + j * slot_per_feat] = weight[i*input_dim + k];
                }
            }
        }
    }
}