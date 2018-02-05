cmake_minimum_required(VERSION 3.3)
project(arith++ CXX)
enable_testing()

include_directories(.)
include_directories(3rdparty)

add_subdirectory(3rdparty/rapidcheck)

add_executable(arith++_test
        arith++/arith++.h
        test/internal_test.cpp
        test/int_test.cpp
        test/rational_test.cpp
        ${GMP_TEST}
        test/test_main.cpp
        test/Rational.cpp)

set_property(TARGET arith++_test PROPERTY CXX_STANDARD 14)
set_property(TARGET arith++_test PROPERTY CXX_STANDARD_REQUIRED On)
add_test(Test_arith++ arith++_test)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(Gmp)
if (GMP_FOUND)
    include_directories(${GMP_INCLUDE_DIR})
    include_directories(${GMPXX_INCLUDE_DIR})
    add_executable(random_test
            arith++/arith++.h
            test/gmp_test.cpp)
    set_property(TARGET random_test PROPERTY CXX_STANDARD 14)
    set_property(TARGET random_test PROPERTY CXX_STANDARD_REQUIRED On)
    target_link_libraries(random_test ${GMP_LIB} ${GMPXX_LIB})
    target_link_libraries(random_test rapidcheck)
    add_test(Random_test random_test)
endif (GMP_FOUND)

