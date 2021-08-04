#pragma once
#include <iostream>
#include <vector>
#include <cstring>
#include <set>
#include <map>
#include <numeric>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iterator>
#include <string>
#include <cmath>


#define EIGEN_NO_DEBUG 
#define EIGEN_DONT_VECTORIZE
#define EIGEN_DONT_PARALLELIZE
#define EIGEN_MPL2_ONLY 
#include <Eigen/Core>
#include <Eigen/LU>


constexpr unsigned int DATASET = 8000;
constexpr unsigned int BATCH = DATASET;
constexpr unsigned int X_COLUMN = 141157;
constexpr unsigned int Y_COLUMN = 4;

constexpr unsigned int N_COMPONENTS = 200;
constexpr unsigned int K = 1;

constexpr double P = 0.1;
constexpr unsigned int TRAIN_SIZE = BATCH*(1-P);
constexpr unsigned int MAX_SEQ = 30000;

const std::string DATA_PATH = "Data/Challenge.fa";
const std::string BASIS_PATH = "Challenge/basis.csv";

namespace capsuleGene
{
    class Preprocessor
    {
    private:
        std::multimap<std::string,std::vector<std::string>> dic;
        Eigen::Matrix2f components;
        Eigen::VectorXf mean;
        Eigen::VectorXf sqrt_variance;
        int N_COMPONENTS;
        int X_COLUMN;

    private:

        /**
         * @brief split string into vec
         * 
         * @param s 
         * @param delim 
         * @return std::vector<std::string> 
         */
        static std::vector<std::string> split(const std::string &s, char delim);

        /**
         * @brief split as loat
         * 
         * @param s 
         * @param delim 
         * @return std::vector<double> 
         */
        static std::vector<double> split_to_float(const std::string &s, char delim);

        /**
         * @brief load dict
         * 
         * @param filename 
         * @return std::multimap<std::string, std::vector<std::string>> 
         */
        static std::multimap<std::string, std::vector<std::string>> load_dictionary(std::string filename);

        /**
         * @brief load sequences
         * 
         * @param ifs 
         * @param sequences 
         * @param labels 
         */
        static void load_seq(std::ifstream& ifs,std::vector<std::string>& sequences,std::vector<std::string>& labels);

        /**
         * @brief load PCA components
         * 
         * @param ifs 
         * @param m 
         */
        static void load_components(std::ifstream& ifs,Eigen::Matrix2f& m, int N_COMPONENTS, int X_COLUMN);
        
        /**
         * @brief load PCA means
         * 
         * @param ifs 
         * @param m 
         */
        static void load_mean(std::ifstream& ifs,Eigen::VectorXf& m);

        /**
         * @brief load pca variance
         * 
         * @param ifs 
         * @param m 
         */
        static void load_variance(std::ifstream& ifs,Eigen::VectorXf& m);


        static void multilabel_binalize(std::vector<std::vector<bool>>& buf,std::vector<std::string>& seqs,std::multimap<std::string,std::vector<std::string>>& dic);
        
        static void onehot_encode(std::array<std::array<bool,Y_COLUMN>,BATCH>& buf,std::vector<std::string>& words);


    public:
        Preprocessor(){};
        Preprocessor(std::string path_pca_components, 
                     std::string path_pca_mean, 
                     std::string path_pca_variance, 
                     std::string path_dictionary,
                     const int N_COMPONENTS,
                     const int X_COLUMN);

        std::vector<std::vector<double>> process(const std::string data_path);
    };
}