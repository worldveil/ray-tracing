#ifndef MATERIALH
#define MATERIALH

#include "hittable.h"
#include "rand.h"
#include "ray.h"


struct hit_record;


float schlick(float cosine, float ref_idx) {
    float r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosine),5);
}


bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
    vec3 uv = unitVector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
    if (discriminant > 0) {
        refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    } else return false;
}


vec3 reflect(const vec3& v, const vec3& n) {
     return v - 2*dot(v,n)*n;
}


vec3 randomInUnitSphere() {
    vec3 p;
    do {
        p = 2.0*vec3(random_double(),random_double(),random_double()) - vec3(1,1,1);
    } while (p.squaredLength() >= 1.0);
    return p;
}


class material  {
    public:
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
        virtual ~material() = 0;
};

// weird gotcha: we HAVE to declare this for a virtual base class! otherwise we get
//     ld: symbol(s) not found for architecture x86_64
material::~material() {}


class lambertian : public material {
    public:
        lambertian(const vec3& a) : albedo(a) {}
        ~lambertian() {}
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const  {
             vec3 target = rec.p + rec.normal + randomInUnitSphere();
             scattered = ray(rec.p, target - rec.p);
             attenuation = albedo;
             return true;
        }

        vec3 albedo;
};


class metal : public material {
    public:
        metal(const vec3& a, float f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }
        ~metal() {}
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const  {
            vec3 reflected = reflect(unitVector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*randomInUnitSphere());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }
        vec3 albedo;
        float fuzz;
};


class dielectric : public material {
    public:
        dielectric(float ri) : ref_idx(ri) {}
        ~dielectric() {}
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const  {
             vec3 outward_normal;
             vec3 reflected = reflect(r_in.direction(), rec.normal);
             float ni_over_nt;
             attenuation = vec3(1.0, 1.0, 1.0);
             vec3 refracted;
             float reflect_prob;
             float cosine;
             if (dot(r_in.direction(), rec.normal) > 0) {
                  outward_normal = -rec.normal;
                  ni_over_nt = ref_idx;
               // cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
                  cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
                  cosine = sqrt(1 - ref_idx * ref_idx * (1 - cosine * cosine));
             }
             else {
                  outward_normal = rec.normal;
                  ni_over_nt = 1.0 / ref_idx;
                  cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
             }
             if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
                reflect_prob = schlick(cosine, ref_idx);
             else
                reflect_prob = 1.0;
             if (random_double() < reflect_prob)
                scattered = ray(rec.p, reflected);
             else
                scattered = ray(rec.p, refracted);
             return true;
        }

        float ref_idx;
};


#endif