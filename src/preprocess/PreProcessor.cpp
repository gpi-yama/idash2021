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

    std::multimap<std::string, std::vector<std::string>> Preprocessor::load_dictionary(std::string filename){
        // load file from python to string
        std::vector<std::string> values;
        std::string buf;

        std::ifstream ifs(filename);

        if(ifs.fail()){
            std::cerr << "file read failed" << std::endl;
        }

        while(getline(ifs,buf,',')){
            values.push_back(buf);
        }

        // make dict(multimap)
        // dict[i] is vector<string>, which is showing found alpabet at column i from training dataset
        std::multimap<std::string, std::vector<std::string>> dic;
        std::string tmp_index;
        std::vector<std::string> tmp_vec;
        for(int i=0; i<values.size(); i++){

            std::string separator = "_";
            int separator_length = separator.length();
            std::string value = values[i];

            auto offset = std::string::size_type(0);
            auto pos = value.find(separator, offset);
            auto index = value.substr(offset, pos - offset);

            if(tmp_index != index){
            dic.insert(std::make_pair(tmp_index, tmp_vec));
            tmp_vec.clear();
            }

            offset = pos + separator_length;
            pos = value.find(separator, offset);

            assert(pos == std::string::npos);

            auto alphabet = value.substr(offset);
            tmp_vec.push_back(alphabet);
            tmp_index = index;
        }
        return dic;
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
                // std::cout << "load seq" << std::endl;
                if (line.empty()) break;
                // if(line[0]=='>'){
                    // std::string label = Preprocessor::split(line,'_')[0].substr(1);
                    // labels.push_back(label);
                // 
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
        // char delimiter = ' ';
        // IOUtils::read_csv(path, out, false, false, delimiter);
        IOUtils::read_vector_npy(path, out);
        // IOUtils::read_binary_vector(path, out);
    }

    void Preprocessor::load_mean(std::string path, std::vector<float> &out){
        // IOUtils::read_csv_vec(path, out);
        IOUtils::read_vector_npy(path, out);
        // IOUtils::read_binary_vector(path, out);
    }
        
    void Preprocessor::load_variance(std::string path, std::vector<float> &out){
        // IOUtils::read_csv_vec(path, out);
        IOUtils::read_vector_npy(path, out);
        // IOUtils::read_binary_vector(path, out);
        uint32_t i;
        for (i = 0; i < N_COMPONENTS; i++){
            out[i] = std::sqrt(out[i]);
        }
    }

    void Preprocessor::multilabel_binarize(std::vector<std::vector<float>>& buf,std::vector<std::string>& seqs,std::multimap<std::string,std::vector<std::string>>& dic){
        int N = seqs.size();
        for(int i=0;i<N;i++){
            auto iter = dic.find(std::to_string(i));
            std::vector<std::string> nucls=iter->second;
            buf[i].resize(nucls.size()*MAX_SEQ);
            for(int j =0;j<nucls.size()*MAX_SEQ;j+=nucls.size()){
                auto iter=std::find(nucls.begin(),nucls.end(),seqs[i]);
                if(iter!=nucls.end())buf[i][j+(iter-nucls.begin())]=1;
            }
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
        float v;
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
                               const std::string path_pca_variance, 
                               const std::string path_dictionary)
    {
        // std::multimap<std::string,std::vector<std::string>> dic = Preprocessor::load_dictionary(path_dictionary);

        sqrt_variance.resize(N_COMPONENTS);
        mean.resize(X_COLUMN);
        components.resize(N_COMPONENTS * X_COLUMN);

        std::cout << "load mean" << std::endl;
        Preprocessor::load_mean(path_pca_mean, mean);
        
        std::cout << "load variance" << std::endl;
        Preprocessor::load_variance(path_pca_variance, sqrt_variance);

        std::cout << "load components" << std::endl;
        Preprocessor::load_components(path_pca_components, components);
    }

    void Preprocessor::pca(std::array<float, X_COLUMN> &X, std::vector<float> &out){
        /* in-place subtraction to X's rows by mean */
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
        std::cout << "start load seq" << std::endl;
        Preprocessor::load_seq(data_path,sequences);
        std::cout << "load ok" << std::endl;
        uint32_t BATCH = sequences.size();
        std::cout << "batch size:" << BATCH << std::endl;

        /* MultiLabelBinalize */
        std::vector<std::vector<float>> X(BATCH, std::vector<float>(N_COMPONENTS));
        std::cout << "binarize" << std::endl;
        Preprocessor::binarize_by_existance(X, sequences, rnaMap, BATCH);
        return X;
    }


    std::vector<float> Preprocessor::dot(std::vector<float> &x, std::vector<std::vector<float>> &m){
        uint32_t i;
        std::vector<float> result(N_COMPONENTS);
        for (i = 0; i < N_COMPONENTS; i++){
            result[i] = std::inner_product(m[i].begin(), m[i].end(), x.begin(), 0.f);
        }
        return result;
    }

    std::vector<std::vector<float>> Preprocessor::batch_dot(std::vector<std::vector<float>> &x, std::vector<std::vector<float>> &m){
        uint32_t i, j, k, N = x.size();
        // std::cout << N << ", " << x[0].size() << ", " << m.size() << ", " << m[0].size() << std::endl;
        std::vector<std::vector<float>> result(N, std::vector<float>(N_COMPONENTS));
        float summed;
        for (i = 0; i < N; i++){
            // for (j = 0; j < N_COMPONENTS; i++){
            //     result[i][j] = std::inner_product(m[j].begin(), m[j].end(), x[i].begin(), .0f);
            // }
            for (j = 0; j < N_COMPONENTS; j++){
                summed = 0.f;
                for (k = 0; k < X_COLUMN; k++){
                    summed += m[j][k] * x[i][k];
                }
                result[i][j] = summed;
            }
        }
        return result;
    }
}