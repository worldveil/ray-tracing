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

        vec3 *pixels;
        int height;
        int width;
};

inline vec3& Image::getPixel(int i, int j) {
    return pixels[width * i + j];
}

inline void Image::setPixel(vec3& p, int i, int j) {
    pixels[width * i + j] = p;
}