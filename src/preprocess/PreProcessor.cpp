#include "PreProcessor.h"


namespace capsuleGene{
    std::vector<std::string> Preprocessor::split(const std::string &s, char delim){
        std::vector<std::string> elems;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            if (!item.empty()) {
                elems.push_back(item);
            }
        }
        return elems;
    }

    std::vector<double> Preprocessor::split_to_float(const std::string &s, char delim){
        std::vector<double> elems;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            if (!item.empty()) {
                elems.push_back(std::stof(item));
            }
        }
        return elems;
    }

    void Preprocessor::load_seq(const std::string path,std::vector<std::string>& sequences){
        uint32_t d;
        try{
            std::ifstream ifs(path, std::ios::in);
            ifs.exceptions(std::ifstream::badbit);
            std::string line;
            uint32_t num_rows = 0;
            while (ifs.good()){
                std::getline(ifs, line);
                num_rows += 1;
                if (line.empty()) break;
                if (num_rows % 2 == 0){
                    d = MAX_SEQ - line.size();
                    line += std::string(d,'O');
                    sequences.emplace_back(line);
                }
            }
            ifs.close();
        }
        catch (std::ios_base::failure const &e)
        {
            throw std::ios_base::failure(std::string("fatal error occured while opening file.") + e.what());
        }
        catch (std::exception const &e)
        {
            throw std::runtime_error(std::string("runtime exception occured while reading file") + e.what());
        }
    }

    void Preprocessor::load_components(std::string path, std::vector<float> &out){
        IOUtils::read_vector_npy(path, out);
    }

    void Preprocessor::load_mean(std::string path, std::vector<float> &out){
        IOUtils::read_vector_npy(path, out);
    }
        
    void Preprocessor::load_variance(std::string path, std::vector<float> &out){
        IOUtils::read_vector_npy(path, out);
        uint32_t i;
        for (i = 0; i < N_COMPONENTS; i++){
            out[i] = std::sqrt(out[i]);
        }
    }

    void Preprocessor::binarize_by_existance(std::vector<std::vector<float>>& buf, std::vector<std::string>& seqs, std::unordered_map<char, int> rnaMap, const uint32_t batch_size){

        std::array<std::array<float, 4>, 16> rnaTable = {{
        //     A      C      G      T
        {1.f,   0.f,   0.f,   0.f},
        {0.f,   1.f,   0.f,   0.f},
        {0.f,   0.f,   1.f,   0.f},
        {0.f,   0.f,   0.f,   1.f},
        {0.5f,  0.f,   0.5f,  0.f},   // A or G
        {0.f,   0.5f,  0.f,   0.5f}, // C or T
        {0.f,   0.5f,  0.5f,  0.f},  // G or C
        {0.5f,  0.f,   0.f,   0.5f},  // A or T
        {0.f,   0.f,   0.5f,  0.5f},  // G or T
        {0.5f,  0.5f,  0.f,   0.f},  // A or C
        {0.f,   0.33f, 0.33f, 0.33f},  // C or G or T
        {0.33f, 0.f,   0.33f, 0.33f},  // A or G or T
        {0.33f, 0.33f, 0.f,   0.33f},  // A or C or T
        {0.33f, 0.33f, 0.33f, 0.f}, // A or C or G
        {0.25f, 0.25f, 0.25f, 0.25f},  // any base
        {0.f,   0.f,   0.f,   0.f}
        }};  // NONE
    
        uint32_t i, j, k;
        int key;
        std::array<float, X_COLUMN> binarized;

        for(i = 0; i < batch_size; i++){
            for(j = 0; j < MAX_SEQ; j++){
                key = rnaMap[seqs[i][j]];
                for(k = 0; k < 4; k++){
                    binarized[j*4 + k] = rnaTable[key][k];
                }
            }
            this->pca(binarized, buf[i]);
        }
    }
        
    Preprocessor::Preprocessor(const std::string path_pca_components, 
                               const std::string path_pca_mean, 
                               const std::string path_pca_variance)
    {
        sqrt_variance.resize(N_COMPONENTS);
        mean.resize(X_COLUMN);
        components.resize(N_COMPONENTS * X_COLUMN);

        Preprocessor::load_mean(path_pca_mean, mean);
        Preprocessor::load_variance(path_pca_variance, sqrt_variance);
        Preprocessor::load_components(path_pca_components, components);
    }

    void Preprocessor::pca(std::array<float, X_COLUMN> &X, std::vector<float> &out){
        uint32_t i, j;

        for (i = 0; i < X_COLUMN; i++){
            X[i] -= mean[i];
        }

        float summed;
        for (i = 0; i < N_COMPONENTS; i++){
            summed = 0.f;
            for (j = 0; j < X_COLUMN; j++){
                summed += components[i*X_COLUMN + j] * X[j];
            }
            out[i] = summed;
        }

        for (i = 0; i < N_COMPONENTS; i++){
            out[i] /= sqrt_variance[i];
        }
    }

    std::vector<std::vector<float>> Preprocessor::process(const std::string data_path){
        /* Load Sequences */
        std::vector<std::string> sequences;
        Preprocessor::load_seq(data_path,sequences);
        uint32_t BATCH = sequences.size();

        /* MultiLabelBinalize */
        std::vector<std::vector<float>> X(BATCH, std::vector<float>(N_COMPONENTS));
        Preprocessor::binarize_by_existance(X, sequences, rnaMap, BATCH);
        return X;
    }
}