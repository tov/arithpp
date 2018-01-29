cmake_minimum_required(VERSION 3.3)
project(arith++ CXX)

enable_testing()

include_directories(.)

add_executable(arith++_test test/arithmetic_test.cpp)
set_property(TARGET arith++_test PROPERTY CXX_STANDARD 14)
set_property(TARGET arith++_test PROPERTY CXX_STANDARD_REQUIRED On)
