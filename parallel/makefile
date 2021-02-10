UNAME := $(shell uname)

ALL: mandelbrot
CC =  mpicc
profile_flags =-L/home/luhanc/mpiP/lib -L/home/luhanc/libunwind/build/lib -lmpiP -lbfd -liberty -lunwind
compile_flags = -lm -O3 -I. -std=c99
debug_flags = -O0 -g3
runtime_flags = -n 2
EXEC = mpirun
ifeq ($(UNAME), Darwin)
	debugger = lldb -o run
else 
	debugger = gdb -ex=r
endif
profile: mandelbrot.c
	$(CC) $< -o mandelbrot $(compile_flags) $(profile_flags)
debug: mandelbrot 
	$(EXEC) $(runtime_flags) xterm -e $(debugger) ./$< 
mandelbrot: mandelbrot.c
	$(CC) $< -o $@ $(compile_flags) 
run: mandelbrot
	$(EXEC) $(runtime_flags) $< 
clean:
	rm -rf mandelbrot Mandelbrot.ppm
	rm -rf *.dSYM
	rm -rf slurm*.out
	rm -rf *.mpiP
