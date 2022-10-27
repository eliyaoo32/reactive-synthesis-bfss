cmake_minimum_required(VERSION 3.21)
project(reactive-synthesis-bfss)

set(CMAKE_CXX_STANDARD 17)

add_executable(find_dependencies find_dependencies.cpp variable_dependency.cpp reactive_specification.cpp utils.cpp)


# Include BDDX package
find_library(bddx_location NAMES libbddx.so
            PATHS   ${SPOT_PREFIX}/lib
                    /usr/lib
                    /usr/lib64
                    /usr/local/lib
                    /usr/local/lib64 )
if(NOT bddx_location)
    message(FATAL_ERROR "BDDX library not found")
endif()
message(STATUS "Found bddx location at ${bddx_location}")
add_library(bddx STATIC IMPORTED)
set_target_properties(bddx PROPERTIES IMPORTED_LOCATION ${bddx_location})
target_link_libraries( find_dependencies bddx )

# Include spot package
find_library(spot_location NAMES libspot.so
        PATHS   ${SPOT_PREFIX}/lib
        /usr/lib
        /usr/lib64
        /usr/local/lib
        /usr/local/lib64 )
if(NOT spot_location)
    message(FATAL_ERROR "Spot library not found")
endif()
message(STATUS "Found spot location at ${spot_location}")
add_library(spot STATIC IMPORTED)
set_target_properties(spot PROPERTIES IMPORTED_LOCATION ${spot_location})
target_link_libraries( find_dependencies spot )

# Add headers
# TODO: find the headers of Spot and include them instead of including directly from /usr/local/include
include_directories(/usr/local/include)
include_directories(${SPOT_PREFIX}/include)
