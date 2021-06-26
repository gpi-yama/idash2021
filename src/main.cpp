#include "common.h"
#include "server/ServerSide.h"
#include "client/ClientSide.h"
#include "preprocess/DummyPreprocessor.h"

using namespace capsuleGene;

int main()
{
    // ----------- define parameters -------------
    double scale = pow(2.0, 40);
    int poly_modulus_degree = 8192;
    std::vector<int32_t> modulus = {60, 40, 40, 60};
    // -----------------------------------------

    // ----------- input ----------
    std::vector<std::string> input(10, "TEST");
    int input_dim = 100;
    int output_dim = 4;
    // ----------------------------

    // Preprocessor *preprocessor = new DummyPreprocessor();
    // ClientSide client(*preprocessor);
    DummyPreprocessor preprocessor = DummyPreprocessor();
    ClientSide client(preprocessor);
    client.generate_keys(scale, modulus, poly_modulus_degree);
    std::vector<Ciphertext> enc_input = client.process(input);

    std::shared_ptr<Evaluator> evaluator = client.getEvaluator();
    std::shared_ptr<Encryptor> encryptor = client.getEncryptor();
    std::shared_ptr<GaloisKeys> gal_keys = client.getGalKey();
    std::shared_ptr<RelinKeys> relKeys = client.getRelinKeys();
    std::shared_ptr<CKKSEncoder> encoder = client.getEncoder();

    LogisticRegression ml(input_dim, output_dim, scale, evaluator, encryptor, encoder, gal_keys, relKeys);
    ServerSide server(ml, evaluator, encryptor, gal_keys, relKeys, encoder);
    server.load_weight("/home/yamaguchi/idash2021/coef.csv");
    server.load_bias("/home/yamaguchi/idash2021/bias.csv");
    std::vector<std::vector<Ciphertext>> output = server.process(enc_input);

    std::vector<std::vector<double>> result = client.postprocess(output);
    return 0;
}