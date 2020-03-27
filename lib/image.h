#include <string>

#include "vec3.h"

class Image {
    public:
        Image(int h, int w) : height(h), width(w) {
            pixels = new vec3[height * width];
        }
        ~Image() {
            delete pixels;
        }

        vec3& getPixel(int i, int j);
        void setPixel(vec3& p, int i, int j);
        void writeToFile(std::string filepath);

        vec3 *pixels;
        int height; 
        int width;
};

inline vec3& Image::getPixel(int i, int j) {
    return pixels[height * i + j];
}

inline void Image::setPixel(vec3& p, int i, int j) {
    pixels[height * i + j] = p;
}

inline void Image::writeToFile(std::string filepath) {
    // open up file to write
    std::ofstream f;
    f.open(filepath);

    // header for PPM file
    f << "P3\n" << width << " " << height << "\n255\n";

    // write pixels
    for (int j = height; j >= 0; j--) {
        for (int i = 0; i < width; i++) {
            vec3 pixel = getPixel(i, j);
            f << pixel.r() << " " << pixel.g() << " " << pixel.b() << "\n";
        }
    }

    // close file
    f.close();
}