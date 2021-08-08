#pragma once
#include <iostream>
#include <vector>
#include <cstring>
#include <set>
#include <unordered_map>
#include <map>
#include <numeric>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iterator>
#include <string>
#include <cmath>
#include <memory>
#include "../utils/IOUtils.h"
#include "../constants.h"

const std::string DATA_PATH = "Data/Challenge.fa";
const std::string BASIS_PATH = "Challenge/basis.csv";

#include <cassert>

namespace capsuleGene
{

    static const std::string rnaChar = "ACGTRYSWKMBDHVNO";

    class Preprocessor
    {
    private:
        std::multimap<std::string,std::vector<std::string>> dic;
        std::vector<float> components;
        std::vector<float> mean;
        std::vector<float> sqrt_variance;


        std::unordered_map<char, int> rnaMap{
        //     A      C      G      T
        {'A', 0},
        {'C', 1},
        {'G', 2},
        {'T', 3},
        {'R', 4},   // A or G
        {'Y', 5}, // C or T
        {'S', 6},  // G or C
        {'W', 7},  // A or T
        {'K', 8},  // G or T
        {'M', 9},  // A or C
        {'B', 10},  // C or G or T
        {'D', 11},  // A or G or T
        {'H', 12},  // A or C or T
        {'V', 13},  // A or C or G
        {'N', 14},  // any base
        {'O', 15},  // NONE
        };


        // std::unordered_map<char, std::array<float, 4>> rnaMap{
        // //     A      C      G      T
        // {'A', {1.f,   0.f,   0.f,   0.f}},
        // {'C', {0.f,   1.f,   0.f,   0.f}},
        // {'G', {0.f,   0.f,   1.f,   0.f}},
        // {'T', {0.f,   0.f,   0.f,   1.f}},
        // {'R', {0.5f,  0.f,   0.5f,  0.f}},   // A or G
        // {'Y', {0.f,   0.5f,  0.f,   0.5f}}, // C or T
        // {'S', {0.f,   0.5f,  0.5f,  0.f}},  // G or C
        // {'W', {0.5f,  0.f,   0.f,   0.5f}},  // A or T
        // {'K', {0.f,   0.f,   0.5f,  0.5f}},  // G or T
        // {'M', {0.5f,  0.5f,  0.f,   0.f}},  // A or C
        // {'B', {0.f,   0.33f, 0.33f, 0.33f}},  // C or G or T
        // {'D', {0.33f, 0.f,   0.33f, 0.33f}},  // A or G or T
        // {'H', {0.33f, 0.33f, 0.f,   0.33f}},  // A or C or T
        // {'V', {0.33f, 0.33f, 0.33f, 0.f}},  // A or C or G
        // {'N', {0.25f, 0.25f, 0.25f, 0.25f}},  // any base
        // {'O', {0.f,   0.f,   0.f,   0.f}},  // NONE
        // };

        // std::unordered_map<char, std::array<float, 4>> rnaMap{
        // //     A    C    G    T
        // {'A', {1,   0,   0,   0}},
        // {'C', {0,   1,   0,   0}},
        // {'G', {0,   0,   1,   0}},
        // {'T', {0,   0,   0,   1}},
        // {'R', {1, 0,   1, 0}},   // A or G
        // {'Y', {0,   1, 0,   1}}, // C or T
        // {'S', {0,   1, 1, 0}},  // G or C
        // {'W', {1, 0,   0,   1}},  // A or T
        // {'K', {0,   0,   1, 1}},  // G or T
        // {'M', {1, 1, 0,   0}},  // A or C
        // {'B', {0,   1,1,1}},  // C or G or T
        // {'D', {1,0,   1,1}},  // A or G or T
        // {'H', {1,1,0,   1}},  // A or C or T
        // {'V', {1,1,1,0}},  // A or C or G
        // {'N', {1,1,1,1}},  // any base
        // {'O', {0.,  0.,  0.  ,0.}},  // NONE
        // };


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
        static void load_seq(const std::string path, std::vector<std::string>& sequences);

        /**
         * @brief load PCA components
         * 
         * @param ifs 
         * @param m 
         */
        static void load_components(std::string path, std::vector<float> &out);
        
        /**
         * @brief load PCA means
         * 
         * @param ifs 
         * @param m 
         */
        static void load_mean(std::string path, std::vector<float> &out);

        /**
         * @brief load pca variance
         * 
         * @param ifs 
         * @param m 
         */
        static void load_variance(std::string path, std::vector<float> &out);

        static void multilabel_binarize(std::vector<std::vector<float>>& buf,std::vector<std::string>& seqs,std::multimap<std::string,std::vector<std::string>>& dic);

        void binarize_by_existance(std::vector<std::vector<float>>& buf, std::vector<std::string>& seqs, std::unordered_map<char, int> rnaMap, const uint32_t batch_size);

        static std::vector<float> dot(std::vector<float> &x, std::vector<std::vector<float>> &m);

        static std::vector<std::vector<float>> batch_dot(std::vector<std::vector<float>> &x, std::vector<std::vector<float>> &m);

    public:
        Preprocessor(){
            // delete mean;
            // delete sqrt_variance;
            // delete components;
        };
        Preprocessor(const std::string path_pca_components, 
                     const std::string path_pca_mean, 
                     const std::string path_pca_variance, 
                     const std::string path_dictionary);

        std::vector<std::vector<float>> process(const std::string data_path);

        void load_and_binarize(const std::string data_path, std::vector<std::vector<float>> &X);

        void pca(std::array<float, X_COLUMN> &X, std::vector<float> &out);
    };
}