#include <gtest/gtest.h>
#include "../common.h"
#include "testLogisticRegression.cpp"

using namespace capsuleGene;

class AlgebraTest : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        client.generate_keys(scale, modulus, poly_modulus_degree);
    }

    // virtual void TearDown() {}
    ClientSide client;
    const double scale = pow(2.0, 40);
    const int poly_modulus_degree = 8192;
    const std::vector<int32_t> modulus = {60, 40, 60};
};

TEST_F(AlgebraTest, rotate_and_sum_in_col)
{
    std::vector<double> v = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2};
    int ans = 33;
    Ciphertext enc_v = client.encrypt(v);
    Ciphertext enc_result = AlgebraUtils::rotate_and_sum_in_col(enc_v, v.size(), client.getEvaluator(), client.getGalKey(), client.getEncoder(), poly_modulus_degree, scale);
    std::vector<double> result = client.decrypt(enc_result);
    EXPECT_FLOAT_EQ(result[0], ans);
};

TEST_F(AlgebraTest, add)
{
    std::vector<double> v = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5};
    std::vector<float> ans = {2, 4, 6, 8, 10, 2, 4, 6, 8, 10};
    Ciphertext enc_v = client.encrypt(v);
    Ciphertext enc_result = AlgebraUtils::add(enc_v, v, client.getEvaluator(), client.getEncoder(), scale);
    std::vector<double> result = client.decrypt(enc_result);
    for (int i = 0; i < v.size(); i++)
    {
        EXPECT_FLOAT_EQ(result[i], ans[i]);
    }
};

TEST_F(AlgebraTest, multiply)
{
    std::vector<double> v = {1, 2, 3, 4, 5};
    std::vector<double> ans = {1, 4, 9, 16, 25};
    Ciphertext enc_v = client.encrypt(v);
    Ciphertext enc_result = AlgebraUtils::multiply(enc_v, v, client.getEvaluator(), client.getRelinKeys(), client.getEncoder(), scale);
    std::vector<double> result = client.decrypt(enc_result);
    for (int i = 0; i < v.size(); i++)
    {
        EXPECT_FLOAT_EQ(result[i], ans[i]);
    }
};

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
