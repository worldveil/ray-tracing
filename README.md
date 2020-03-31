Ray Tracer
===

<img src="/img/red_balls.png" alt="" title="" width="800">

<img src="/img/matte_sphere.png" alt="" title="" width="800">

## About

This is a ray tracer for spheres only in C++. It features a variable position, lens focusable camera. Also three different materials with parameterized color, refraction, and reflection. 

It's based on the "Ray Tracer In One Weekend" blog post I found online, but I have adapted it and coded it myself so I could understand what's going on. I choose to keep most of the library code in headers as inline functions, both to keep it to fewer files and for performance reasons.

I've also parallelized it with a multithreaded execution model, added time estimates for rendering hi-res scenes, parameterized with command line flag parsing, and optimized memory management with Valgrind.

It's certainly a toy ray tracer, but a fun one.

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
make production && ./tracer -h 800 -w 1200 -s 100 -d 40 -o scene.ppm -e 0.01
```

On my machine, this takes about 3 minutes. Crazy you say? Well...

```
(800*1200 pixels) * 40 bounces possible * 100 antialiasing samples * ~50 spheres ~= 200 billion ray collision checks / calcuations!
```

For this reason, optimizing ray tracers is pretty important. Probably the next step for code like this (beyond actually allowing for triangles!) would be some time of quadtree-like intersection method, AKA a [Bounding Volume Hierarchy](https://en.wikipedia.org/wiki/Bounding_volume_hierarchy), which would cut down the number of intersections by some log factor. Still, this is a monumental task once we get to larger image sizes and number of objects.

Note that the `make production` compilation uses `-O3`, and this makes the program run 8-10x faster (!).

## Valgrind

Install it on Mojave with: 

```shell
brew install --HEAD https://raw.githubusercontent.com/LouisBrunner/valgrind-macos/master/valgrind.rb
```

then run with:

```shell
$ make development
$ valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --progress-interval=5 \
    -- \
    ./tracer -h 10 -w 20 -o test.ppm -s 10 -d 10 -o scene.ppm -e 0.01
```

It's worth noting that Valgrind actually serializes (orders) thread execution so they just execute one after the other. Valgrind in general is up to 50x slower as well, since it's monitoring every memory allocation. So this multithreaded ray tracer will be really, really slow in Valgrind. That's why I have the image so small in the example shell command.

If the C++ gods smile favorably on you, you'll see something like:

```
==55007== HEAP SUMMARY:
==55007==     in use at exit: 0 bytes in 0 blocks
==55007==   total heap usage: 0 allocs, 0 frees, 0 bytes allocated
==55007== 
==55007== All heap blocks were freed -- no leaks are possible
```

## Resources

* [Ray tracing in one weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
* [Clang docs](https://clang.llvm.org/get_started.html)
* Makefiles
    * [Makefile reference](http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/)
    * [Ultimate source for Makefiles](https://www.gnu.org/software/make/manual/make.html)
* [Journey from C code to executable](https://stackoverflow.com/a/1213207/)
* [Nice online C++ shell](http://cpp.sh/)
* [Ray tracing resources - GOOD](http://www.realtimerendering.com/raytracing.html#books)
* [Really cool course from EPFL](https://wjakob.github.io/nori/)

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

## TODO

If I were to continue this:

* Add intersections for triangles, and some way to incorporate them into polygons & 3D volumes
* Better time esimation that isn't hardcoded for my machine / 8 cores
* BVH for increased performance
* Explicit light sources
* Converting RGB to spectrum for even more efficiency