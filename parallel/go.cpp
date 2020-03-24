#include <iostream>

#include "image.h"

using std::milli;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

void printStats(high_resolution_clock::time_point start, high_resolution_clock::time_point end) {
    printf("%f", duration_cast<duration<double, milli> >(end - start).count());
}

int main() {
    const high_resolution_clock::time_point startTime = high_resolution_clock::now();

    Image im(300, 400);
    vec3 p(0.5, 0.5, 0.5);
    im.setPixel(p, 1, 1);

    const high_resolution_clock::time_point endTime = high_resolution_clock::now();

    std::cout << "Pixel: " << im.getPixel(1, 1) << std::endl;

    printStats(startTime, endTime);
}
