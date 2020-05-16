#ifndef TRACINGH
#define TRACINGH

#include <limits>
#include "vec3.h"
#include "hittable.h"
#include "ray.h"
#include "camera.h"
#include "material.h"

namespace tracing {

    // colors
    static vec3 white(1.0, 1.0, 1.0);
    static vec3 blue(0.5, 0.7, 1.0);
    static vec3 red(153./255., 0., 0.);

    struct TracedPixel {
        public:
            TracedPixel(int row, int col) : i(row), j(col) {}
            int i, j;
            vec3* pixel;
    };

    struct RayTracingConfig {
        unsigned int height, width, max_depth, num_samples;
        std::unique_ptr<camera> cam;
        std::unique_ptr<hittable> world;
        std::string savepath;
        float estimate;
    };

    vec3 color(const ray& r, const RayTracingConfig& config, unsigned int depth) {
        hit_record rec;

        // if it's a valid (positive) time (in front of camera), then display a gradient
        // based on the normal vector from the center of the circle to the intersection point
        if (config.world->hit(r, 0.001, std::numeric_limits<float>::max(), rec)) {
        ray scattered;
        vec3 attenuation;

        if (depth < config.max_depth && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            // scattered
            return attenuation * color(scattered, config, depth + 1);
        
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
    vec3 trace(int i, int j, const RayTracingConfig& config) {
        vec3 c(0, 0, 0);

        // decide our color with `config.num_samples` random rays
        for (unsigned int s = 0; s < config.num_samples; ++s) {  // pre-increment doesn't need variable on stack!
            float xPercent = float(i + random_double()) / float(config.width);
            float yPercent = float(j + random_double()) / float(config.height);
            ray r = config.cam->get_ray(xPercent, yPercent);
            c += color(r, config, 0); // depth = 0
        }
        c /= float(config.num_samples);
        vec3 gamma_corrected(sqrt(c[0]), sqrt(c[1]), sqrt(c[2]));

        // make our color
        int ir = int(gamma_corrected[0] * 255.99);
        int ig = int(gamma_corrected[1] * 255.99);
        int ib = int(gamma_corrected[2] * 255.99);
        return vec3(ir, ig, ib);
    }

    /**
     * Batch of pixels to trace within a single thread
     * 
     * We use a lambda to set our Image object directly from threads. Each thread has a 
     * non-overlapping set of pixels to compute, so no thread coorination is necessary.
     **/
    void tracePixelBatch(int start, int end, const std::vector<TracedPixel>& jobs, const RayTracingConfig& config, Image& img) {
        std::for_each(
            jobs.begin() + start,
            jobs.begin() + end,
            [&](TracedPixel pixel) {
                vec3 c = trace(pixel.i, pixel.j, config);
                img.setPixel(c, pixel.i, pixel.j);
            }
        );
    }

}

#endif