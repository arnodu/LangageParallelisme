module add compiler/intel/stable
module add mpi/openmpi/current

mkdir -p build 
cd build
cmake ..
make
