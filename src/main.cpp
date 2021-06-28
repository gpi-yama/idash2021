#include "common.h"
#include "server/ServerSide.h"
#include "client/ClientSide.h"
#include "preprocess/DummyPreprocessor.h"

using namespace capsuleGene;

int main()
{
    // ----------- define parameters -------------
    const double scale = pow(2.0, 40);
    const int poly_modulus_degree = 8192;
    const std::vector<int32_t> modulus = {60, 40, 60};
    // -----------------------------------------

    // ----------- input ----------
    const std::vector<std::string> input(10, "TEST");
    const int input_dim = 100;
    const int output_dim = 4;
    // ----------------------------

    ClientSide client;
    client.generate_keys(scale, modulus, poly_modulus_degree);
    std::cout << "client process" << std::endl;
    const std::vector<Ciphertext> enc_input = client.process(input);

    const std::shared_ptr<Evaluator> evaluator = client.getEvaluator();
    const std::shared_ptr<Encryptor> encryptor = client.getEncryptor();
    const std::shared_ptr<GaloisKeys> gal_keys = client.getGalKey();
    const std::shared_ptr<RelinKeys> rel_keyss = client.getRelinKeys();
    const std::shared_ptr<CKKSEncoder> encoder = client.getEncoder();

    LogisticRegression ml(input_dim, output_dim, scale, poly_modulus_degree, evaluator, encryptor, encoder, gal_keys, rel_keyss);

    ServerSide server(ml, evaluator, encryptor, gal_keys, rel_keyss, encoder);
    server.load_weight("/home/yamaguchi/idash2021/coef.csv");
    server.load_bias("/home/yamaguchi/idash2021/bias.csv");
    std::cout << "server process" << std::endl;
    const std::vector<std::vector<Ciphertext>> output = server.process(enc_input);

    std::cout << "client postprocess" << std::endl;
    std::vector<std::vector<double>> result = client.postprocess(output);
    for (auto vec : result)
    {
        int max = *std::max_element(vec.begin(), vec.end());
        for (auto v : vec)
        {
            std::cout << v << ", ";
        }
        std::cout << max << std::endl;
    }
    // std::cout << "eof" << std::endl;
    return 0;
}