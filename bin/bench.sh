for nb_proc in 1 2 3 4 5 
do
    for nb_threads in 1 2 4 8 
    do 
	echo -e "#size\ttime" > bench_mpd_${nb_proc}x${nb_threads}
	for i in 1 2 3 4 5 6
	do
	    mpiexec -np 1 ./bench ${nb_proc} ${nb_threads} ${i}
	done
    done
done
