cmake_minimum_required( VERSION 2.8 )

project( analysis )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(REMOVE_ITEM CEF_CXX_COMPILER_FLAGS
    "-std=c++14")

find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#message("${ROOT_LIBRARIES}")

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)

add_executable( RockSpecAnalyzer RockSpecAnalyzer.cpp ${sources}  )
target_link_libraries( RockSpecAnalyzer ${ROOT_LIBRARIES} )

add_executable( ProcessTrack ProcessTrack.cpp ${sources}  )
target_link_libraries( ProcessTrack ${ROOT_LIBRARIES} )

add_executable( EnergyDepVSIntialEnergy EnergyDepVSIntialEnergy.cpp ${sources}  )
target_link_libraries( EnergyDepVSIntialEnergy ${ROOT_LIBRARIES} )

add_executable( PlotSpectra PlotSpectra.cpp ${sources}  )
target_link_libraries( PlotSpectra ${ROOT_LIBRARIES} )
