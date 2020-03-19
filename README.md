ray tracing
===

* [Ray tracing in one weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
* [Clang getting started](https://clang.llvm.org/get_started.html)
* [Makefile reference](http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/)
* [Journey from C code to executable](https://stackoverflow.com/a/1213207/)
* [Nice online C++ shell](http://cpp.sh/)

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

## 6) Surface Normals and Multiple Objects

```shell
make surface_normals; ./surface_normals/run > ./surface_normals/image.ppm
```




