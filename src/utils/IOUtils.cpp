#include "IOUtils.h"

namespace capsuleGene
{
    std::vector<std::vector<double>> IOUtils::read_csv(const std::string file_path, const bool header, const bool index)
    {
        std::vector<std::vector<double>> csv;
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

                std::vector<double> row;
                int col_count = 0;
                std::stringstream linestream(line);
                if (row_count != header_row)
                {
                    while (std::getline(linestream, cell, ','))
                    {
                        if (col_count != index_col)
                        {
                            row.push_back(stod(cell));
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

    std::vector<double> IOUtils::read_csv_vec(const std::string file_path)
    {
        std::vector<double> csv;
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

                std::vector<double> row;
                int col_count = 0;
                csv.push_back(stod(line));
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
}