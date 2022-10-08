cmake_minimum_required(VERSION 3.23)
project(reactive-synthesis-bfss)

set(CMAKE_CXX_STANDARD 17)

add_executable(reactive-synthesis-bfss find_dependents.cpp variable_dependency.cpp)


# Include BDDX package
# TODO: show error message if BDDX wasn't located
find_library(bddx_location NAMES libbddx.a)
message(STATUS "Found bddx location at ${bddx_location}")
add_library(bddx STATIC IMPORTED)
set_target_properties(bddx PROPERTIES IMPORTED_LOCATION ${bddx_location})
target_link_libraries( reactive-synthesis-bfss bddx )

# Include spot package
# TODO: show error message if spot wasn't located
find_library(spot_location NAMES libspot.a)
message(STATUS "Found spot location at ${spot_location}")
add_library(spot STATIC IMPORTED)
set_target_properties(spot PROPERTIES IMPORTED_LOCATION ${spot_location})
target_link_libraries( reactive-synthesis-bfss spot )

# Add headers
# TODO: find the headers of Spot and include them instead of including directly from /usr/local/include
include_directories(/usr/local/include)
