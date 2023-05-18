#include "HSLAPixel.h"

namespace cs221util {
    
    HSLAPixel::HSLAPixel() {
        h = s = 0;
        l = a = 1.0;
    }

    HSLAPixel::HSLAPixel(double hue, double sat, double lum) {
        h = hue;
        s = sat;
        l = lum;
        a = 1.0;
    }
    
    HSLAPixel::HSLAPixel(double hue, double sat, double lum, double alpha) {
        h = hue;
        s = sat;
        l = lum;
        a = alpha;
    }
}