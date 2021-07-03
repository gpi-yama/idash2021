#include <chrono>
#include "common.h"
#include "server/ServerSide.h"
#include "client/ClientSide.h"
#include "preprocess/DummyPreprocessor.h"

using namespace capsuleGene;

int main()
{
    std::chrono::system_clock::time_point start, end, start1, end1, start2, end2, start3, end3;
    double elapsed;
    start = std::chrono::system_clock::now();

    // ----------- define parameters -------------
    const double scale = pow(2.0, 25);
    const int poly_modulus_degree = 4096;
    const std::vector<int32_t> modulus = {30, 25, 30};
    // -----------------------------------------

    // if you want to know 128bit max coefs please use this.
    // auto a = CoeffModulus::MaxBitCount(4096);
    // std::cout << a << std::endl;

    // ----------- input ----------
    const std::vector<std::string> input(100, "TEST");
    const int input_dim = 10;
    const int output_dim = 4;
    // ----------------------------

    start1 = std::chrono::system_clock::now();

    ClientSide client;
    client.generate_keys(scale, modulus, poly_modulus_degree);
    const std::vector<Ciphertext> enc_input = client.process(input);

    end1 = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
    std::cout << "client time: " << elapsed << std::endl;

    const std::shared_ptr<Evaluator> evaluator = client.getEvaluator();
    const std::shared_ptr<Encryptor> encryptor = client.getEncryptor();
    const std::shared_ptr<GaloisKeys> gal_keys = client.getGalKey();
    const std::shared_ptr<RelinKeys> rel_keyss = client.getRelinKeys();
    const std::shared_ptr<CKKSEncoder> encoder = client.getEncoder();

    LogisticRegression ml(input_dim, output_dim, scale, poly_modulus_degree, evaluator, encryptor, encoder, gal_keys, rel_keyss);

    start1 = std::chrono::system_clock::now();

    ServerSide server(ml, evaluator, encryptor, gal_keys, rel_keyss, encoder);
    server.load_weight("/home/yamaguchi/idash2021/coef.csv");
    server.load_bias("/home/yamaguchi/idash2021/bias.csv");
    std::cout << "server process" << std::endl;
    const std::vector<std::vector<Ciphertext>> output = server.process(enc_input);

    end1 = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
    std::cout << "server time: " << elapsed << std::endl;

    start1 = std::chrono::system_clock::now();

    std::cout << "client postprocess" << std::endl;
    std::vector<std::vector<double>> result = client.postprocess(output);

    end1 = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
    std::cout << "client post process time: " << elapsed << std::endl;

    // calculate accuracy
    std::vector<std::vector<double>> ans = IOUtils::read_csv("/home/yamaguchi/idash2021/out_tests.csv", false, false);
    int max, i;
    int N = result.size();
    float acc = 0.0;

#pragma omp parallel for private(max) reduction(+ \
                                                : acc)
    for (i = 0; i < N; i++)
    {
        max = distance(result[i].begin(), std::max_element(result[i].begin(), result[i].end()));
        acc += int(max == int(ans[i][0]));
    }

    std::cout << "ACC: " << acc / N << std::endl;
    end = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "total time: " << elapsed << std::endl;

    return 0;
}