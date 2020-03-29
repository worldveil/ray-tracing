#ifndef HITTABLEH
#define HITTABLEH

#include "ray.h"

class material;

struct hit_record {
	float t;
	vec3 p;
	vec3 normal;
  material *mat_ptr;
};

class hittable  {
    public:
        virtual bool hit(
            const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
        virtual ~hittable() = 0;
};

// weird gotcha: we HAVE to declare this for a virtual base class! otherwise we get
//     ld: symbol(s) not found for architecture x86_64
hittable::~hittable() { }

#endif