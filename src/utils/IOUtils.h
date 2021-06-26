#include <vector>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
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
        static std::vector<std::vector<double>> read_csv(string file_path, bool header = true, bool index = true);
    };

}
