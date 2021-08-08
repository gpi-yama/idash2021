#include <chrono>
#include "common.h"
#include "server/ServerSide.h"
#include "client/ClientSide.h"

using namespace capsuleGene;

int save_csv_to_bin(){
    const std::string data_path = "/home/yamaguchi/idash2021/data/";

    std::string path_pca_components = data_path + "pca_5_components.csv";
    std::string path_dict = data_path + "dict";
    std::string path_pca_variance = data_path + "pca_5_variance.csv";
    std::string path_pca_mean = data_path + "pca_5_mean.csv";

    std::vector<std::vector<float>> components = IOUtils::read_csv(path_pca_components, false, false, ' ');
    std::vector<float> flatten;
    for (int i = 0; i < components.size(); i++){
        for (int j = 0; j < components[i].size(); j++){
            flatten.push_back(components[i][j]);
        }
    }
    IOUtils::write_binary_vector(data_path+"pca_5_components.bin", flatten);

    std::vector<float> mean = IOUtils::read_csv_vec(path_pca_mean);
    IOUtils::write_binary_vector(data_path+"pca_5_mean.bin", mean);

    std::vector<float> variance = IOUtils::read_csv_vec(path_pca_variance);
    IOUtils::write_binary_vector(data_path+"pca_5_variance.bin", variance);
    return 0;
}

int main_batch()
{
    std::chrono::system_clock::time_point start, end, start1, end1;
    double elapsed;
    start = std::chrono::system_clock::now();

    // ----------- define parameters -------------
    const double scale = pow(2.0, 40);
    const int poly_modulus_degree = 8192;//4096;
    const std::vector<int32_t> modulus = {60, 40, 60};
    // -----------------------------------------

    // if you want to know 128bit max coefs please use this.
    // auto a = CoeffModulus::MaxBitCount(4096);
    // std::cout << a << std::endl;

    // ----------- input ----------
    // const std::vector<std::string> input(100, "TEST");
    const std::string path = "/home/yamaguchi/idash2021/dataset/Challenge_test.fa";
    const std::string data_path = "/home/yamaguchi/idash2021/data/";
    const int input_dim = 200;
    const int output_dim = 4;
    // ----------------------------
    std::string path_pca_components = data_path + "pca_200_components.npy";
    std::string path_dict = data_path + "dict";
    std::string path_pca_variance = data_path + "pca_200_variance.npy";
    std::string path_pca_mean = data_path + "pca_200_mean.npy";

    start1 = std::chrono::system_clock::now();
    std::shared_ptr<Preprocessor> preprocessor = std::make_shared<Preprocessor>(path_pca_components, path_pca_mean, path_pca_variance, path_dict);

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
    server.load_weight("/home/yamaguchi/idash2021/data/coef_200.npy");
    server.load_bias("/home/yamaguchi/idash2021/data/bias_200.npy");
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
    std::vector<std::vector<float>> ans = IOUtils::read_csv("/home/yamaguchi/idash2021/out_tests.csv", false, false);
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
        std::cout << i << "," << max << ", " << ans[i][0] << std::endl;
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
    const double scale = pow(2.0, 25);
    const int poly_modulus_degree = 4096;
    const std::vector<int32_t> modulus = {30, 25, 30};
    // -----------------------------------------

    // if you want to know 128bit max coefs please use this.
    // auto a = CoeffModulus::MaxBitCount(4096);
    // std::cout << a << std::endl;

    // ----------- input ----------
    const std::vector<std::string> input(100, "TEST");
    const std::string path = "/home/yamaguchi/idash2021/dataset/Challange.a";
    const std::string path_of_components = "/home/yamaguchi/idash2021/data/pca_200_components.csv";
    const std::string path_of_mean = "/home/yamaguchi/idash2021/data/pca_200_mean.csv";
    const std::string path_of_variance = "/home/yamaguchi/idash2021/data/pca_200_variance.csv";
    const std::string path_of_dictionary = "/home/yamaguchi/idash2021/data/cols_from_python.txt";
    
    
    const int input_dim = 100;
    const int output_dim = 800;
    const int dim_of_raw = 30000;
    // ----------------------------

    start1 = std::chrono::system_clock::now();
    Preprocessor preprocessor(path_of_components, path_of_mean, path_of_variance, path_of_dictionary);
    ClientSide client;
    client.generate_keys(scale, modulus, poly_modulus_degree);
    const std::vector<Ciphertext> enc_input = client.process(path);

    end1 = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
    std::cout << "client time: " << elapsed << std::endl;

    CoefLogisticRegression ml(input_dim, output_dim, scale, poly_modulus_degree, client.getEvaluator(), client.getEncoder(), client.getGalKey(), client.getRelinKeys());

    start1 = std::chrono::system_clock::now();

    ServerSide server(ml);
    server.load_weight("/home/yamaguchi/idash2021/data/coef.npy");
    server.load_bias("/home/yamaguchi/idash2021/data/bias.npy");
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
    std::vector<std::vector<float>> ans = IOUtils::read_csv("/home/yamaguchi/idash2021/out_tests.csv", false, false);
    int max, i;
    int N = ans.size();
    float acc = 0.0;

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

int main(){
    save_csv_to_bin();
    main_batch();
    // main_coef();
    return 0;
}