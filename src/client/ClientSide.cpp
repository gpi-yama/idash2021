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
        this->poly_modulus_degree = poly_modulus_degree;
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

    std::vector<Ciphertext> ClientSide::batch_encrypt(const std::vector<std::vector<double>> &input)
    {
        Plaintext plain;
        Ciphertext ctxt;

        uint32_t i, j, k;
        uint32_t N = input.size();
        uint32_t input_dim = input[0].size();
        this->slot_per_feat = pow(2, int(log2(input_dim) + 1));
        uint32_t num_feat_per_ctxt = uint32_t(poly_modulus_degree / 2 / this->slot_per_feat);
        uint32_t num_ctxt = int(N / num_feat_per_ctxt + 1);
        std::vector<Ciphertext> ctxt_list(num_ctxt);

#pragma omp parallel for
        for (i = 0; i < num_ctxt; i++)
        {
            std::vector<double> feat_in_single_vec(poly_modulus_degree / 2, 0.0);
            for (j = 0; j < num_feat_per_ctxt; j++)
            {
                if (i * num_feat_per_ctxt + j >= N)
                    break;
                for (k = 0; k < input_dim; k++)
                {
                    feat_in_single_vec[k + j * this->slot_per_feat] = input[j + num_feat_per_ctxt * i][k];
                }
            }
            ctxt_list[i] = this->encrypt(feat_in_single_vec);
        }
        return ctxt_list;
    }

    std::vector<double> ClientSide::decrypt(const std::vector<Ciphertext> &enc_x)
    {
        Plaintext plain;
        uint32_t i, size = enc_x.size();
        std::vector<double> result(size);
        std::vector<double> decrypted;

#pragma omp parallel for private(plain, decrypted)
        for (i = 0; i < size; i++)
        {
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

    std::vector<std::vector<double>> ClientSide::batch_decrypt(const std::vector<std::vector<Ciphertext>> &enc_x)
    {
        Plaintext plain;
        uint32_t i, j, k, size = enc_x.size();
        uint32_t output_dim = enc_x[0].size();
        uint32_t num_feat_per_ctxt = uint32_t(poly_modulus_degree / 2 / this->slot_per_feat);
        std::vector<std::vector<double>> result(size * num_feat_per_ctxt, std::vector<double>(output_dim));
        std::vector<double> decrypted;
        // #pragma omp parallel for private(plain, decrypted)
        for (i = 0; i < size; i++) // which ctxt is decryopted
        {
            for (j = 0; j < output_dim; j++)
            {
                decrypted = this->decrypt(enc_x[i][j]);
                for (k = 0; k < num_feat_per_ctxt; k++)
                {
                    // output_dim
                    result[i * num_feat_per_ctxt + k][j] = decrypted[k * this->slot_per_feat];
                }
            }
        }
        return result;
    }

    std::vector<Ciphertext> ClientSide::process(const std::vector<std::string> &input)
    {
        return this->batch_encrypt(ClientSide::preprocess(input));
    }

    std::vector<std::vector<double>> ClientSide::postprocess(const std::vector<std::vector<Ciphertext>> &x)
    {
        uint32_t i, size = x.size();
        std::vector<std::vector<double>> result = this->batch_decrypt(x);
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