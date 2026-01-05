cmake_minimum_required(VERSION 3.15)
project(azurite)

file(GLOB sources src/*.cpp src/*.h)

target_compile_options(az PUBLIC -O3)

add_executable(az ${sources})