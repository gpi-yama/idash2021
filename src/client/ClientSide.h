#include "../common.h"
#include "../preprocess/PreProcessor.h"

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
        int ploy_modulus_degree;
        std::vector<int32_t> modulus_chain;
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
        EncryptionParameters generateParameters(double scale, std::vector<int32_t> &modulus_chain, int poly_modulus_degree);

        /**
         * @brief generate keys
         * 
         * @param parms EncryptionParameters
         * @return std::tuple<PublicKey, SecretKey, RelinKeys, GaloisKeys, std::shared_ptr<SEALContext>> 
         */
        std::tuple<PublicKey, SecretKey, RelinKeys, GaloisKeys, std::shared_ptr<SEALContext>> gen_keys(EncryptionParameters parms);

        /**
         * @brief encrypt inptu matrix
         * 
         * @param input matrix
         * @return std::vector<Ciphertext> 
         */
        std::vector<Ciphertext> encrypt(std::vector<std::vector<double>> &input);

        /**
         * @brief preprocess
         * 
         * @param input vector of strings
         * @return std::vector<std::vector<double>> 
         */
        std::vector<std::vector<double>> preprocess(std::vector<std::string> &input);

    public:
        /**
         * @brief Construct a new Client Side object
         * 
         * @param preprocessor 
         */
        ClientSide(Preprocessor &preprocessor);

        /**
         * @brief generate keys and evaluators and set as private variables
         * 
         * @param scale 
         * @param modulus_chain 
         * @param poly_modulus_degree 
         */
        void generate_keys(double scale, std::vector<int32_t> &modulus_chain, int poly_modulus_degree);

        /**
         * @brief Client side preprocessing
         * 
         * @param input batched input vector
         * @return std::vector<Ciphertext> 
         */
        std::vector<Ciphertext> process(std::vector<std::string> &input);

        // Getter
        std::shared_ptr<Evaluator> getEvaluator();
        std::shared_ptr<Encryptor> getEncryptor();
        std::shared_ptr<Decryptor> getDecryptor();
        std::shared_ptr<PublicKey> getPublicKey();
        std::shared_ptr<SecretKey> getSecretKey();
        std::shared_ptr<GaloisKeys> getGalKey();
        std::shared_ptr<RelinKeys> getRelinKeys();
    };
}