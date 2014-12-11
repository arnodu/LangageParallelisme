module add compiler/mkl/stable;
module add mpi/openmpi/1.8.1;
module load compiler/intel/stable;

mkdir build;
cd build;
cmake;
make;
cd ..;
