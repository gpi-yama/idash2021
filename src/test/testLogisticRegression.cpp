#include <gtest/gtest.h>
#include "../common.h"
#include "../client/ClientSide.h"
#include "../ml/LogisticRegression.h"

using namespace capsuleGene;

class LogisticRegressionTest : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        client.generate_keys(scale, modulus, poly_modulus_degree);

        for (auto v : inp)
        {
            enc_inp.push_back(client.encrypt(v));
        };

        ml = LogisticRegression(input_dim, output_dim, scale, slot_size, client.getEvaluator(), client.getEncryptor(), client.getEncoder(), client.getGalKey(), client.getRelinKeys());
        ml.set_weight(weight);
        ml.set_bias(bias);
    }

    const uint32_t input_dim = 6;
    const uint32_t output_dim = 4;
    const int slot_size = 4096;

    // virtual void TearDown() {}
    LogisticRegression ml;
    ClientSide client;
    const double scale = pow(2.0, 40);
    const int poly_modulus_degree = 8192;
    const std::vector<int32_t> modulus = {60, 40, 60};

    const std::vector<std::vector<double>> weight =
        {{1, 2, 1, 2, 1, 2},
         {1, 2, 1, 2, 1, 2},
         {1, 1, 1, 1, 1, 2},
         {1, 1, 1, 1, 1, 2}};

    const std::vector<double> bias = {1., 1., 1., 1.};

    const std::vector<std::vector<double>>
        inp =
            {{1., 1., 1., 1., 1., 2.},
             {1., 1., 1., 1., 1., 2.},
             {1., 1., 1., 1., 1., 2.},
             {1., 1., 1., 1., 1., 2.}};

    const std::vector<std::vector<double>> ans = {
        {12, 12, 10, 10},
        {12, 12, 10, 10},
        {12, 12, 10, 10},
        {12, 12, 10, 10}};

    std::vector<Ciphertext>
        enc_inp;
};

TEST_F(LogisticRegressionTest, predict)
{
    std::vector<std::vector<Ciphertext>> result = ml.predict(enc_inp);
    int i = 0;
    for (auto v : result)
    {
        std::vector<double> out = client.decrypt(v);
        for (int j = 0; j < output_dim; j++)
        {
            EXPECT_FLOAT_EQ(out[j], ans[i][j]);
        }
        i++;
    }
};
