#include "common.h"
#include "server/ServerSide.h"
#include "client/ClientSide.h"
#include "preprocess/DummyPreprocessor.h"

using namespace capsuleGene;

int main()
{
    // ----------- defin parameters -------------
    double scale = pow(2.0, 40);
    int poly_modulus_degree = 4096;
    std::vector<int32_t> modulus = {50, 30, 50};

    // ------------------------------------------
    Preprocessor *preprocessor = new DummyPreprocessor();
    ClientSide client(*preprocessor);
    client.generate_keys(scale, modulus, poly_modulus_degree);
}