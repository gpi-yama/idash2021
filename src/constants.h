#pragma once
#include <vector>
#include <iostream>
#include <cstdint>
#include <cmath>
#include <array>
#include <unordered_map>

static constexpr unsigned int X_COLUMN = 120000;
static constexpr unsigned int Y_COLUMN = 4;
static constexpr unsigned int N_COMPONENTS = 200;
static constexpr unsigned int MAX_SEQ = 30000;


static const double SCALE = pow(2.0, 30);
static const int POLY_MODULUS_DEGREE = 4096;
static const std::vector<int32_t> MODULUS = {38, 30, 38};


    

