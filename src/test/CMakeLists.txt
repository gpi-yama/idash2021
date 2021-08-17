cmake_minimum_required(VERSION 3.12)

project(SEALExamples VERSION 3.6.2 LANGUAGES CXX)

# If not called from root CMakeLists.txt
if(NOT DEFINED SEAL_BUILD_EXAMPLES)
    set(SEAL_BUILD_EXAMPLES ON)

    # Import Microsoft SEAL
    find_package(SEAL 3.6.2 EXACT REQUIRED)
    find_package(PkgConfig)
    pkg_search_module(GTEST REQUIRED gtest_main)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
endif()

if(SEAL_BUILD_EXAMPLES)
    include(GoogleTest)
    
    set(CMAKE_CXX_FLAGS "-g -O1 -lgtest -lpthread")
    add_executable(algebra_test
            testAlgebra.cpp
            ../server/ServerSide.cpp
            ../client/ClientSide.cpp
            ../utils/IOUtils.cpp
            ../utils/mathUtils.cpp
            ../ml/algebra.cpp
            ../ml/LogisticRegression.cpp
            ../ml/ML.h
            ../preprocess/PreProcessor.h
            ../postprocess/PostProcessor.h
            ../common.h
    )

    
    gtest_discover_tests(algebra_test)
    if(TARGET SEAL::seal)
        target_link_libraries(
            algebra_test PRIVATE SEAL::seal
            Threads::Threads gtest gtest_main
        )
    elseif(TARGET SEAL::seal_shared)
        target_link_libraries(sealexamples PRIVATE SEAL::seal_shared)
    else()
        message(FATAL_ERROR "Cannot find target SEAL::seal or SEAL::seal_shared")
    endif()
endif()