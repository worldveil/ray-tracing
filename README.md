ray tracing
===

<img src="/img/red_balls.png" alt="" title="" width="800">

<img src="/img/matte_sphere.png" alt="" title="" width="800">

## Running

For the scene rendered with:

* height=800
* width=1200
* 100 antialiasing samples
* maximum bounce depth of 40 ray bounces
* save to filename `scene.ppm`
* estimate runtime before rendering with 0.01 (1%) of pixels

Simply run:

```shell
make && ./tracer -h 800 -w 1200 -o test.ppm -s 100 -d 40 -o scene.ppm -e 0.01
```

On my machine, this takes about 20 minutes. Crazy you say? Well...

```
(800*1200 pixels) * 40 bounces possible * 100 antialiasing samples * ~50 spheres ~= 200 billion ray collision checks / calcuations!
```

For this reason, optimizing ray tracers is pretty important. Probably the next step for code like this (beyond actually allowing for triangles!) would be some time of quadtree-like intersection method, AKA a [Bounding Volume Hierarchy](https://en.wikipedia.org/wiki/Bounding_volume_hierarchy), which would cut down the number of intersections by some log factor. Still, this is a monumental task once we get to larger image sizes and number of objects.  

## Valgrind

Install it on Mojave with: 

```shell
brew install --HEAD https://raw.githubusercontent.com/LouisBrunner/valgrind-macos/master/valgrind.rb
```

then run with:

```shell
$ valgrind ./tracer -h 800 -w 1200 -o test.ppm -s 100 -d 40 -o scene.ppm -e 0.01
```

## Resources

* [Ray tracing in one weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
* [Clang getting started](https://clang.llvm.org/get_started.html)
* Makefiles
    * [Makefile reference](http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/)
    * [Ultimate source for Makefiles](https://www.gnu.org/software/make/manual/make.html)
* [Journey from C code to executable](https://stackoverflow.com/a/1213207/)
* [Nice online C++ shell](http://cpp.sh/)
* [Ray tracing resources - GOOD](http://www.realtimerendering.com/raytracing.html#books)
* [Really cool course from EPFL](https://wjakob.github.io/nori/)

### C++ Concepts you'll need to understand the code:

* [Initialization lists](https://www.cprogramming.com/tutorial/initialization-lists-c++.html): way to initialize instance variables without coding a constructor as explicitly. Mostly syntactic sugar. 
* [You can't allocate an array of abstract class objects](https://stackoverflow.com/questions/60764923/c-allocating-memory-for-list-of-abstract-class-objects): you need an array of pointers. This is the only way polymorphism can work since otherwise C++ has no idea how much space to allocate for each entry in the array (each subclass might have different memory requirements)!
* Makefiles and header-only inline functions
* Pointers vs References...of course
* Virtual functions and OO in C++

## Remember:

* `(x, y)`
* For PPM: `(width, height)`
* Add `-g gdb` in compiler arguments so that GDB symbols can be loaded from a binary for debugging purposes

## Basic Algorithm

Pseudocode for ray tracing:

```
# Create objects which encapsulate an object using 3D vectors + parameterized volume to answer questions. The questions
# are both spatial && material oriented.
# Questions:
# [Spatial]  1) does a given ray hit this object?
# [Material] 2) if hit, how much do we attenuate the ray hitting it (how reflective is this surface)? This is a float in range [0, 1]. 
#               0 would mean completely black (black hole), and 1 would mean perfectly reflective (ie: mirror).
# [Material] 3) if hit by ray of light, what direction does the (attenuated) new scattered ray go in?
objects = []
objects.add(
    new Sphere(center  = new Point(x, y, z),
               material= new Metal(attenuation=Point(R, G, B))
    )
)
# ... and so on

# add to world object which will encapsulate getting closest object hit
world = new World(objects)

# field of view stuff
heightPixels = 600
widthPixels = 800
antialiasingRandomSampleFactor = 100
camera = new Camera(
    origin = new Point(x, y, z),
    lowerLeft = new Point(x, y, z),
    # ... etc
)
image = new Image(heightPixels, widthPixels)

# render image, one pixel at a time
for x in widthPixels
    for y in heightPixels
        colorAccumulator = new Point(0, 0, 0)

        # take `antialiasingRandomSampleFactor` random samplings and average them together
        # to calculate each pixel's visible color
        for s in antialiasingRandomSampleFactor
            xPercent = (x + randomFloat()) / widthPixels
            yPercent = (y + randomFloat()) / heightPixels
            r = new Ray(camera, xPercent, yPercent)

            # this is where most of the magic happens - for this ray, the world finds the 
            # closest object it hits (if any), and if another ray is scattered, recurses to find the
            # eventual source of light that generates this ray. if no source is found, then the
            # color is zero'd out (black). each time bounces/scatters happen, the light is generally
            # attenuated by some scalar 0 to 1 multiple. Any multiple of 0 to 1 will somewhat darken the
            # color, eventually going to black, though we generally limit the depth of bounces.
            color = world.bounce(r)

            colorAccumulator += color
        
        # transform color if needed
        colorAccumulator = gammaCorrect(colorAccumulator)

        # set pixel
        image[x, y] = colorAccumulator / antialiasingRandomSampleFactor

# save image!
image.save('output.png')
```