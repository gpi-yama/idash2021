#include "../common.h"
#include "../postprocess/PostProcessor.h"

namespace capsuleGene
{
    class ML
    {
    public:
        virtual ~ML(){};
        virtual std::vector<Ciphertext> predict(std::vector<Ciphertext> &x);
        virtual Postprocessor gen_postprocessor();

    private:
        uint32_t numFeatures;
        uint32_t featureDim;
    };
}