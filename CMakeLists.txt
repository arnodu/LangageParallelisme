cmake_minimum_required(VERSION 2.6)
project(TDP3)

# Write binaries to bin/
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -fopenmp -std=c99")

if(DEFINED USE_MKL)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mkl:${USE_MKL} -DUSE_MKL")
endif()

file(GLOB_RECURSE util_files
       "src/util/*.c"
       "src/util/*.h")

find_package(MPI)
message("Using MPI libs: ${MPI_LIBRARIES}")

set(PROJECT_INCLUDE_DIR "src")
include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${MPI_INCLUDE_PATH}")

add_executable("master"	${util_files}
			src/main/master.c)
target_link_libraries("master" ${MPI_LIBRARIES} m)

add_executable("worker"	${util_files}
			src/main/worker.c)
target_link_libraries("worker" ${MPI_LIBRARIES} m)

add_executable("test_util"	${util_files}
			src/main/test_util.c)
target_link_libraries("test_util" ${MPI_LIBRARIES} m)







