#ifndef SCENEH
#define SCENEH

#include <algorithm>

#include "rand.h"
#include "sphere.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "vec3.h"

namespace scene {

    hittable *random_scene(bool floating) {
        int n = 500;
        hittable **list = new hittable*[n+1];
        list[0] =  new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
        int i = 1;
        for (int a = -11; a < 11; a++) {
            for (int b = -11; b < 11; b++) {
                float choose_mat = random_double();

                // do we want our spheres to float?
                vec3 center(0, 0, 0);
                if (floating) {
                    center.e[0] = a + 0.9 * random_double();
                    center.e[1] = std::max(0.2, (-center.e[0] - 1.) / 4. + random_double());  // plane equation: x + 2y = 0
                    center.e[2] = b + 0.9 * random_double();
                
                } else {
                    center.e[0] = a + 0.9 * random_double();
                    center.e[1] = 0.2;
                    center.e[2] = b + 0.9 * random_double();
                }

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
}

#endif