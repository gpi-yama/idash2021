#include "../common.h"
#include "../preprocess/DummyPreprocessor.h"
#include "../postprocess/SoftmaxPostprocessor.h"
#include "../preprocess/PreProcessor.h"

namespace capsuleGene
{
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
        Preprocessor preprocessor;

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

        static Plaintext encode_as_coef(std::vector<double> x, CKKSEncoder &encoder, double scale);

        static std::vector<Ciphertext> encrypt_as_coef(std::vector<std::vector<double>> xs, CKKSEncoder &encoder, Encryptor &encryptor, double scale);

        static std::vector<double> decode_as_coef(Plaintext x, CKKSEncoder &encoder, double scale);

        static std::vector<std::vector<double>> decrypt_as_coef(std::vector<Ciphertext>  xs, CKKSEncoder &encoder, Decryptor &decryptor, double scale, int n);

        static std::vector<std::vector<double>> pack_vector_for_coef(std::vector<std::vector<double>> xs, int l, int ls, int n);

        static std::vector<double> unpack_vector_for_coef(std::vector<std::vector<double>> xs, int l, int ls, int n, int dim, int bs);

    public:
        /**
         * @brief encrypt inptu matrix
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
        std::vector<Ciphertext> batch_encrypt(const std::vector<std::vector<double>> &input);

        /**
         * @brief preprocess
         * 
         * @param input vector of strings
         * @return std::vector<std::vector<double>> 
         */
        std::vector<std::vector<double>> preprocess(const std::string input_path);

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
        ClientSide(){};

        /**
         * @brief Construct a new Client Side object with setting preprocessor
         * 
         * @param preprocessor 
         */
        ClientSide(Preprocessor preprocessor){
            this->preprocessor = preprocessor;
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
         * @brief Client side preprocessing
         * 
         * @param input path
         * @return std::vector<Ciphertext> 
         */
        std::vector<Ciphertext> process(const std::string input_path);

        /**
         * @brief Client side preprocessing with coef encoding
         * 
         * @param input_path
         * @return std::vector<Ciphertext> 
         */
        std::vector<Ciphertext> process_as_coef(const std::string input_path);

        std::vector<std::vector<double>> postprocess(const std::vector<std::vector<Ciphertext>> &x);

        /**
         * @brief  Client side postprocessing with coef encoding
         * 
         * @param x 
         * @return std::vector<std::vector<double>> 
         */
        std::vector<std::vector<double>> postprocess_as_coef(const std::vector<Ciphertext> &x);

        // Getter
        std::shared_ptr<Evaluator> getEvaluator();
        std::shared_ptr<Encryptor> getEncryptor();
        std::shared_ptr<Decryptor> getDecryptor();
        std::shared_ptr<CKKSEncoder> getEncoder();
        std::shared_ptr<PublicKey> getPublicKey();
        std::shared_ptr<SecretKey> getSecretKey();
        std::shared_ptr<GaloisKeys> getGalKey();
        std::shared_ptr<RelinKeys> getRelinKeys();
    };
}