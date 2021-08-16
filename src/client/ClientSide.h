#pragma once
#include "../common.h"
#include "../postprocess/SoftmaxPostprocessor.h"
#include "../preprocess/PreProcessor.h"

namespace capsuleGene
{

    // struct for coefficient logistic regression
    class CoefParams{
        public:
            int l;
            int n;
            int nt;
            int ls;
            int dim;
            int bs;

        public:
            CoefParams(){};
            CoefParams(int dim, int bs, int poly_modulus_degree){
                this->bs = bs;
                this->dim = dim;
                l = std::min(poly_modulus_degree/2/2/dim, bs);
                n = bs/l;
                nt = bs%l;
                if(nt!=0) n += 1; // n ctxts in total
                ls = l;
                if(nt!=0) ls = nt; // last ctxt has ls of x
            }
    };

    class ClientSide
    {
    private:
        std::shared_ptr<Evaluator> evaluator;
        std::shared_ptr<Encryptor> encryptor;
        std::shared_ptr<Decryptor> decryptor;
        std::shared_ptr<PublicKey> public_key;
        std::shared_ptr<SecretKey> secret_key;
        std::shared_ptr<GaloisKeys> gal_keys;
        std::shared_ptr<RelinKeys> relin_keys;
        std::shared_ptr<CKKSEncoder> encoder;
        double scale;
        int poly_modulus_degree;
        std::vector<int32_t> modulus_chain;
        uint32_t slot_per_feat;
        CoefParams coef_params;

    private:
        /**
         * @brief generate EncryptionParameters
         * 
         * @param scale 
         * @param modulus_chain 
         * @param poly_modulus_degree 
         * @return EncryptionParameters 
         */
        static EncryptionParameters generateParameters(const double scale, const std::vector<int32_t> &modulus_chain, const int poly_modulus_degree);

        /**
         * @brief coeficinet level encoding
         * 
         * @param x : vector of doulbe that you want to encrypt
         * @param encoder : ckks encoder
         * @param scale : scale of ctxt
         * @return Plaintext 
         */
        static Plaintext encode_as_coef(const std::vector<double> &x, const std::shared_ptr<CKKSEncoder> &encoder, const double scale);

        /**
         * @brief coeficient lebel encryption
         * 
         * @param xs : 2d vector thatis bached double vector you want to encrypt: vector of (Batch_size, number_of_features)
         * @param encoder : ckks encoder
         * @param encryptor : ckks encryptor 
         * @param scale : scale of ctxt
         * @return std::vector<Ciphertext> 
         */
        static std::vector<Ciphertext> encrypt_as_coef(const std::vector<std::vector<double>> &xs, const std::shared_ptr<CKKSEncoder> &encoder, const std::shared_ptr<Encryptor> &encryptor, const double scale);

        /**
         * @brief coefficient level decoding
         * 
         * @param x : Plaintext
         * @param encoder : ckks encoder
         * @param scale : ctxt scale
         * @return std::vector<double> : decoded result
         */
        static std::vector<double> decode_as_coef(const Plaintext &x, const std::shared_ptr<CKKSEncoder> &encoder, const double scale);

        /**
         * @brief coefficinet level decryption
         * 
         * @param xs : batched Ciphertext)
         * @param encoder : ckks encoder
         * @param decryptor : ckks decryptor
         * @param scale : ctxt scale
         * @return std::vector<std::vector<double>> 
         */
        static std::vector<std::vector<double>> decrypt_as_coef(const std::vector<Ciphertext> &xs, const std::shared_ptr<CKKSEncoder> &encoder, const std::shared_ptr<Decryptor> &decryptor, const double scale);

        /**
         * @brief pack to single vector for efficient coefficient level logistic regression
         * 
         * @param xs : vector of (Batch_size, number_of_features)
         * @param l : CoefParams.l
         * @param ls : CoefParams.ls
         * @param n : CoefParams.n
         * @return std::vector<std::vector<double>> 
         */
        static std::vector<std::vector<double>> pack_vector_for_coef(const std::vector<std::vector<float>> &xs, const int l, const int ls, const int n);

        /**
         * @brief unpack vector for efficient coefficient level logistic regression
         * 
         * @param xs : decrypted result
         * @param l : CoefParams.l
         * @param ls : CoefParams.ls
         * @param n : CoefParams.n
         * @param dim : CoefParams.dim
         * @param bs : CoefParams.bs
         * @return std::vector<double> 
         */
        static std::vector<double> unpack_vector_for_coef(const std::vector<std::vector<double>> &xs, const int l, const int ls, const int n, const int dim, const int bs);

    public:
        /**
         * @brief encrypt input matrix
         * 
         * @param input matrix
         * @return std::vector<Ciphertext> 
         */
        std::vector<Ciphertext> encrypt(const std::vector<std::vector<double>> &input);

        /**
         * @brief encrypt input vector
         * 
         * @param input vector
         * @return std::vector<Ciphertext> 
         */
        Ciphertext encrypt(const std::vector<double> &input);

        /**
         * @brief encrypt and pack feature into the same ctxt until slot_size
         * 
         * @param input : 2d vector
         * @return std::vector<Ciphertext> 
         */
        std::vector<Ciphertext> batch_encrypt(const std::vector<std::vector<float>> &input);

        /**
         * @brief encrypt and pack features as coef 
         * 
         * @param input : 2d vector
         * @return std::vector<Ciphertext> 
         */
        std::vector<Ciphertext> coef_encrypt(const std::vector<std::vector<float>> &input);

        /**
         * @brief decrypt ciphertext vector
         * 
         * @param x 
         * @return std::vector<double>
         */
        std::vector<double> decrypt(const std::vector<Ciphertext> &x);

        /**
         * @brief decrypt ciphertext
         * 
         * @param x 
         * @return std::vector<double>
         */
        std::vector<double> decrypt(const Ciphertext &x);

        /**
         * @brief batch decrypt
         * 
         * @param x 
         * @return std::vector<double> 
         */
        std::vector<std::vector<double>> batch_decrypt(const std::vector<std::vector<Ciphertext>> &x);

        /**
         * @brief Construct a new Client Side object
         */
        ClientSide(){
        };

        /**
         * @brief generate keys and evaluators and set as private variables
         * 
         * @param scale 
         * @param modulus_chain 
         * @param poly_modulus_degree 
         */
        void generate_keys(const double scale, const std::vector<int32_t> &modulus_chain, const int poly_modulus_degree);

        /**
         * @brief Client side postprocessing
         * 
         * @param x 
         * @return std::vector<std::vector<double>> 
         */
        std::vector<std::vector<double>> postprocess(const std::vector<std::vector<Ciphertext>> &x);

        /**
         * @brief Client side postprocessing with coef encoding
         * 
         * @param x 
         * @return std::vector<std::vector<double>> 
         */
        std::vector<std::vector<double>> postprocess_as_coef(const std::vector<std::vector<Ciphertext>> &x);

    // Getter
    public:
        std::shared_ptr<Evaluator> getEvaluator();
        std::shared_ptr<Encryptor> getEncryptor();
        std::shared_ptr<CKKSEncoder> getEncoder();
        std::shared_ptr<PublicKey> getPublicKey();
        std::shared_ptr<GaloisKeys> getGalKey();
        std::shared_ptr<RelinKeys> getRelinKeys();

    private:
        std::shared_ptr<Decryptor> getDecryptor();
        std::shared_ptr<SecretKey> getSecretKey();
    };
}