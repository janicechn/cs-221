#ifndef CS221UTIL_HSLAPIXEL_H
#define CS221UTIL_HSLAPIXEL_H

namespace cs221util {
    class HSLAPixel {
        public: // or change class to struct
        double h, s, l, a;
        HSLAPixel();
        HSLAPixel(double hue, double sat, double lum);
        HSLAPixel(double hue, double sat, double lum, double alpha);
    };
}

#endif