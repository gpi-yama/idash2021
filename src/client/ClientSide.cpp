#include "ClientSide.h"

namespace capsuleGene
{
    EncryptionParameters ClientSide::generateParameters(const double scale, const std::vector<int32_t> &modulus_chain, const int poly_modulus_degree)
    {
        EncryptionParameters parms(scheme_type::ckks);
        parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, modulus_chain));
        parms.set_poly_modulus_degree(poly_modulus_degree);
        return parms;
    }

    void ClientSide::generate_keys(const double scale, const std::vector<int32_t> &modulus_chain, const int poly_modulus_degree)
    {
        this->scale = scale;
        EncryptionParameters parms = ClientSide::generateParameters(scale, modulus_chain, poly_modulus_degree);

        // create context
        std::shared_ptr<SEALContext> context = std::make_shared<SEALContext>(parms);

        // key generation
        KeyGenerator keygen(*context.get());

        PublicKey public_key;
        keygen.create_public_key(public_key);

        SecretKey secret_key = keygen.secret_key();

        RelinKeys relin_keys;
        keygen.create_relin_keys(relin_keys);

        GaloisKeys galois_keys;
        keygen.create_galois_keys(galois_keys);

        this->encryptor = std::make_shared<Encryptor>(*context.get(), public_key);
        this->evaluator = std::make_shared<Evaluator>(*context.get());
        this->decryptor = std::make_shared<Decryptor>(*context.get(), secret_key);
        this->encoder = std::make_shared<CKKSEncoder>(*context.get());

        this->public_key = std::make_shared<PublicKey>(public_key);
        this->secret_key = std::make_shared<SecretKey>(secret_key);
        this->gal_keys = std::make_shared<GaloisKeys>(galois_keys);
        this->relin_keys = std::make_shared<RelinKeys>(relin_keys);
    }

    std::vector<std::vector<double>> ClientSide::preprocess(const std::vector<std::string> &input)
    {
        return DummyPreprocessor::process(input);
    }

    std::vector<Ciphertext> ClientSide::encrypt(const std::vector<std::vector<double>> &input)
    {
        uint32_t i, size = input.size();
        Plaintext plain;
        std::vector<Ciphertext> ctxt(size);
#pragma omp parallel for private(plain)
        for (i = 0; i < size; i++)
        {
            this->encoder->encode(input[i], this->scale, plain);
            this->encryptor->encrypt(plain, ctxt[i]);
        }
        return ctxt;
    }

    Ciphertext ClientSide::encrypt(const std::vector<double> &input)
    {
        Plaintext plain;
        Ciphertext ctxt;
        this->encoder->encode(input, this->scale, plain);
        this->encryptor->encrypt(plain, ctxt);
        return ctxt;
    }

    std::vector<double> ClientSide::decrypt(const std::vector<Ciphertext> &enc_x)
    {
        Plaintext plain;
        uint32_t i, size = enc_x.size();
        std::vector<double> result(size);
#pragma omp parallel for private(plain)
        for (i = 0; i < size; i++)
        {
            std::vector<double> decrypted;
            this->decryptor->decrypt(enc_x[i], plain);
            this->encoder->decode(plain, decrypted);
            result[i] = decrypted[0];
        }
        return result;
    }

    std::vector<double> ClientSide::decrypt(const Ciphertext &enc_x)
    {
        Plaintext plain;
        std::vector<double> result;
        this->decryptor->decrypt(enc_x, plain);
        this->encoder->decode(plain, result);
        return result;
    }

    std::vector<Ciphertext> ClientSide::process(const std::vector<std::string> &input)
    {
        return this->encrypt(ClientSide::preprocess(input));
    }

    std::vector<std::vector<double>> ClientSide::postprocess(const std::vector<std::vector<Ciphertext>> &x)
    {
        uint32_t i, size = x.size();
        std::vector<double> likelihood;
        std::vector<std::vector<double>> result(size);

#pragma omp parallel for
        for (i = 0; i < size; i++)
        {
            result[i] = this->decrypt(x[i]);
        }
        return SoftmaxPostprocessor::process(result);
    }

    std::shared_ptr<Evaluator> ClientSide::getEvaluator()
    {
        return this->evaluator;
    }
    std::shared_ptr<Encryptor> ClientSide::getEncryptor()
    {
        return this->encryptor;
    }
    std::shared_ptr<Decryptor> ClientSide::getDecryptor()
    {
        throw std::runtime_error("You cannot get secret key!");
        // return this->decryptor;
    }
    std::shared_ptr<CKKSEncoder> ClientSide::getEncoder()
    {
        return this->encoder;
    }
    std::shared_ptr<PublicKey> ClientSide::getPublicKey()
    {
        return this->public_key;
    }
    std::shared_ptr<SecretKey> ClientSide::getSecretKey()
    {
        throw std::runtime_error("You cannot get secret key!");
        // return this->secret_key();
    }
    std::shared_ptr<GaloisKeys> ClientSide::getGalKey()
    {
        return this->gal_keys;
    }
    std::shared_ptr<RelinKeys> ClientSide::getRelinKeys()
    {
        return this->relin_keys;
    }
}