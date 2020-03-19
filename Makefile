CC=clang++
CFLAGS=-I ./lib

scratch: scratch/scratch.cpp
	$(CC) scratch/scratch.cpp -o scratch/run && ./scratch/run

ppm: ppm/make_ppm.cpp
	$(CC) ppm/make_ppm.cpp -o ppm/run

ppm_vec3: ppm/make_ppm_vec3.cpp lib/vec3.h
	$(CC) ppm/make_ppm_vec3.cpp -o ppm/run_vec3 $(CFLAGS)

ppm_assembly:
	$(CC) -S -masm=intel ppm/make_ppm.cpp -o ppm/ppm.asm

ray_background: ray_background/bg.cpp lib/ray.h lib/vec3.h
	$(CC) ray_background/bg.cpp -o ray_background/run $(CFLAGS)

hit_sphere: hit_sphere/hit_sphere.cpp lib/ray.h lib/vec3.h
	$(CC) hit_sphere/hit_sphere.cpp -o hit_sphere/run $(CFLAGS)

surface_normals: surface_normals/render.cpp lib/ray.h lib/vec3.h
	$(CC) surface_normals/render.cpp -o surface_normals/run $(CFLAGS)

hittable_spheres: hittable_spheres/render.cpp lib/ray.h lib/vec3.h
	$(CC) hittable_spheres/render.cpp -o hittable_spheres/run $(CFLAGS)
