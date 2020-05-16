#ifndef SPHEREH
#define SPHEREH

#include "hittable.h"
#include "material.h"

class sphere: public hittable  {
    public:
        sphere(vec3 cen, float r, std::unique_ptr<material> m) 
            : center(cen), radius(r), squaredRadius(r * r), mat_ptr(std::move(m)) {};
        
        virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;

        vec3 center;
        float radius;
        float squaredRadius;
        std::unique_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    const vec3 oc = r.origin() - center;
    const float a = dot(r.direction(), r.direction());
    const float b = dot(oc, r.direction());
    const float c = dot(oc, oc) - squaredRadius;
    const float discriminant = b*b - a*c;

    if (discriminant > 0) {
        const float sqrt_discriminant_div_a = sqrt(discriminant) / a;
        const float minus_b_div_a = - b / a;
        float temp = minus_b_div_a - sqrt_discriminant_div_a;

        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.pointAtParameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr.get();
            return true;
        }

        temp = minus_b_div_a + sqrt_discriminant_div_a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.pointAtParameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr.get();
            return true;
        }
    }
    return false;
}


#endif