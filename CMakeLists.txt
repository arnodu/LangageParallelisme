cmake_minimum_required(VERSION 2.6)
project(TDP3)

# Write binaries to bin/
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -fopenmp -std=c99")

if(DEFINED USE_MKL)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mkl:${USE_MKL} -DUSE_MKL")
endif()

file(GLOB_RECURSE master_files
       "src/master/*.c"
       "src/master/*.h")

file(GLOB_RECURSE worker_files
       "src/worker/*.c"
       "src/worker/*.h")

file(GLOB_RECURSE common_files
       "src/common/*.c"
       "src/common/*.h")

find_package(MPI)
message("Using MPI libs: ${MPI_LIBRARIES}")

set(PROJECT_INCLUDE_DIR "src")
include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${MPI_INCLUDE_PATH}")

add_executable("master"	${master_files}
			${common_files})
target_link_libraries("master" ${MPI_LIBRARIES})

add_executable("worker"	${worker_files}
			${common_files})
target_link_libraries("worker" ${MPI_LIBRARIES})








