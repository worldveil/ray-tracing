#include "ray.h"
#include "vec3.h"

// colors
static vec3 white(1.0, 1.0, 1.0);
static vec3 blue(0.5, 0.7, 1.0);
static vec3 red(1., 0., 0.);

// circle variables
static vec3 circleCenter(0., 0., -1);
static float circleRadius = 0.5;


bool hitSphere(const ray& r, const vec3& center, float radius) {
    float a = r.B.squaredLength();
    vec3 aMinusC = r.origin() - center;
    float b = 2. * dot(r.B, aMinusC);
    float c = dot(aMinusC, aMinusC) - radius * radius;
    float discriminant = b*b - 4.*a*c;
    return (discriminant > 0);
}

vec3 color(const ray& r) {
    if (hitSphere(r, circleCenter, circleRadius))
        return red;

    // std::cout << "Direction: " << r.direction() << std::endl;
    // get unit direction vector
    vec3 unitDirection = unitVector(r.direction());
    // std::cout << "yUnitDirection: " << yUnitDirection << std::endl;
    float yScaled = unitDirection.y() / 2. + 0.5;
    // std::cout << "yScaled: " << yScaled << std::endl;
    return (1. - yScaled) * white + yScaled * blue;
}

int main() {
    int width = 200;
    int height = 100;

    // header for PPM file
    std::cout << "P3\n" << width << " " << height << "\n255\n";

    // (x, y, z)
    vec3 lower_left_corner(-2., -1., -1.);
    vec3 horizontal(4., 0., 0.);
    vec3 vertical(0., 2., 0.);
    vec3 origin(0., 0., 0.);

    for (int j = height; j >= 0; j--) {
        for (int i = 0; i < width; i++) {

            // decide our pixel
            float xPercent = float(i) / float(width);
            float yPercent = float(j) / float(height);

            // cast our ray, and figure out what color it is depending on the angle
            vec3 destination = lower_left_corner + xPercent * horizontal + yPercent * vertical;
            // std::cout << "Destination: " << destination << std::endl;

            ray r(origin, destination);
            // std::cout << "Ray: " << r << std::endl;
            vec3 c = color(r);
            // std::cout << "Color: " << c << std::endl;

            // make our color
            int ir = int(c[0] * 255.99);
            int ig = int(c[1] * 255.99);
            int ib = int(c[2] * 255.99);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
