#include "ray.h"
#include "vec3.h"
#include "hittable_list.h"
#include "sphere.h"
#include "rand.h"
#include "camera.h"

// colors
static vec3 white(1.0, 1.0, 1.0);
static vec3 blue(0.5, 0.7, 1.0);
static vec3 red(153./255., 0., 0.);

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
    //   return (1. - t) * white + t * blue;
      return (1. - t) * white + t * red;
    //   return (1. - t) * red + t * white;
    }
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

int main() {
    int width = 300;
    int height = 200;
    int numAntialiasingSamples = 35;

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
    hittable *world = random_scene();

    // set up camera
    vec3 up = vec3(0, 1, 0);
    vec3 lookFrom(5, 1.3, 1.4);
    vec3 lookAt(0, 1, 0);
    float aspect = float(width) / float(height);
    float distToFocusAt = (lookFrom - lookAt).length();
    float aperture = 0.;
    float fieldOfViewDegrees = 45;

    camera cam(lookFrom, lookAt, up, fieldOfViewDegrees, aspect, aperture, distToFocusAt);

    int totalPixels = width * height;
    int every = int(totalPixels / 10.);
    int completedPixels = 0;

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

            // progress reports
            completedPixels++;
            float percentCompleted = completedPixels / float(totalPixels) * 100;
            if (completedPixels % every == 0) {
                std::cerr << "Completed " << percentCompleted << " percent of pixels..." << std::endl;
            }
        }
    }
}
