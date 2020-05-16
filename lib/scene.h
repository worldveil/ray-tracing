#ifndef SCENEH
#define SCENEH

#include <algorithm>
#include <memory>

#include "rand.h"
#include "sphere.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "vec3.h"

namespace scene {

    std::unique_ptr<hittable> random_scene(bool floating) {
        int n = 500;
        std::vector<std::unique_ptr<hittable>> list;
        list.reserve(n); // preallocate memory, but do not default construct (ie: nullptr)

        // add world sphere
        list.push_back(std::make_unique<sphere>(
            vec3(0, -1000, 0),
            1000, 
            std::make_unique<lambertian>(vec3(0.5, 0.5, 0.5))
        ));
        
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
                        list.push_back(std::make_unique<sphere>(center, 0.2,
                            std::make_unique<lambertian>(vec3(
                                random_double(),
                                0.,
                                0.)
                            )
                        ));
                    }
                    else if (choose_mat < 0.95) { // metal
                        list.push_back(std::make_unique<sphere>(center, 0.2,
                                std::make_unique<metal>(vec3(0.5*(1 + random_double()),
                                            0.5*(random_double()),
                                            0.5*(random_double())),
                                        0.5*random_double()))
                        );
                    }
                    else {  // glass
                        list.push_back(
                            std::make_unique<sphere>(center, 0.2, std::make_unique<dielectric>(1.5))
                        );
                    }
                }
            }
        }

        // add large spheres
        list.push_back(std::make_unique<sphere>(vec3(0, 1, 0), 1.0, std::make_unique<dielectric>(1.5)));
        list.push_back(std::make_unique<sphere>(vec3(-4, 1, 0), 1.0, std::make_unique<lambertian>(vec3(0.2, 0.2, 0.2))));
        list.push_back(std::make_unique<sphere>(vec3(4, 1, 0), 1.0, std::make_unique<metal>(vec3(0.7, 0.6, 0.5), 0.)));
        
        return std::make_unique<hittable_list>(std::move(list));
    }
}

#endif