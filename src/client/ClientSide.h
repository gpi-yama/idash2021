#include "../common.h"
#include "../preprocess/DummyPreprocessor.h"
#include "../postprocess/SoftmaxPostprocessor.h"
// #include "../preprocess/PreProcessor.h"

namespace capsuleGene
{
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
        static std::vector<std::vector<double>> preprocess(const std::vector<std::string> &input);

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
         * @param input batched input vector
         * @return std::vector<Ciphertext> 
         */
        std::vector<Ciphertext> process(const std::vector<std::string> &input);

        std::vector<std::vector<double>> postprocess(const std::vector<std::vector<Ciphertext>> &x);

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