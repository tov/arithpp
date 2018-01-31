cmake_minimum_required(VERSION 3.3)
project(arith++ CXX)
enable_testing()

add_subdirectory(unittest-cpp)

include_directories(.)
include_directories(unittest-cpp)

add_executable(arith++_test
        arith++/arith++.h
        test/internal_test.cpp
        test/int_test.cpp
        test/rational_test.cpp
        test/Rational.cpp)
set_property(TARGET arith++_test PROPERTY CXX_STANDARD 14)
set_property(TARGET arith++_test PROPERTY CXX_STANDARD_REQUIRED On)
target_link_libraries(arith++_test UnitTest++)
add_test(Test_arith++ arith++_test)
