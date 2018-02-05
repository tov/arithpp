cmake_minimum_required(VERSION 3.3)
project(int++ CXX)
enable_testing()

include_directories(.)
include_directories(3rdparty)

add_subdirectory(3rdparty/rapidcheck)

add_executable(int++_test
        int++/intpp.h
        test/internal_test.cpp
        test/int_test.cpp
        test/rational_test.cpp
        test/test_main.cpp
        test/Rational.cpp)

set_property(TARGET int++_test PROPERTY CXX_STANDARD 14)
set_property(TARGET int++_test PROPERTY CXX_STANDARD_REQUIRED On)
add_test(Test_int++ int++_test)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
find_package(Gmp)
if (GMP_FOUND)
    include_directories(${GMP_INCLUDE_DIR})
    include_directories(${GMPXX_INCLUDE_DIR})
    add_executable(random_test
            int++/intpp.h
            test/gmp_test.cpp)
    set_property(TARGET random_test PROPERTY CXX_STANDARD 14)
    set_property(TARGET random_test PROPERTY CXX_STANDARD_REQUIRED On)
    target_link_libraries(random_test ${GMP_LIB} ${GMPXX_LIB})
    target_link_libraries(random_test rapidcheck)
    add_test(Random_test random_test)
endif (GMP_FOUND)

