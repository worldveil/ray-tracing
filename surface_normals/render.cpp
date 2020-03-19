#include "ray.h"
#include "vec3.h"

// colors
static vec3 white(1.0, 1.0, 1.0);
static vec3 blue(0.5, 0.7, 1.0);
static vec3 red(1., 0., 0.);

// circle variables
static vec3 circleCenter(0., 0., -1);
static float circleRadius = 0.5;


float hitSphere(const ray& r, const vec3& center, float radius) {
    float a = r.B.squaredLength();
    vec3 aMinusC = r.origin() - center;
    float b = 2. * dot(r.B, aMinusC);
    float c = dot(aMinusC, aMinusC) - radius * radius;
    float discriminant = b*b - 4.*a*c;

    if (discriminant < 0) {
        // ray does not intersect!
        return -1.;
    } else {
        // Let's assume the closest hit point (smallest t)
        return (-b - sqrt(discriminant)) / (2 * a);
    }

    return (discriminant > 0);
}

vec3 color(const ray& r) {
    // find out the time t at which (if at all) our ray intersects the circle
    float t = hitSphere(r, circleCenter, circleRadius);

    // if it's a valid (positive) time (in front of camera), then display a gradient
    // based on the normal vector from the center of the circle to the intersection point
    if (t > 0.0) {
        vec3 normal = unitVector(r.pointAtParameter(t) - circleCenter);
        // now, turn unit vector with components in range (-1, 1) into (0, 1) RGB colors
        return 0.5 * vec3(normal.x()+1, normal.y()+1, normal.z()+1);
    }

    // get unit direction vector
    vec3 unitDirection = unitVector(r.direction());
    t = unitDirection.y() / 2. + 0.5;
    return (1. - t) * white + t * blue;
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
