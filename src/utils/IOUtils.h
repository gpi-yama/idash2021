#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <array>
#include <memory>
#include "../constants.h"
#include "npy.h"

// using namespace NTL;

namespace capsuleGene
{

    class IOUtils
    {
        /*
    This class is IO purpose,
    takes care of print, writing out to file, etc

*/
    public:
        static std::vector<std::vector<float>> read_csv(const std::string file_path, const bool header = false, const bool index = false, char delimiter = ',');
        static std::vector<float> read_csv_vec(const std::string file_path);


        static void read_csv(const std::string file_path, std::vector<std::vector<float>> &out, const bool header = false, const bool index = false, char delimiter = ',');
        static void read_csv_vec(const std::string file_path, std::vector<float> &out);

        static void read_vector_npy(const std::string path, std::vector<float> &out);
        static void read_matrix_npy(const std::string path, std::vector<std::vector<float>> &out);

        static void read_binary_vector(const std::string path, std::vector<float> &x);

        static void write_binary_vector(const std::string path, std::vector<float> &x);  
    };

}
