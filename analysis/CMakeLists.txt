cmake_minimum_required( VERSION 2.8 )

project( analysis )

#SET( CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(REMOVE_ITEM CEF_CXX_COMPILER_FLAGS
    "-std=c++14")

#find_package(ROOT REQUIRED)
#include(${ROOT_USE_FILE})
#----------------------------------------------------------------------------
# Find ROOT
IF(NOT DEFINED ROOT_FOUND)
  message("Root not found... Let's try Find_package ;)")
  find_package(ROOT CONFIG REQUIRED)
  #find_package(ROOT COMPONENTS MathMore Matrix Hist Tree Graf Gpad)
  IF(NOT ${ROOT_FOUND})
      message("Root not found... have to try an other way ;)")
      include($ENV{ROOTSYS}/etc/root/cmake/FindROOT.cmake)
    include_directories( PUBLIC ${ROOT_INCLUDE_DIRS})
    link_libraries( ${ROOT_LIBRARIES})
  ELSE(NOT ${ROOT_FOUND})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")
      include(${ROOT_USE_FILE})
    include_directories(PUBLIC ${ROOT_INCLUDE_DIRS})
    link_libraries( ${ROOT_LIBRARIES})
  ENDIF()
ENDIF()
#-----------------------------------------------------------------------------


include_directories( include )
file( GLOB SOURCES "src/*.cpp" )

add_executable( ProcessTrack ProcessTrack.cpp ${SOURCES} )
target_link_libraries( ProcessTrack ${ROOT_LIBRARIES} )

add_executable( RockSpecAnalyzer RockSpecAnalyzer.cpp )
target_link_libraries( RockSpecAnalyzer ${ROOT_LIBRARIES} )

add_executable( EnergyDepVSIntialEnergy EnergyDepVSIntialEnergy.cpp )
target_link_libraries( EnergyDepVSIntialEnergy ${ROOT_LIBRARIES} )

add_executable( PlotSpectra PlotSpectra.cpp )
target_link_libraries( PlotSpectra ${ROOT_LIBRARIES} )
