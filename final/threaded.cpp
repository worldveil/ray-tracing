#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <thread>
#include <algorithm>
#include <random>

#include "ray.h"
#include "vec3.h"
#include "hittable_list.h"
#include "sphere.h"
#include "rand.h"
#include "camera.h"
#include "image.h"

// timing imports
using std::milli;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

// colors
static vec3 white(1.0, 1.0, 1.0);
static vec3 blue(0.5, 0.7, 1.0);
static vec3 red(153./255., 0., 0.);

// circle variables
static vec3 circleCenter(0., 0., -1);
static float circleRadius = 0.5;

static const int MAX_DEPTH = 25;
static const int HEIGHT = 600;
static const int WIDTH = 800;
static const int NUM_SAMPLES = 25;

static const unsigned NUM_THREADS = std::max(std::thread::hardware_concurrency() - 1, (unsigned)1);


struct TracedPixel {
    public:
        TracedPixel(int row, int col) : i(row), j(col) {}
        int i, j;
        vec3* pixel;
};

struct RayTracingConfig {
    int height, width, max_depth, num_samples;
    camera* cam;
    hittable* world;
};

vec3 color(const ray& r, hittable *world, int depth) {
    hit_record rec;

    // if it's a valid (positive) time (in front of camera), then display a gradient
    // based on the normal vector from the center of the circle to the intersection point
    if (world->hit(r, 0.001, MAXFLOAT, rec)) {
      ray scattered;
      vec3 attenuation;

      if (depth < MAX_DEPTH && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
        // scattered
        return attenuation * color(scattered, world, depth + 1);
      
      } else {
        // absorbed
        return vec3(0, 0, 0);
      }

    } else {
      // we didn't hit the sphere, so render the background
      vec3 unitDirection = unitVector(r.direction());
      float t = unitDirection.y() / 2. + 0.5;
    //   return (1. - t) * white + t * blue;
      return (1. - t) * white + t * red;
    //   return (1. - t) * red + t * white;
    }
}

/**
 * Traces a single pixel
 **/
vec3* trace(int i, int j, RayTracingConfig& config) {
    vec3 c(0, 0, 0);

    // decide our color with `config.num_samples` random rays
    for (int s = 0; s < config.num_samples; ++s) {  // pre-increment doesn't need variable on stack!
        float xPercent = float(i + random_double()) / float(config.width);
        float yPercent = float(j + random_double()) / float(config.height);
        ray r = config.cam->get_ray(xPercent, yPercent);
        c += color(r, config.world, 0); // depth = 0
    }
    c /= float(config.num_samples);
    vec3 gamma_corrected(sqrt(c[0]), sqrt(c[1]), sqrt(c[2]));

    // make our color
    int ir = int(gamma_corrected[0] * 255.99);
    int ig = int(gamma_corrected[1] * 255.99);
    int ib = int(gamma_corrected[2] * 255.99);
    return new vec3(ir, ig, ib);
}

/**
 * Batch of pixels to trace within a single thread
 * 
 * We use a lambda to set our Image object directly from threads. Each thread has a 
 * non-overlapping set of pixels to compute, so no thread coorination is necessary.
 **/
void tracePixelBatch(int start, int end, std::vector<TracedPixel>& jobs, RayTracingConfig& config, Image& img) {
    std::for_each(
        jobs.begin() + start,
        jobs.begin() + end,
        [&](TracedPixel pixel) {
            vec3* c = trace(pixel.i, pixel.j, config);
            img.setPixel(*c, pixel.i, pixel.j);
        }
    );
}

hittable *random_scene() {
    int n = 500;
    hittable **list = new hittable*[n+1];
    list[0] =  new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = random_double();
            vec3 center(a + 0.9 * random_double(),
                        0.2,
                        b + 0.9 * random_double());

            // prevent from intersecting with main spheres!
            if ((center - vec3(0, 1, 0)).length() <= 1.25) {
                continue;
            }
            if ((center - vec3(-4, 1, 0)).length() <= 1.25) {
                continue;
            }
            if ((center - vec3(4, 1, 0)).length() <= 1.25) {
                continue;
            }
            
            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {  // diffuse
                    list[i++] = new sphere(center, 0.2,
                        new lambertian(vec3(random_double(),
                                            0.,
                                            0.)
                        )
                    );
                }
                else if (choose_mat < 0.95) { // metal
                    list[i++] = new sphere(center, 0.2,
                            new metal(vec3(0.5*(1 + random_double()),
                                           0.5*(random_double()),
                                           0.5*(random_double())),
                                      0.5*random_double()));
                }
                else {  // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.2, 0.2, 0.2)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.));

    return new hittable_list(list, i);
}

float printStats(const char *const tag, high_resolution_clock::time_point start, high_resolution_clock::time_point end, bool output) {
    float ms = duration_cast<duration<double, milli> >(end - start).count();
    if (output) printf("%s: %f ms\n", tag, ms);
    return ms;
}

int main() {
    std::cout << "Rendering with " << NUM_THREADS << " threads..." << std::endl;

    // create our configuration struct
    RayTracingConfig config;
    config.height = HEIGHT;
    config.width = WIDTH;
    config.max_depth = MAX_DEPTH;
    config.num_samples = NUM_SAMPLES;

    /*
      hittable objects in the scene.

      Note!
      we have to have this array of abstract (hittable) type as array of pointers because if we declare an
      array of abstract type, c++ tries to allocate memory for that object based on default values. however,
      c++ doesn't know what those defaults are because it could be one of many classes!
      It's possible C++ could allocate the max memory we'd need for each possible class...
      but that's not what c++ is about. Thus, we have to have an array of pointers. The memory allocation
      is delegated to some other operation, at some other time.
    */
    config.world = random_scene();

    // set up camera
    vec3 up = vec3(0, 1, 0);
    vec3 lookFrom(7.8, 1.5, 1.95);
    vec3 lookAt(0, 1, 0);
    float aspect = float(config.width) / float(config.height);
    float distToFocusAt = (lookFrom - lookAt).length();
    float aperture = 0.;
    float fieldOfViewDegrees = 45;
    camera cam(lookFrom, lookAt, up, fieldOfViewDegrees, aspect, aperture, distToFocusAt);
    config.cam = &cam;

    // for status updates, have some stats about the image
    int totalPixels = config.width * config.height;
    int every = int(totalPixels / 10.);
    int completedPixels = 0;
    std::cout.precision(3);

    // allocate image
    Image img(config.height, config.width);

    // create pixel tracing jobs
    std::vector<TracedPixel> jobs;
    for (int j = config.height; j >= 0; j--) {
        for (int i = 0; i < config.width; i++) {
            TracedPixel p(i, j);
            jobs.push_back(p);
        }
    }

    // shuffle for better parallelism since some regions of image are more costly than others
    // auto rng = std::default_random_engine{};
    // std::shuffle(std::begin(jobs), std::end(jobs), rng);

    // start rendering time
    const high_resolution_clock::time_point startRenderTime = high_resolution_clock::now();

    // create list of thread pointers
    std::vector<std::thread *> threads;

    // spin off threads
    int itemsPerThread = int(totalPixels / NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; ++i) {
        int start = i * itemsPerThread;
        int end = (i == NUM_THREADS - 1) ? totalPixels - 1 : start + itemsPerThread;
        std::thread* th = new std::thread(tracePixelBatch, start, end, std::ref(jobs), std::ref(config), std::ref(img));
        threads.push_back(th);
    }

    // join threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i]->join();
    }

    // report time back to user
    const high_resolution_clock::time_point endRenderTime = high_resolution_clock::now();
    float renderingMs = printStats("\nRendering took", startRenderTime, endRenderTime, true);
    float perPixel = renderingMs / totalPixels;
    std::cout << "Per pixel render ms (" << totalPixels << "): " << perPixel << " ms" << std::endl;

    // then write to disk
    std::ofstream f;
    f.open ("test-threading.ppm");

    // header for PPM file
    f << "P3\n" << config.width << " " << config.height << "\n255\n";

    // write pixels to disk in correct PPM format order
    for (int j = config.height; j >= 0; j--) {
        for (int i = 0; i < config.width; i++) {
            vec3 pixel = img.getPixel(i, j);
            f << pixel.r() << " " << pixel.g() << " " << pixel.b() << "\n";
        }
    }

    // close file
    f.close();
}