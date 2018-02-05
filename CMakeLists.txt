cmake_minimum_required(VERSION 3.3)
project(arith++ CXX)
enable_testing()

include_directories(.)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(Gmp REQUIRED)
include_directories(${GMP_INCLUDE_DIR})
include_directories(${GMPXX_INCLUDE_DIR})

add_executable(arith++_test
        arith++/arith++.h
        test/internal_test.cpp
        test/int_test.cpp
        test/rational_test.cpp
        test/gmp_test.cpp
        test/test_main.cpp
        test/Rational.cpp)
set_property(TARGET arith++_test PROPERTY CXX_STANDARD 14)
set_property(TARGET arith++_test PROPERTY CXX_STANDARD_REQUIRED On)
target_link_libraries(arith++_test ${GMP_LIB} ${GMPXX_LIB})
add_test(Test_arith++ arith++_test)
