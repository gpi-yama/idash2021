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

    void Preprocessor::load_seq(std::ifstream& ifs,std::vector<std::string>& sequences,std::vector<std::string>& labels){
        std::string line;
        for(int i=0;i<BATCH*2;i++){ 
            ifs >> line;
            if(line[0]=='>'){
                std::string label = Preprocessor::split(line,'_')[0].substr(1);
                labels.push_back(label);
            }else{
                int d = MAX_SEQ - line.size();
                line += std::string(d,'O');
                sequences.emplace_back(line);
            }
        }
        if(DATASET<sequences.size() || DATASET<labels.size()){
            std::cerr<<"Bad data"<<std::endl;
        }
    }

    void Preprocessor::load_components(std::ifstream& ifs,Eigen::Matrix2f& m, int N_COMPONENTS, int X_COLUMN){
        std::string row;
	    int i = 0;
    	for(i=0;i<N_COMPONENTS;i++){ 
            getline(ifs,row);
            std::vector<double> row_ = Preprocessor::split_to_float(row,' ');
            for(int j=0;j<X_COLUMN;++j){
                m(i,j) = row_[j];
            }
    	}
    }

    void Preprocessor::load_mean(std::ifstream& ifs,Eigen::VectorXf& m){
        std::string elm;
        for(int i=0;i<BATCH;i++){
            ifs >> elm;
            m(i)=std::stof(elm);
        }
    }
        
    void Preprocessor::load_variance(std::ifstream& ifs,Eigen::VectorXf& m){
        std::string elm;
        for(int i=0;i<BATCH;i++){
            ifs >> elm;
            m(i)=std::sqrt(std::stof(elm));
        }
    }

    void Preprocessor::multilabel_binalize(std::vector<std::vector<bool>>& buf,std::vector<std::string>& seqs,std::multimap<std::string,std::vector<std::string>>& dic){
        for(int i=0;i<BATCH;i++){
            auto iter = dic.find(std::to_string(i));
            std::vector<std::string> nucls=iter->second;
            buf[i].resize(nucls.size()*MAX_SEQ);
            for(int j =0;j<nucls.size()*MAX_SEQ;j+=nucls.size()){
                auto iter=std::find(nucls.begin(),nucls.end(),seqs[i]);
                if(iter!=nucls.end())buf[i][j+(iter-nucls.begin())]=1;
            }
        }
    }

    void Preprocessor::onehot_encode(std::array<std::array<bool,Y_COLUMN>,BATCH>& buf,std::vector<std::string>& words){
        for(int i=0;i<BATCH;i++){
            buf[i].fill(false);
            if(words[i]=="B.1.427"){
                buf[i][0]=true;
            }
            else if(words[i]=="B.1.1.7"){
                buf[i][1]=true;
            }
            else if(words[i]=="P.1"){
                buf[i][2]=true;
            }
            else {
                buf[i][3]=true;
            }
        }
    }
        
    Preprocessor::Preprocessor(std::string path_pca_components, 
                               std::string path_pca_mean, 
                               std::string path_pca_variance, 
                               std::string path_dictionary,
                               const int N_COMPONENTS,
                               const int X_COLUMN)
    {
        this->N_COMPONENTS = N_COMPONENTS;
        this->X_COLUMN = X_COLUMN;

        std::ifstream ifs2(path_pca_components,std::ios::in);
        std::ifstream ifs3(path_pca_mean,std::ios::in);
        std::ifstream ifs4(path_pca_variance,std::ios::in);
        std::multimap<std::string,std::vector<std::string>> dic = Preprocessor::load_dictionary("cols_from_python.txt");

        this->components.resize(N_COMPONENTS,X_COLUMN);
        Preprocessor::load_components(ifs2,components,N_COMPONENTS,X_COLUMN);
        
        this->mean.resize(X_COLUMN);
        Preprocessor::load_mean(ifs3, mean);
        
        this->sqrt_variance.resize(N_COMPONENTS);
        Preprocessor::load_variance(ifs4,sqrt_variance);

    }

    std::vector<std::vector<double>> Preprocessor::process(const std::string data_path){
    	int i,j, BATCH;

        std::ifstream ifs1(data_path, std::ios::in);
        
        /* Load Sequences */
        std::vector<std::string> sequences;
        std::vector<std::string> labels;
        Preprocessor::load_seq(ifs1,sequences,labels);

        BATCH = sequences.size();

        /* MultiLabelBinalize */
        std::vector<std::vector<bool>> X(BATCH);
        Preprocessor::multilabel_binalize(X, sequences,dic);

        Eigen::Matrix2f X_(BATCH,X_COLUMN);
        for(i=0;i<BATCH;++i){
            for(j=0;j<X_COLUMN;++j)X_(i,j)=X[i][j];
        }

        /* in-place subtraction to X's rows by mean */
        X_.rowwise() -= mean.transpose();

        /* pca transform */
        X_ = components * X_;
        
        /* actualy this operation is like normalization */
        X_.array().rowwise() /= sqrt_variance.transpose().array();

        std::vector<std::vector<double>> x_vec(BATCH, std::vector<double>(X_COLUMN));
        for(i = 0; i < BATCH; ++i){
            for(j = 0; j < X_COLUMN; ++j){
                x_vec[i][j] = X_(i, j);
            }
        }
        return x_vec;
    }

}