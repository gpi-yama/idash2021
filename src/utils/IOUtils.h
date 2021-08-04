#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

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
        static std::vector<std::vector<double>> read_csv(const std::string file_path, const bool header = false, const bool index = false);
        static std::vector<double> read_csv_vec(const std::string file_path);
        static std::vector<double> read_vector_npy(const std::string path);
        static std::vector<std::vector<double>> read_matrix_npy(const std::string path);
    };

}
