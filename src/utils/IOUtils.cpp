#include "IOUtils.h"

namespace capsuleGene
{
    std::vector<std::vector<float>> IOUtils::read_csv(const std::string file_path, const bool header, const bool index, char delimiter)
    {
        std::vector<std::vector<float>> csv;
        int index_col = -1, header_row = -1, row_count = 0;
        std::string line, cell;
        if (header)
            header_row = 0;
        if (index)
            index_col = 0;

        try
        {
            std::ifstream f(file_path);
            f.exceptions(std::ifstream::badbit);
            while (f.good())
            {
                std::getline(f, line);
                if (line.empty())
                    break;

                std::vector<float> row;
                int col_count = 0;
                std::stringstream linestream(line);
                if (row_count != header_row)
                {
                    while (std::getline(linestream, cell, delimiter))
                    {
                        if (col_count != index_col)
                        {
                            row.push_back(stof(cell));
                        }
                        col_count++;
                    }
                    csv.push_back(row);
                }
                row_count++;
            }
            f.close();
        }
        catch (std::ios_base::failure const &e)
        {
            throw std::ios_base::failure(std::string("fatal error occured while opening file.") + e.what());
        }
        catch (std::exception const &e)
        {
            throw std::runtime_error(std::string("runtime exception occured while reading file") + e.what());
        }

        return csv;
    }

    void IOUtils::read_csv(const std::string file_path, std::vector<std::vector<float>> &out, const bool header, const bool index, char delimiter)
    {
        int index_col = -1, header_row = -1, row_count = 0;
        std::string line, cell;
        if (header)
            header_row = 0;
        if (index)
            index_col = 0;

        try
        {
            std::ifstream f(file_path);
            f.exceptions(std::ifstream::badbit);
            while (f.good())
            {
                std::getline(f, line);
                if (line.empty())
                    break;

                std::vector<float> row;
                int col_count = 0;
                std::stringstream linestream(line);
                if (row_count != header_row)
                {
                    while (std::getline(linestream, cell, delimiter))
                    {
                        if (col_count != index_col)
                        {
                            out[row_count][col_count] = stof(cell);
                        }
                        col_count++;
                    }
                }
                row_count++;
            }
            f.close();
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

    std::vector<float> IOUtils::read_csv_vec(const std::string file_path)
    {
        std::vector<float> csv;
        int row_count = 0;
        std::string line, cell;
        try
        {
            std::ifstream f(file_path);
            f.exceptions(std::ifstream::badbit);
            while (f.good())
            {
                std::getline(f, line);
                if (line.empty())
                    break;

                std::vector<float> row;
                csv.push_back(stof(line));
                row_count++;
            }
            f.close();
        }
        catch (std::ios_base::failure const &e)
        {
            throw std::ios_base::failure(std::string("fatal error occured while opening file.") + e.what());
        }
        catch (std::exception const &e)
        {
            throw std::runtime_error(std::string("runtime exception occured while reading file") + e.what());
        }

        return csv;
    }

    void IOUtils::read_csv_vec(const std::string file_path, std::vector<float> &out)
    {
        int row_count = 0;
        std::string line, cell;
        try
        {
            std::ifstream f(file_path);
            f.exceptions(std::ifstream::badbit);
            while (f.good())
            {
                std::getline(f, line);
                if (line.empty())
                    break;

                std::vector<float> row;
                out[row_count] = stof(line);
                row_count++;
            }
            f.close();
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

    void IOUtils::read_vector_npy(const std::string path, std::vector<float> &out){
        std::vector<unsigned long> shape;
        bool fortran_order;
        npy::LoadArrayFromNumpy(path, shape, fortran_order, out);
    }


    void IOUtils::read_vector_npy(const std::string path, std::vector<int> &out){
        std::vector<unsigned long> shape;
        bool fortran_order;
        npy::LoadArrayFromNumpy(path, shape, fortran_order, out);
    }

    void IOUtils::read_binary_vector(const std::string path, std::vector<float> &vec){
        std::ifstream fin(path, std::ios::in | std::ios::binary);
        fin.read((char*)&vec[0], vec.size()*sizeof(vec[0]));
        fin.close();
    }
    
    void IOUtils::write_binary_vector(const std::string path, std::vector<float> &vec){
        std::ofstream fout(path, std::ios::out | std::ios::binary);
        fout.write((char*)&vec[0], vec.size()*sizeof(vec[0]));
        fout.close();
    }


    void IOUtils::read_binary_vector2d(const std::string path, std::vector<std::vector<float>> &vec2d, uint32_t num_row, uint32_t num_col){
        std::ifstream file(path, std::ios::in | std::ofstream::binary);
        for(uint32_t i = 0; i < num_row; i++ )
        {
            file.read((char*)&vec2d[i][0], num_col*sizeof(vec2d[i][0]));
        }
        file.close();
    }
    
    void IOUtils::write_binary_vector2d(const std::string path, std::vector<std::vector<float>> &vec2d){
        std::ofstream file(path, std::ios::out | std::ofstream::binary);
        uint32_t N = vec2d.size();
        uint32_t M = vec2d[0].size();

        for(size_t i = 0; i < N; i++ )
        {
            file.write((char*)&vec2d[i][0], M*sizeof(vec2d[i][0]));
        }
        file.close();
    }

    void IOUtils::write_prob_to_file(std::vector<std::vector<double>> &x, std::string filename){
        std::ofstream myfile(filename);
        uint32_t i, j;
        uint32_t N = x.size();
        uint32_t M = x[0].size();

        myfile << "index, >B.1.526, >B.1.1.7, >B.1.427, >P.1" << std::endl;
        for (i = 0; i < N; i++)
        {
            myfile << i << ", ";
            for (j = 0; j < M; j++)
            {
                myfile << x[i][j];
                if (j < M - 1) {
                    myfile << ", ";
                }
            }
            myfile << std::endl;
        }
        myfile.close();
    }

    void IOUtils::write_label_to_file(std::vector<std::string> &x, std::string filename){
        std::ofstream myfile(filename);
        int vsize = x.size();
        for (int n=0; n<vsize; n++)
        {
            myfile << x[n] << std::endl;
        }
        myfile.close();
    }

    void IOUtils::write_batch_size_to_file(int batch_size, std::string filename){
        std::ofstream myfile(filename);
        myfile << batch_size << std::endl;
        myfile.close();
    }

    int IOUtils::load_batch_size_from_file(std::string filename){
        std::ifstream file(filename);
        std::string line;
        std::getline(file, line);
        file.close();
        return stoi(line);
    }
}