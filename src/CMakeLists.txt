cmake_minimum_required(VERSION 3.23)
project(reactive-synthesis-bfss)

set(CMAKE_CXX_STANDARD 17)


add_executable(reactive-synthesis-bfss find_dependents.cpp)

# Include spot package
find_library(spot_location NAMES libspot.a)
message(STATUS "Found spot location at ${spot_location}")
add_library(spot STATIC IMPORTED)
set_target_properties(spot PROPERTIES IMPORTED_LOCATION ${spot_location})
target_link_libraries( reactive-synthesis-bfss spot )

# Add headers
include_directories(/usr/local/include)

