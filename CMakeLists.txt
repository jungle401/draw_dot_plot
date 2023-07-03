cmake_minimum_required (VERSION 3.16)

# project name
project(app)

add_definitions( -DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

# opencv
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( "/mnt/ec/ness/johnson/installs/boost/boost_1_79_0" )
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include/)

# openmp
FIND_PACKAGE(OpenMP REQUIRED)
if (OPENMP_FOUND)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-sign-compare")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# link your source code to your project
add_executable(app src/main.cpp)
find_package(fmt REQUIRED)

find_package(OpenMP REQUIRED)
if(OpenMP_FOUND)
    target_link_libraries(app OpenMP::OpenMP_CXX)
endif()

target_link_libraries(app fmt::fmt)

# opencv
target_link_libraries( app ${OpenCV_LIBS} )
target_link_libraries( app "/mnt/ec/ness/johnson/installs/boost/boost_1_79_0/stage/lib/" )
