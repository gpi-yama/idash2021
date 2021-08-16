#include "utils/Timer.hpp"
#include "common.h"
#include "server/ServerSide.h"
#include "client/ClientSide.h"

using namespace capsuleGene;



int preprocessing(std::string parameter_folder, std::string input_path){
    std::string path_pca_components = parameter_folder + "/pca_" + std::to_string(N_COMPONENTS) + "_components.npy";
    std::string path_pca_variance = parameter_folder + "/pca_" + std::to_string(N_COMPONENTS) + "_variance.npy";
    std::string path_pca_mean = parameter_folder + "/pca_" + std::to_string(N_COMPONENTS) + "_mean.npy";

    Preprocessor preprocessor(path_pca_components, path_pca_mean, path_pca_variance);
    std::vector<std::vector<float>> features = preprocessor.process(input_path);
    IOUtils::write_binary_vector2d(parameter_folder+"/features.bin", features);
    return features.size();
}


int main_batch(std::string parameter_folder, std::string input_path, std::string output_path)
{
    Timer timer_all;
    Timer local_timer;

    // Client preprocessing
    std::cout << "[client] preprocessing..." << std::endl;
    int batch_size = preprocessing(parameter_folder, input_path);
    std::cout << "[client] preprocessing time: " << local_timer.end() << std::endl;
    local_timer.reset();

    // Client encryption
    std::vector<std::vector<float>> features(batch_size, std::vector<float>(N_COMPONENTS));
    IOUtils::read_binary_vector2d(parameter_folder+"/features.bin", features, batch_size, N_COMPONENTS);
    ClientSide client;
    client.generate_keys(SCALE, MODULUS, POLY_MODULUS_DEGREE);
    const std::vector<Ciphertext> enc_input = client.batch_encrypt(features);
    std::cout << "[client] encryption time: " << local_timer.end() << std::endl;
    local_timer.reset();

    // Server inference:
    LogisticRegression ml(N_COMPONENTS, Y_COLUMN, SCALE, POLY_MODULUS_DEGREE, client.getEvaluator(), client.getEncoder(), client.getGalKey(), client.getRelinKeys());
    ServerSide server(ml);
    server.load_weight(parameter_folder + "/coef_" + std::to_string(N_COMPONENTS) + ".npy");
    server.load_bias(parameter_folder + "/bias_" + std::to_string(N_COMPONENTS) + ".npy");
    const std::vector<std::vector<Ciphertext>> enc_result = server.process(enc_input);
    std::cout << "[server] inference time: " << local_timer.end() << std::endl;
    local_timer.reset();

    std::vector<std::vector<double>> result = client.postprocess(enc_result);
    std::cout << "[client] postprocessing time: " << local_timer.end() << std::endl;
    local_timer.reset();

    std::vector<std::string> parsed_classes = MathUtils::parse_results(result, batch_size);
    IOUtils::write_label_to_file(parsed_classes, output_path);
    std::cout << "total time: " << timer_all.end() << std::endl;
    return 0;
}

// this is coef encoded lr
int main_coef(std::string parameter_folder, std::string input_path, std::string output_path){
    Timer timer_all;
    Timer local_timer;

    // Client preprocessing
    std::cout << "[client] preprocessing..." << std::endl;
    int batch_size = preprocessing(parameter_folder, input_path);
    std::cout << "[client] preprocessing time: " << local_timer.end() << std::endl;
    local_timer.reset();

    // Client encryption
    std::cout << "[client] encrypting..." << std::endl;
    std::vector<std::vector<float>> features(batch_size, std::vector<float>(N_COMPONENTS));
    IOUtils::read_binary_vector2d(parameter_folder+"/features.bin", features, batch_size, N_COMPONENTS);
    ClientSide client;
    client.generate_keys(SCALE, MODULUS, POLY_MODULUS_DEGREE);
    const std::vector<Ciphertext> enc_input = client.coef_encrypt(features);
    std::cout << "[client] encryption time: " << local_timer.end() << std::endl;
    local_timer.reset();

    // Server inference
    std::cout << "[server] predicting..." << std::endl;
    CoefLogisticRegression ml(N_COMPONENTS, Y_COLUMN, SCALE, POLY_MODULUS_DEGREE, client.getEvaluator(), client.getEncoder(), client.getGalKey(), client.getRelinKeys());
    ServerSide server(ml);
    server.load_weight(parameter_folder + "/coef_" + std::to_string(N_COMPONENTS) + ".npy");
    server.load_bias(parameter_folder + "/bias_" + std::to_string(N_COMPONENTS) + ".npy");
    const std::vector<std::vector<Ciphertext>> enc_result = server.process(enc_input);
    std::cout << "[server] inference time: " << local_timer.end() << std::endl;
    local_timer.reset();

    // Client postprocessing
    std::cout << "[client] postprocessing..." << std::endl;
    std::vector<std::vector<double>> result = client.postprocess_as_coef(enc_result);
    std::cout << "[client] postprocessing time: " << local_timer.end() << std::endl;
    local_timer.reset();

    std::vector<std::string> parsed_classes = MathUtils::parse_results(result, batch_size);
    IOUtils::write_label_to_file(parsed_classes, output_path);
    std::cout << "total time: " << timer_all.end() << std::endl;
    return 0;
}

int main(int argc, char *argv[]){
    if (argc != 4){
        std::cerr << "usage ./test_main input_path parameter_folder output_path" << std::endl;
        return -1;
    }
    // 
    std::string input_path = argv[1];
    std::string parameter_folder = argv[2];
    std::string output_path = argv[3];

    printf("\n====================================================\n");
    printf("input_path: %s\n", input_path.c_str());
    printf("parameter_folder: %s\n", parameter_folder.c_str());
    printf("output_path: %s\n", output_path.c_str());

    // if you want to run with basic lr method
    // main_batch(parameter_folder, input_path, output_path);
    // if you want to use coefficient encoding lr pelase use this function
    main_coef(parameter_folder, input_path, output_path);
    return 0;
}