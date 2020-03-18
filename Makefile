CC=clang++
CFLAGS=-I ./lib

ppm: ppm/make_ppm.cpp
	$(CC) ppm/make_ppm.cpp -o ppm/run

ppm_vec3: ppm/make_ppm_vec3.cpp lib/vec3.h
	$(CC) ppm/make_ppm_vec3.cpp -o ppm/run_vec3 $(CFLAGS)

ppm_assembly:
	$(CC) -S -masm=intel ppm/make_ppm.cpp -o ppm/ppm.asm

ray_background: ray_background/bg.cpp lib/ray.h lib/vec3.h
	$(CC) ray_background/bg.cpp -o ray_background/run $(CFLAGS)