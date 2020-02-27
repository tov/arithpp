cmake_minimum_required(VERSION 3.3)
project(xxint CXX)

add_subdirectory(test)

# If we have Doxygen then we can generate docs.
find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_subdirectory(docs)
endif()

