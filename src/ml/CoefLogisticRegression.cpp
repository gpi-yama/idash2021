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
        bias.resize(output_dim, std::vector<double>(slot_size / 2, 0));
        weight.resize(output_dim, std::vector<double>(input_dim, 0.0));
    };

    void CoefLogisticRegression::prepare_w(std::vector<double> &w){
        std::reverse(w.begin(), w.end());
    }

    std::vector<double> CoefLogisticRegression::prepare_b(const double &b, const int pmd){
        std::vector<double> res(pmd);
#pragma omp parallel for
        for(int i=0; i<pmd; i++){
            res[i] = b;
        }
        return res;
    }


    Plaintext CoefLogisticRegression::encode_w(const std::vector<double> &w, const std::shared_ptr<CKKSEncoder> &encoder, const double scale){
        Plaintext res;
        encoder->encode_as_coeff(w, scale, res);
        return res;
    }

    Plaintext CoefLogisticRegression::encode_b(const std::vector<double> &b, const std::shared_ptr<CKKSEncoder> &encoder, const double scale){
        Plaintext res;
        encoder->encode_as_coeff(b, scale, res);
        return res;
    }

    std::vector<Ciphertext> CoefLogisticRegression::mult_xs_w(const std::vector<Ciphertext> &xs, const Plaintext &w, const std::shared_ptr<Evaluator> &evaluator, const std::shared_ptr<RelinKeys> &relin_keys){
        uint32_t i, n = xs.size();
        std::vector<Ciphertext> res(n);

#pragma omp parallel for
        for(i=0; i<n; i++){
            evaluator->multiply_plain(xs[i], w, res[i]);
            evaluator->relinearize_inplace(res[i], *relin_keys.get());
        }
        return res;
    }

    std::vector<Ciphertext> CoefLogisticRegression::add_xs_b(const std::vector<Ciphertext> &xs, Plaintext b, const std::shared_ptr<Evaluator> &evaluator, const std::shared_ptr<RelinKeys> &relin_keys){
        uint32_t i, n = xs.size();
        int scale_n;
        std::vector<Ciphertext> res(n);

#pragma omp parallel for private(scale_n)
        for(i=0; i<n; i++){
            scale_n = int(round(log2(xs[i].scale())));
            b.scale() = pow(2.0, scale_n);
            evaluator->add_plain(xs[i], b, res[i]);
        }
        return res;
    }

    std::vector<std::vector<Ciphertext>> CoefLogisticRegression::predict(const std::vector<Ciphertext> &x) {
        std::vector<std::vector<Ciphertext>> res_ctxt(output_dim);
        Plaintext plain_w;
        Plaintext plain_b;
        std::vector<Ciphertext> xs_w;
        uint32_t i;
#pragma omp parallel for private(plain_w, plain_b, xs_w)
        for (i = 0; i < output_dim; i++){
            plain_w = encode_w(this->weight[i], encoder, scale);
            plain_b = encode_b(this->bias[i], encoder, scale);
            xs_w = this->mult_xs_w(x, plain_w, evaluator, rel_keys);
            xs_w = this->add_xs_b(xs_w, plain_b, evaluator, rel_keys);
            res_ctxt[i] = xs_w;
        }
        return res_ctxt;
    }

    void CoefLogisticRegression::set_bias(const std::vector<float> &bias)
    {
        uint32_t i;
#pragma omp parallel for
        for (i = 0; i < output_dim; i++){
            this->bias[i] = this->prepare_b(bias[i], this->slot_size);
        }
    };

    void CoefLogisticRegression::set_weight(const std::vector<float> &weight)
    {
        uint32_t i, j;
#pragma omp parallel for
        for (i = 0; i < output_dim; i++){
            for (j = 0; j < input_dim; j++){
                this->weight[i][j] = weight[i*input_dim + j];
            }
        }

#pragma omp parallel for
        for (i = 0; i < output_dim; i++){
            CoefLogisticRegression::prepare_w(this->weight[i]);
        }
    }
}