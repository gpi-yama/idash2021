#include "utils/Timer.hpp"
#include "common.h"
#include "preprocess/PreProcessor.h"

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

int main(int argc, char *argv[]){
    if (argc != 2){
        std::cerr << "usage: ./bin/preprocessing /path/to/Challange.fa" << std::endl;
        return -1;
    }
    // 
    std::string input_path = argv[1];
    std::string parameter_folder = "../data";

    
    printf("---------------------------\n");
    printf("input_path: %s\n", input_path.c_str());
    printf("parameter_folder: %s\n", parameter_folder.c_str());
    printf("--- start preprocessing ---\n");

    Timer local_time;
    int batch_size = preprocessing(parameter_folder, input_path);
    IOUtils::write_batch_size_to_file(batch_size, parameter_folder+"/batch_size.txt");
    std::cout << "preprocessing_time[ms]: " << local_time.end() << std::endl;
    printf("---- end preprocessing ----\n");
    return 0;
}