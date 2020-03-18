ray tracing
===

* [Ray tracing in one weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
* [Clang getting started](https://clang.llvm.org/get_started.html)
* [Makefile reference](http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/)
* [Journey from C code to executable](https://stackoverflow.com/a/1213207/)

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

