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
        static std::vector<std::vector<double>> read_csv(const std::string file_path, const bool header = true, const bool index = true);
    };

}
