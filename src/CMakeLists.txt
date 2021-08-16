cmake_minimum_required(VERSION 3.12)

project(IDASH2021TRACK2 VERSION 3.6.2 LANGUAGES CXX)

# If not called from root CMakeLists.txt
set(CMAKE_CXX_STANDARD 20)

# Import Microsoft SEAL
find_package(SEAL 3.6.5 EXACT REQUIRED)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "-O3 -march=native -mavx512f -Ofast -fpermissive -fopenmp")
# -g -msse4.2
add_executable(pred_rna)

target_sources(pred_rna
    PRIVATE
        main.cpp
        server/ServerSide.cpp
        client/ClientSide.cpp
        postprocess/SoftmaxPostprocessor.cpp
        preprocess/PreProcessor.cpp
        utils/IOUtils.cpp
        utils/mathUtils.cpp
        utils/npy.h
        ml/algebra.cpp
        ml/LogisticRegression.cpp
        ml/CoefLogisticRegression.cpp
        ml/MiharaLogisticRegression.hpp
)

target_link_libraries(pred_rna PRIVATE SEAL::seal
                    Eigen3::Eigen)
