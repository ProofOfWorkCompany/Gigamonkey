cmake_minimum_required(VERSION 3.1)
project(PackageTest CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(conan_integration_test conan_integration_test.cpp)
target_link_libraries(conan_integration_test ${CONAN_LIBS})

# Set C++ version
target_compile_features(conan_integration_test PUBLIC cxx_std_17)
set_target_properties(conan_integration_test PROPERTIES CXX_EXTENSIONS OFF)
