#include "../common.h"
#include "ML.h"
#include <iostream>
#include <algorithm>
#include <cassert>
#include <string>
#include <fstream>
#include <chrono>



namespace capsuleGene
{
    class MiharaLogisticRegression: public ML
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

    std::vector<std::vector<double>> pp_xs(std::vector<std::vector<double>> xs, int l, int ls, int n, int dim, int bs){
    assert(xs.size() == bs);
    assert(xs[0].size() == dim);

    std::vector<std::vector<double>> res;
    for(int i=0; i<n-1; i++){
        std::vector<double> tmp;
        for(int j=0; j<l; j++){
        for(int k=0; k<dim; k++){
            tmp.push_back(xs[i*l+j][k]);
        }
        }
        res.push_back(tmp);
    }

    std::vector<double> tmp;
    for(int j=0; j<ls; j++){
        for(int k=0; k<dim; k++){
        tmp.push_back(xs[(n-1)*l+j][k]);
        }
    }
    res.push_back(tmp);
    assert(res.size()==n);
    return res;
    }


    std::vector<double> pp_w(std::vector<double> w, int dim){
    assert(w.size() == dim);
    std::reverse(w.begin(), w.end());
    return w;
    }


    std::vector<double> pp_b(double b, int pmd){
    std::vector<double> res(pmd);
    for(int i=0; i<pmd; i++){
        res[i] = b;
    }
    return res;
    }

    std::vector<Ciphertext> encode_encrypt_input(std::vector<std::vector<double>> xs, CKKSEncoder &encoder, Encryptor &encryptor, double scale, int n){
    std::vector<Plaintext> tmp;
    for(int i=0; i<n; i++){
        Plaintext tmp1;
        encoder.encode_as_coeff(xs[i], scale, tmp1);
        tmp.push_back(tmp1);
    }

    std::vector<Ciphertext> res;
    for(int i=0; i<n; i++){
        Ciphertext tmp1;
        encryptor.encrypt(tmp[i], tmp1);
        res.push_back(tmp1);
    }
    return res;
    }

    Plaintext encode_w(std::vector<double> w, CKKSEncoder &encoder, double scale){
    Plaintext res;
    encoder.encode_as_coeff(w, scale, res);
    return res;
    }

    Plaintext encode_b(std::vector<double> b, CKKSEncoder &encoder, double scale){
    Plaintext res;
    encoder.encode_as_coeff(b, scale, res);
    return res;
    }

    std::vector<Ciphertext> mult_xs_w(std::vector<Ciphertext> xs, Plaintext w, Evaluator &evaluator, RelinKeys relin_keys, int n){
    std::vector<Ciphertext> res;
    for(int i=0; i<n; i++){
        Ciphertext tmp;
        evaluator.multiply_plain(xs[i], w, tmp);
        evaluator.relinearize_inplace(tmp, relin_keys);

        res.push_back(tmp);
    }
    return res;
    }

    std::vector<Ciphertext> add_xs_b(std::vector<Ciphertext> xs, Plaintext b, Evaluator &evaluator, RelinKeys relin_keys, int n){
    std::vector<Ciphertext> res;
    for(int i=0; i<n; i++){
        Ciphertext tmp;

        int scale_n = int(round(log2(xs[i].scale())));
        b.scale() = pow(2.0, scale_n);

        evaluator.add_plain(xs[i], b, tmp);
        res.push_back(tmp);
    }
    return res;
    }


    std::vector<std::vector<double>> decrypt_decode_res(std::vector<Ciphertext> xs, CKKSEncoder &encoder, Decryptor &decryptor, int n){
    std::vector<Plaintext> tmp;
    std::vector<std::vector<double>> res;

    for(int i=0; i<n; i++){
        Plaintext tmp1;
        decryptor.decrypt(xs[i], tmp1);
        tmp.push_back(tmp1);
    }

    for(int i=0; i<n; i++){
        std::vector<double> tmp1;
        encoder.decode_as_coeff(tmp[i], tmp1);
        res.push_back(tmp1);
    }
    return res;
    }

    std::vector<double> psp_res(std::vector<std::vector<double>> xs, int l, int ls, int n, int dim, int bs){
    assert(xs.size() == n);
    assert(n > 0);

    std::vector<double> res;
    for(int i=0; i<n-1; i++){
        for(int j=0; j<l; j++){
        res.push_back(xs[i][dim-1+dim*j]);
        }
    }
    for(int j=0; j<ls; j++){
        res.push_back(xs[n-1][dim-1+dim*j]);
    }
    return res;
    }


    std::vector<double> debug(std::vector<std::vector<double>> xs, std::vector<double> w, double b, int dim, int bs){
    assert(xs.size() == bs);
    assert(xs[0].size() == dim);
    assert(w.size() == dim);

    std::vector<std::vector<double>> tmp1;
    for(int i=0; i<bs; i++){
        std::vector<double> tmp2;
        for(int j=0; j<dim; j++){
        tmp2.push_back(xs[i][j]*w[j]);
        }
        tmp1.push_back(tmp2);
    }

    std::vector<double> res;
    for(int i=0; i<bs; i++){
        double tmp3 = 0;
        for(int j=0; j<dim; j++){
        tmp3 += tmp1[i][j];
        }
        res.push_back(tmp3+b);
    }

    return res;
    }


    std::vector<std::vector<double>> test_slice(std::vector<std::vector<double>> x, int size){
        std::vector<std::vector<double>> tmp;
        for(int i=0; i<size; i++){
        tmp.push_back(x[i]);
        }
        return tmp;
    }


    public:
        ~MiharaLogisticRegression()
        {
        }
        MiharaLogisticRegression(const uint32_t input_dim, const uint32_t output_dim, const double scale, const int slot_size, const std::shared_ptr<Evaluator> evaluator, const std::shared_ptr<CKKSEncoder> encoder, const std::shared_ptr<GaloisKeys> gal_keys, const std::shared_ptr<RelinKeys> rel_keys){
            this->evaluator = evaluator;
            this->encoder = encoder;
            this->gal_keys = gal_keys;
            this->rel_keys = rel_keys;
            this->scale = scale;
            this->input_dim = input_dim;
            this->output_dim = output_dim;
            this->slot_size = slot_size;
            bias.resize(output_dim, std::vector<double>(slot_size / 2, 0.0));
            weight.resize(output_dim, std::vector<double>(input_dim, 0.0));
        };

        std::vector<std::vector<Ciphertext>> predict(const std::vector<Ciphertext> &x) override {
            std::vector<std::vector<Ciphertext>> res_ctxt;
            int i, n = x.size();
            for (i = 0; i < 4; i++){
                Plaintext plain_w = encode_w(this->weight[i], *encoder.get(), scale);
                Plaintext plain_b = encode_b(this->bias[i], *encoder.get(), scale);
                std::vector<Ciphertext> xs_w = mult_xs_w(x, plain_w, *evaluator.get(), *rel_keys.get(), n);
                xs_w = this->add_xs_b(xs_w, plain_b, *evaluator.get(), *rel_keys.get(), n);
                res_ctxt.push_back(xs_w);
            }
            return res_ctxt;
        }

        // setter
        void set_bias(const std::vector<float> &bias) override {
            int i;
            for (i = 0; i < output_dim; i++){
                this->bias[i] = this->pp_b(bias[i], this->slot_size);
            }
        }
        void set_weight(const std::vector<float> &weight) override {
            int i, j;
            for (i = 0; i < output_dim; i++){
                for (j = 0; j < input_dim; j++){
                    this->weight[i][j] = weight[i*input_dim + j];
                }
            }

            for (i = 0; i < output_dim; i++){
                this->weight[i] = this->pp_w(this->weight[i], input_dim);
            }
        }
    };
}