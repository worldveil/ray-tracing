#include <iostream>
#include <stdlib.h>

/*
    Usage:
    $ make ppm
    $ ppm/run > ppm/image.ppm
*/

int main() {
    int nx = 800;
    int ny = 500;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    for (int j = ny-1; j >= 0; j--) {
    // for (int j = 0; j < ny; j++) {
        for (int i = 0; i < nx; i++) {
            float r = float(i) / float(nx);
            float g = float(j) / float(ny);
            float b = 0.2;
            int ir = int(255.99*r);
            int ig = int(255.99*g);
            int ib = int(255.99*b);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}