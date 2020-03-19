ray tracing
===

* [Ray tracing in one weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
* [Clang getting started](https://clang.llvm.org/get_started.html)
* [Makefile reference](http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/)
* [Journey from C code to executable](https://stackoverflow.com/a/1213207/)
* [Nice online C++ shell](http://cpp.sh/)

C++ Concepts you'll need to understand the code:

* [Initialization lists](https://www.cprogramming.com/tutorial/initialization-lists-c++.html): way to initialize instance variables without coding a constructor as explicitly. Mostly syntactic sugar. 
* [You can't allocate an array of abstract class objects](https://stackoverflow.com/questions/60764923/c-allocating-memory-for-list-of-abstract-class-objects): you need an array of pointers. This is the only way polymorphism can work since otherwise C++ has no idea how much space to allocate for each entry in the array (each subclass might have different memory requirements)!
* Makefiles and header-only inline functions
* Pointers vs References...of course
* Virtual functions and OO in C++

Remember:

* `(x, y)`
* For PPM: `(width, height)`

## Steps

### 2) Output an Image

```shell
# first PPM example
make ppm
ppm/run > ppm/image.ppm
make ppm_assembly  # if you want to see the assembly generated
```

### 4) Rays + Simple Camera

Here we have our first camera at the origin, and we "paint" a screen by iterating stepwise with two vectors, one for the x-component and the other for the y-component. 

We linearly interpolate ("lerp") between blue & white.

```shell
make ray_background
./ray_background/run > ./ray_background/image.ppm
```

### 5) Hitting a Sphere

```shell
make hit_sphere
./hit_sphere/run > ./hit_sphere/image.ppm
```

### 6) Surface Normals and Multiple Objects

```shell
make surface_normals; ./surface_normals/run > ./surface_normals/image.ppm
```

### 7) Hittable Inheiritence 

```shell
make hittable_spheres; ./hittable_spheres/run > ./hittable_spheres/image.ppm
```

### 8) Antialiasing

```shell
```



