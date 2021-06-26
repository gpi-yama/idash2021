cmake_minimum_required(VERSION 3.12)

project(SEALExamples VERSION 3.6.2 LANGUAGES CXX)

# If not called from root CMakeLists.txt
if(NOT DEFINED SEAL_BUILD_EXAMPLES)
    set(SEAL_BUILD_EXAMPLES ON)

    # Import Microsoft SEAL
    find_package(SEAL 3.6.2 EXACT REQUIRED)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
endif()

if(SEAL_BUILD_EXAMPLES)
    add_executable(sealexamples)

    target_sources(sealexamples
        PRIVATE
            main.cpp
            server/ServerSide.cpp
            client/ClientSide.cpp
            postprocess/SigmoidPostprocessor.cpp
            preprocess/DummyPreprocessor.cpp
            utils/IOUtils.cpp
            utils/mathUtils.cpp
            ml/algebra.cpp
            ml/LogisticRegression.cpp
            ml/ML.h
            preprocess/PreProcessor.h
            postprocess/PostProcessor.h
    )

    if(TARGET SEAL::seal)
        target_link_libraries(sealexamples PRIVATE SEAL::seal)
    elseif(TARGET SEAL::seal_shared)
        target_link_libraries(sealexamples PRIVATE SEAL::seal_shared)
    else()
        message(FATAL_ERROR "Cannot find target SEAL::seal or SEAL::seal_shared")
    endif()
endif()