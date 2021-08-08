#include "CoefLogisticRegression.h"

namespace capsuleGene
{
    CoefLogisticRegression::CoefLogisticRegression(const uint32_t input_dim, const uint32_t output_dim, const double scale, const int slot_size, const std::shared_ptr<Evaluator> evaluator, const std::shared_ptr<CKKSEncoder> encoder, const std::shared_ptr<GaloisKeys> gal_keys, const std::shared_ptr<RelinKeys> rel_keys)
    {
        this->evaluator = evaluator;
        this->encoder = encoder;
        this->gal_keys = gal_keys;
        this->rel_keys = rel_keys;
        this->scale = scale;
        this->input_dim = input_dim;
        this->output_dim = output_dim;
        this->slot_size = slot_size;
        encoded_weight.resize(output_dim);
        bias.resize(output_dim, std::vector<double>(slot_size / 2, 0));
        weight.resize(output_dim, std::vector<double>(slot_size / 2, 0));
    };

    Plaintext CoefLogisticRegression::encode_w(const std::vector<double> &w, CKKSEncoder &encoder, const double scale){
        Plaintext res;
        encoder.encode_as_coeff(w, scale, res);
        return res;
    }

    std::vector<Ciphertext> CoefLogisticRegression::mult_xs_w(const std::vector<Ciphertext> &xs, const Plaintext &w, const Evaluator &evaluator, const RelinKeys &relin_keys){
        uint32_t n = xs.size();
        uint32_t i;
        std::vector<Ciphertext> res(n);
        Ciphertext ctxt;

#pragma omp parallel for private(ctxt)
        for(i=0; i<n; i++){
            evaluator.multiply_plain(xs[i], w, ctxt);
            res[i] = ctxt;
        }
        return res;
    }

    std::vector<Ciphertext> CoefLogisticRegression::add_xs_b(std::vector<Ciphertext> xs, Plaintext b, Evaluator &evaluator, RelinKeys relin_keys){
        std::vector<Ciphertext> res;
        int n = xs.size();
        for(int i=0; i<n; i++){
            Ciphertext tmp;

            int scale_n = int(round(log2(xs[i].scale())));
            b.scale() = pow(2.0, scale_n);

            evaluator.add_plain(xs[i], b, tmp);
            res.push_back(tmp);
        }
        return res;
    }

    std::vector<std::vector<Ciphertext>> CoefLogisticRegression::predict(const std::vector<Ciphertext> &x)
    {
        uint32_t i, j, k, size = x.size();
        std::vector<std::vector<Ciphertext>> result(size, std::vector<Ciphertext>(this->output_dim));
        std::vector<Ciphertext> tmp;

        std::vector<Plaintext> bias_coef(this->output_dim);
        for (i = 0; i < this->output_dim; i++){
            encoder.get()->encode_as_coeff(bias[i], this->scale, bias_coef[i]);
        }
        

#pragma omp parallel for
        for (j = 0; j < this->output_dim; j++)
        {
            result[j] = CoefLogisticRegression::mult_xs_w(x, this->encoded_weight[j], *this->evaluator.get(), *this->rel_keys.get());
            result[j] = CoefLogisticRegression::add_xs_b(result[j], bias_coef[j], *evaluator.get(), *rel_keys.get());
        }
        return result;
    };

    void CoefLogisticRegression::set_bias(std::vector<std::vector<double>> bias)
    {
        uint32_t i, j, k;
        uint32_t slot_per_feat = pow(2, int(log2(input_dim) + 1));

#pragma omp parallel for
        for (i = 0; i < output_dim; i++)
        {
            for (j = 0; j < int(slot_size / 2); j++)
            {
                this->bias[i][j] = bias[i][0];
            }
        }
    };

    void CoefLogisticRegression::set_weight(std::vector<std::vector<double>> weight)
    {
        uint32_t i, j, k;
        uint32_t slot_per_feat = pow(2, int(log2(input_dim) + 1));
        Plaintext plain_w;
        this->weight = weight;

#pragma omp parallel for private(plain_w)
        for (i = 0; i < output_dim; i++){
            plain_w = CoefLogisticRegression::encode_w(weight[i], *encoder.get(), scale);
            this->encoded_weight[i] = plain_w;
        }
    }
}