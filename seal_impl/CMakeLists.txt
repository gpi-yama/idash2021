cmake_minimum_required(VERSION 3.12)

project(capsuleflow VERSION 1.0.0 LANGUAGES CXX C)


#set(CMAKE_CXX_FLAGS "-g -O0 -fpermissive -fopenmp  -lgmp")
set(CMAKE_CXX_FLAGS "-g -O0 -fpermissive -fopenmp ")


find_package(Threads REQUIRED)

# Import Microsoft SEAL
find_package(SEAL 3.6.5 EXACT REQUIRED)

add_executable(lr lr.cpp)
target_link_libraries(lr SEAL::seal Threads::Threads)
target_include_directories(lr PRIVATE)

add_executable(knn knn.cpp)
target_link_libraries(knn SEAL::seal Threads::Threads)
target_include_directories(knn PRIVATE)
