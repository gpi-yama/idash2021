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
                int col_count = 0;
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

    void IOUtils::read_matrix_npy(const std::string path, std::vector<std::vector<float>> &out){
        // std::vector<unsigned long> shape;
        // bool fortran_order;
        // std::vector<std::vector<double>> data;
        // npy::LoadArrayFromNumpy(path, shape, fortran_order, out);
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
}