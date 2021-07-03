cmake_minimum_required(VERSION 3.12)

project(SEALExamples VERSION 3.6.2 LANGUAGES CXX)

# If not called from root CMakeLists.txt
set(CMAKE_CXX_STANDARD 20)

# Import Microsoft SEAL
find_package(SEAL 3.6.2 EXACT REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "-g -O3 -fpermissive -fopenmp")
add_executable(sealexamples)

target_sources(sealexamples
    PRIVATE
        main.cpp
        server/ServerSide.cpp
        client/ClientSide.cpp
        postprocess/SoftmaxPostprocessor.cpp
        preprocess/DummyPreprocessor.cpp
        utils/IOUtils.cpp
        utils/mathUtils.cpp
        ml/algebra.cpp
        ml/LogisticRegression.cpp
)

target_link_libraries(sealexamples PRIVATE SEAL::seal)
