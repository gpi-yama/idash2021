#include <chrono>
#include "common.h"
#include "server/ServerSide.h"
#include "client/ClientSide.h"

using namespace capsuleGene;


int main_batch()
{
    std::chrono::system_clock::time_point start, end, start1, end1;
    double elapsed;
    start = std::chrono::system_clock::now();

    // ----------- define parameters -------------
    const double scale = pow(2.0, 30);
    const int poly_modulus_degree = 4096;
    const std::vector<int32_t> modulus = {38, 30, 38};
    // -----------------------------------------

    // if you want to know 128bit max coefs please use this.
    // auto a = CoeffModulus::MaxBitCount(4096);
    // std::cout << a << std::endl;

    // ----------- input ----------
    // const std::vector<std::string> input(100, "TEST");
    const std::string path = "/home/yamaguchi/idash2021/dataset/Challenge_test.fa";
    const std::string data_path = "/home/yamaguchi/idash2021/data/";
    const int input_dim = N_COMPONENTS;
    const int output_dim = Y_COLUMN;
    // ----------------------------
    std::string path_pca_components = data_path + "pca_" + std::to_string(input_dim) + "_components.npy";
    std::string path_pca_variance = data_path + "pca_" + std::to_string(input_dim) + "_variance.npy";
    std::string path_pca_mean = data_path + "pca_" + std::to_string(input_dim) + "_mean.npy";
    std::string usable_index = "hoge";

    start1 = std::chrono::system_clock::now();
    std::shared_ptr<Preprocessor> preprocessor = std::make_shared<Preprocessor>(path_pca_components, path_pca_mean, path_pca_variance, usable_index);

    ClientSide client(preprocessor);
    client.generate_keys(scale, modulus, poly_modulus_degree);
    const std::vector<Ciphertext> enc_input = client.process(path);

    end1 = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
    std::cout << "client time: " << elapsed << std::endl;

    LogisticRegression ml(input_dim, output_dim, scale, poly_modulus_degree, client.getEvaluator(), client.getEncoder(), client.getGalKey(), client.getRelinKeys());

    start1 = std::chrono::system_clock::now();

    ServerSide server(ml);
    std::cout << "load_weight" << std::endl;
    server.load_weight("/home/yamaguchi/idash2021/data/coef_" + std::to_string(input_dim) + ".npy");
    server.load_bias("/home/yamaguchi/idash2021/data/bias_" + std::to_string(input_dim) + ".npy");
    std::cout << "proess" << std::endl;
    const std::vector<std::vector<Ciphertext>> enc_result = server.process(enc_input);

    end1 = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
    std::cout << "server time: " << elapsed << std::endl;

    start1 = std::chrono::system_clock::now();

    std::vector<std::vector<double>> result = client.postprocess(enc_result);

    end1 = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
    std::cout << "client post process time: " << elapsed << std::endl;

    // calculate accuracy
    std::vector<std::vector<float>> ans = IOUtils::read_csv("/home/yamaguchi/idash2021/dataset/test_answer.txt", false, false);
    int max, i;
    int N = ans.size();
    float acc = 0.0;

    for (i = 0; i < N; i++)
    {
        max = distance(result[i].begin(), std::max_element(result[i].begin(), result[i].end()));
        acc += int(max == int(ans[i][0]));
        for (auto v : result[i])
        {
        std::cout << v << ", ";
        }
        std::cout << i << "," << max << ", ans:" << ans[i][0] << std::endl;
    }

    std::cout << "ACC: " << acc / N << std::endl;
    end = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "total time: " << elapsed << std::endl;

    return 0;
}

// this is coef encoded lr
int main_coef(){
    std::chrono::system_clock::time_point start, end, start1, end1;
    double elapsed;
    start = std::chrono::system_clock::now();

    // ----------- define parameters -------------
    const double scale = pow(2.0, 40);
    const int poly_modulus_degree = 8192;
    const std::vector<int32_t> modulus = {60, 40, 60};
    // -----------------------------------------

    // if you want to know 128bit max coefs please use this.
    // auto a = CoeffModulus::MaxBitCount(4096);
    // std::cout << a << std::endl;

    // ----------- input ----------
    const std::string path = "/home/yamaguchi/idash2021/dataset/Challenge_test.fa";
    const std::string data_path = "/home/yamaguchi/idash2021/data/";
    const int input_dim = N_COMPONENTS;
    const int output_dim = Y_COLUMN;
    // ----------------------------
    std::string path_pca_components = data_path + "pca_" + std::to_string(input_dim) + "_components.npy";
    std::string path_pca_variance = data_path + "pca_" + std::to_string(input_dim) + "_variance.npy";
    std::string path_pca_mean = data_path + "pca_" + std::to_string(input_dim) + "_mean.npy";
    std::string usable_index = "hoge";
    // ----------------------------

    start1 = std::chrono::system_clock::now();
    std::shared_ptr<Preprocessor> preprocessor = std::make_shared<Preprocessor>(path_pca_components, path_pca_mean, path_pca_variance, usable_index);
    ClientSide client(preprocessor);
    client.generate_keys(scale, modulus, poly_modulus_degree);
    const std::vector<Ciphertext> enc_input = client.process_as_coef(path);

    end1 = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
    std::cout << "client time: " << elapsed << std::endl;

    CoefLogisticRegression ml(input_dim, output_dim, scale, poly_modulus_degree, client.getEvaluator(), client.getEncoder(), client.getGalKey(), client.getRelinKeys());

    start1 = std::chrono::system_clock::now();

    ServerSide server(ml);
    server.load_weight("/home/yamaguchi/idash2021/data/coef_200.npy");
    server.load_bias("/home/yamaguchi/idash2021/data/bias_200.npy");
    const std::vector<std::vector<Ciphertext>> enc_result = server.process(enc_input);

    end1 = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
    std::cout << "server time: " << elapsed << std::endl;

    start1 = std::chrono::system_clock::now();

    std::vector<std::vector<double>> result = client.postprocess_as_coef(enc_result);

    end1 = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
    std::cout << "client post process time: " << elapsed << std::endl;

    // calculate accuracy
    std::vector<std::vector<float>> ans = IOUtils::read_csv("/home/yamaguchi/idash2021/dataset/test_answer.txt", false, false);
    int max, i, j;
    int N = ans.size();
    float acc = 0.0;

    for (i = 0; i < N; i++)
    {
        max = distance(result[i].begin(), std::max_element(result[i].begin(), result[i].end()));
        for (auto v : result[i])
        {
        std::cout << v << ", ";
        }
        std::cout << i << "," << max << ", ans:" << ans[i][0] << std::endl;
        acc += int(max == int(ans[i][0]));
    }

    std::cout << "ACC: " << acc / N << std::endl;
    end = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "total time: " << elapsed << std::endl;

    return 0;
}

int main(){
    // main_batch();
    main_coef();
    return 0;
}