#include "utils/Timer.hpp"
#include "common.h"
#include "server/ServerSide.h"
#include "client/ClientSide.h"

using namespace capsuleGene;


int main_batch(std::string parameter_folder, std::string output_path)
{
    Timer timer_all;
    int batch_size;
    double computation_time, encryption_time, decryption_time, round_trip_time;
    batch_size = IOUtils::load_batch_size_from_file(parameter_folder+"/batch_size.txt");

    // load preprocessed features
    std::vector<std::vector<float>> features(batch_size, std::vector<float>(N_COMPONENTS));
    IOUtils::read_binary_vector2d(parameter_folder+"/features.bin", features, batch_size, N_COMPONENTS);

    // Client encryption
    Timer local_timer;
    ClientSide client;
    client.generate_keys(SCALE, MODULUS, POLY_MODULUS_DEGREE);
    const std::vector<Ciphertext> enc_input = client.batch_encrypt(features);
    encryption_time = local_timer.end();
    std::cout << "[client] encryption time: " << encryption_time << std::endl;
    local_timer.reset();

    // Server computation:
    LogisticRegression ml(N_COMPONENTS, Y_COLUMN, SCALE, POLY_MODULUS_DEGREE, client.getEvaluator(), client.getEncoder(), client.getGalKey(), client.getRelinKeys());
    ServerSide server(ml);
    server.load_weight(parameter_folder + "/coef_" + std::to_string(N_COMPONENTS) + ".npy");
    server.load_bias(parameter_folder + "/bias_" + std::to_string(N_COMPONENTS) + ".npy");
    const std::vector<std::vector<Ciphertext>> enc_result = server.process(enc_input);
    computation_time = local_timer.end();
    std::cout << "[server] computation time: " << computation_time << std::endl;
    local_timer.reset();

    // decryption
    std::vector<std::vector<double>> result = client.postprocess(enc_result);
    decryption_time = local_timer.end();
    std::cout << "[client] decryption time: " << local_timer.end() << std::endl;
    IOUtils::write_prob_to_file(result, output_path+"/result.csv");
    std::cout << "total time: " << round_trip_time << std::endl;
    local_timer.reset();

    // write results
    round_trip_time = timer_all.end();
    std::ofstream fmetrics(output_path + "/metrics.csv");
    fmetrics << "round_trip_time[ms], encryption_time[ms], computation_time[ms], decryption_time[ms]" << std::endl;
    fmetrics << round_trip_time << "," << encryption_time << ", " << computation_time << ", " << decryption_time << std::endl;
    fmetrics.close();
    return 0;
}

// this is coef encoded lr
int main_coef(std::string parameter_folder, std::string output_path){
    Timer timer_all;
    int batch_size;
    double computation_time, encryption_time, decryption_time, round_trip_time;
    batch_size = IOUtils::load_batch_size_from_file(parameter_folder+"/batch_size.txt");

    // load preprocessed features
    std::vector<std::vector<float>> features(batch_size, std::vector<float>(N_COMPONENTS));
    IOUtils::read_binary_vector2d(parameter_folder+"/features.bin", features, batch_size, N_COMPONENTS);

    // Client encryption
    Timer local_timer;
    ClientSide client;
    client.generate_keys(SCALE, MODULUS, POLY_MODULUS_DEGREE);
    const std::vector<Ciphertext> enc_input = client.coef_encrypt(features);
    encryption_time = local_timer.end();
    std::cout << "[client] encryption time: " << encryption_time << std::endl;
    local_timer.reset();

    // Server inference
    CoefLogisticRegression ml(N_COMPONENTS, Y_COLUMN, SCALE, POLY_MODULUS_DEGREE, client.getEvaluator(), client.getEncoder(), client.getGalKey(), client.getRelinKeys());
    ServerSide server(ml);
    server.load_weight(parameter_folder + "/coef_" + std::to_string(N_COMPONENTS) + ".npy");
    server.load_bias(parameter_folder + "/bias_" + std::to_string(N_COMPONENTS) + ".npy");
    const std::vector<std::vector<Ciphertext>> enc_result = server.process(enc_input);
    computation_time = local_timer.end();
    std::cout << "[server] computation time: " << computation_time << std::endl;
    local_timer.reset();

    // Client postprocessing
    std::vector<std::vector<double>> result = client.postprocess_as_coef(enc_result);
    decryption_time = local_timer.end();
    std::cout << "[client] decryption time: " << decryption_time << std::endl;
    local_timer.reset();
    
    // write results
    IOUtils::write_prob_to_file(result, output_path+"/result.csv");
    round_trip_time = timer_all.end();
    std::cout << "total time: " << round_trip_time << std::endl;

    std::ofstream fmetrics(output_path + "/metrics.csv");
    fmetrics << "round_trip_time[ms], encryption_time[ms], computation_time[ms], decryption_time[ms]" << std::endl;
    fmetrics << round_trip_time << "," << encryption_time << ", " << computation_time << ", " << decryption_time << std::endl;
    fmetrics.close();
    return 0;
}

int main(int argc, char *argv[]){
    if (argc != 2){
        std::cerr << "usage ./bin/pred_rna output_folder" << std::endl;
        return -1;
    }
    // 
    std::string parameter_folder = "../data";
    std::string output_path = argv[1];

    printf("\n====================================================\n");
    printf("output_path: %s\n", output_path.c_str());

    // if you want to run with basic lr method
    // main_batch(parameter_folder, output_path);
    // if you want to use coefficient encoding lr pelase use this function
    
    main_coef(parameter_folder, output_path);
    return 0;
}