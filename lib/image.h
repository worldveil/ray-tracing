#include <string>

#include "vec3.h"

class Image {
    public:
        Image(int h, int w) : pixels(h * w), height(h), width(w) {}

        const vec3& getPixel(int i, int j) const;
        void setPixel(const vec3& p, int i, int j);
        bool writeToFile(std::string filepath) const;

        std::vector<vec3> pixels;
        int height; 
        int width;
};

inline const vec3& Image::getPixel(int i, int j) const {
    return pixels[height * i + j];
}

inline void Image::setPixel(const vec3& p, int i, int j) {
    pixels[height * i + j] = p;
}

inline bool Image::writeToFile(std::string filepath) const {
    // open up file to write
    std::ofstream f(filepath);
    if (!f.is_open()) {
        return false;
    }

    // header for PPM file
    f << "P3\n" << width << " " << height << "\n255\n";

    // write pixels
    for (int j = height - 1; j >= 0; j--) {
        for (unsigned int i = 0; i < width; i++) {
            const vec3& pixel = getPixel(i, j);
            f << pixel.r() << " " << pixel.g() << " " << pixel.b() << "\n";
        }
    }

    // close file
    f.close();
    return true;
}