#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <thread>
#include <algorithm>
#include <random>

#include "args.hpp"
#include "vec3.h"
#include "camera.h"
#include "image.h"
#include "scene.h"
#include "tracing.h"

// timing imports
using std::milli;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

// circle variables
static vec3 circleCenter(0., 0., -1);
static float circleRadius = 0.5;

// defaults & threading
static const int DEFAULT_MAX_DEPTH = 25;
static const int DEFAULT_NUM_SAMPLES = 25;
static const unsigned NUM_THREADS = std::max(std::thread::hardware_concurrency() - 1, (unsigned)1);


float printStats(const char *const tag, high_resolution_clock::time_point start, high_resolution_clock::time_point end, bool output) {
    float ms = duration_cast<duration<double, milli> >(end - start).count();
    if (output) printf("%s: %f ms\n", tag, ms);
    return ms;
}

int main(int argc, char** argv) {
    tracing::RayTracingConfig config;

    // parse command line arguments
    args::ArgumentParser parser("DREVO Ray Tracer", "This goes after the options.");
    args::HelpFlag help(parser, "help", "Display this help menu", {"help"});
    args::ValueFlag<int> width(parser, "width", "Height of our image", {'w'});
    args::ValueFlag<int> height(parser, "height", "Width of our image", {'h'});
    args::ValueFlag<int> depth(parser, "depth", "How many bounces to allow for each ray of light", {'d'});
    args::ValueFlag<int> sampling(parser, "sampling", "Number of rays to sample per pixel, for antialiasing purposes", {'s'});
    args::ValueFlag<std::string> output(parser, "output", "Output PPM filepath", {'o'});

    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help) {
        std::cout << parser;
        return 0;
    } catch (args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    } catch (args::ValidationError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    // validate the input from the command line
    if (!width || !height) {
        throw args::ValidationError("Requires a height and a width to render image.");
        return 1;
    }

    if (!output) {
        throw args::ValidationError("Requires a savepath for the PPM image file.");
        return 1;
    }

    config.height = args::get(height);
    config.width = args::get(width);
    config.savepath = args::get(output);
    config.max_depth = depth ? args::get(depth) : DEFAULT_MAX_DEPTH; 
    config.num_samples = sampling ? args::get(sampling) : DEFAULT_NUM_SAMPLES; 

    std::cout << "Rendering '" << config.savepath \
        << "' [" << NUM_THREADS \
        << " threads]: height=" << config.height \
        << ", width=" << config.width \
        << ", maxdepth=" << config.max_depth \
        << ", sampling=" << config.num_samples \
        << std::endl;

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
    config.world = scene::random_scene();

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
    std::vector<tracing::TracedPixel> jobs;
    for (int j = config.height; j >= 0; j--) {
        for (int i = 0; i < config.width; i++) {
            tracing::TracedPixel p(i, j);
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
        std::thread* th = new std::thread(tracing::tracePixelBatch, start, end, std::ref(jobs), std::ref(config), std::ref(img));
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