#include "ray.h"
#include "vec3.h"
#include "hittable_list.h"
#include "sphere.h"
#include "rand.h"
#include "camera.h"

// colors
static vec3 white(1.0, 1.0, 1.0);
static vec3 blue(0.5, 0.7, 1.0);
static vec3 red(1., 0., 0.);

// circle variables 
static vec3 circleCenter(0., 0., -1);
static float circleRadius = 0.5;

// depth
static int MAX_DEPTH = 50;

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
      return (1. - t) * white + t * blue;
    }
}

int main() {
    int width = 800;
    int height = 600;
    int numAntialiasingSamples = 50;

    // header for PPM file
    std::cout << "P3\n" << width << " " << height << "\n255\n";

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
    hittable *list[4];
    list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
    list[1] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(1, 1, 1), 0.0));
    list[3] = new sphere(vec3(-1,0,-1), 0.5, new metal(vec3(0.8, 0.8, 0.8), 3));
    hittable *world = new hittable_list(list,4);

    // set up camera
    float aspect = float(width) / float(height);
    vec3 up = vec3(0, 1, 0);
    vec3 lookFrom(-1, 1, 1);
    vec3 lookAt(0, 0, -1);
    camera cam(lookFrom, lookAt, up, 90, aspect);

    for (int j = height; j >= 0; j--) {
        for (int i = 0; i < width; i++) {

            // decide our pixel
            float xPercent = float(i) / float(width);
            float yPercent = float(j) / float(height);

            // decide our color with `numAntialiasingSamples` random rays
            vec3 c(0, 0, 0);
            for (int s = 0; s < numAntialiasingSamples; s++) {
                float xPercent = float(i + random_double()) / float(width);
                float yPercent = float(j + random_double()) / float(height);
                ray r = cam.get_ray(xPercent, yPercent);
                c += color(r, world, 0); // depth = 0
            }
            c /= float(numAntialiasingSamples);
            vec3 gamma_corrected(sqrt(c[0]), sqrt(c[1]), sqrt(c[2]));

            // make our color
            int ir = int(gamma_corrected[0] * 255.99);
            int ig = int(gamma_corrected[1] * 255.99);
            int ib = int(gamma_corrected[2] * 255.99);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
