#ifndef RAYH
#define RAYH

#include "vec3.h"

class ray {
    public:
        ray() {}
        // a = start, b = another point
        ray(const vec3& a, const vec3& b) { A = a; B = b; }
        vec3 origin() const { return A; }
        vec3 direction() const { return B; }
        vec3 pointAtParameter(float t) const { return A + t * B; }

        vec3 A;
        vec3 B;
};

inline std::ostream& operator<<(std::ostream &os, const ray &r) {
    os << "A: " << r.A << ", B: " << r.B;
    return os;
}

#endif