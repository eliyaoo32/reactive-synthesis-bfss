cmake_minimum_required(VERSION 3.21)
project(reactive-synthesis-bfss)

set(CMAKE_CXX_STANDARD 17)

################## Add playground
FILE(GLOB find_dependencies_SRC *.cpp)
list(REMOVE_ITEM find_dependencies_SRC ${CMAKE_CURRENT_SOURCE_DIR}/playground.cpp)
FILE(GLOB playground_SRC *.cpp)
list(REMOVE_ITEM playground_SRC ${CMAKE_CURRENT_SOURCE_DIR}/find_dependencies.cpp)
add_executable(playground ${playground_SRC} )
################## Remove playground

add_executable(find_dependencies ${find_dependencies_SRC} )

# Include Boots Options package
find_library(boost_options_location NAMES libboost_program_options.a)
if(NOT boost_options_location)
    message(FATAL_ERROR "Boost-options library not found")
endif()
message(STATUS "Found BoostOptions location at ${boost_options_location}")
add_library(boost_options STATIC IMPORTED)
set_target_properties(boost_options PROPERTIES IMPORTED_LOCATION ${boost_options_location})
target_link_libraries( find_dependencies boost_options )

# Include Boots JSON package
find_library(boost_json_location NAMES libboost_json.a)
if(NOT boost_json_location)
    message(FATAL_ERROR "Boost-JSON library not found")
endif()
message(STATUS "Found Boost-JSON location at ${boost_json_location}")
add_library(boost_json STATIC IMPORTED)
set_target_properties(boost_json PROPERTIES IMPORTED_LOCATION ${boost_json_location})
target_link_libraries( find_dependencies boost_json )

# Include BDDX package
find_library(bddx_location NAMES libbddx.a)
if(NOT bddx_location)
    message(FATAL_ERROR "BDDX library not found")
endif()
message(STATUS "Found bddx location at ${bddx_location}")
add_library(bddx STATIC IMPORTED)
set_target_properties(bddx PROPERTIES IMPORTED_LOCATION ${bddx_location})
target_link_libraries( find_dependencies bddx )

# Include spot package
find_library(spot_location NAMES libspot.a)
if(NOT spot_location)
    message(FATAL_ERROR "Spot library not found")
endif()
message(STATUS "Found spot location at ${spot_location}")
add_library(spot STATIC IMPORTED)
set_target_properties(spot PROPERTIES IMPORTED_LOCATION ${spot_location})
target_link_libraries( find_dependencies spot )

# TODO: find a way to include the spot headers and boost headers
include_directories(/usr/local/include)

############### Playground packages
target_link_libraries( playground boost_options )
target_link_libraries( playground boost_json )
target_link_libraries( playground bddx )
target_link_libraries( playground spot )