cmake_minimum_required(VERSION 3.3)
project(xxint_test CXX)
enable_testing()

set(cxx_std_props
        CXX_STANDARD            14
        CXX_STANDARD_REQUIRED   On
        CXX_EXTENSIONS          Off)

include_directories(../xxint)

add_executable(xxint_test
        internal_test.cxx
        int_test.cxx
        rational_test.cxx
        test_main.cxx
        Rational.cxx)
add_test(Test_xxint xxint_test)
target_include_directories(xxint_test PRIVATE 3rdparty/catch)
set_target_properties(xxint_test PROPERTIES ${cxx_std_props})

# If we have GMP then we can do random testing using rapidcheck.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
find_package(Gmp)
if (GMP_FOUND)
    add_subdirectory(3rdparty/rapidcheck)
    add_executable(random_test gmp_test.cxx)
    add_test(Random_test random_test)
    target_include_directories(random_test
            PRIVATE
            3rdparty/rapidcheck
            ${GMPXX_INCLUDE_DIR}
            ${GMP_INCLUDE_DIR})
    target_link_libraries(random_test
            ${GMP_LIB}
            ${GMPXX_LIB}
            rapidcheck)
    set_target_properties(random_test PROPERTIES ${cxx_std_props})
endif (GMP_FOUND)

