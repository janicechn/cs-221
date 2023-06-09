#include <iostream>
#include <cmath>
#include <cstdlib>

#include "cs221util/PNG.h"
#include "cs221util/HSLAPixel.h"
#include "lab_intro.h"

using namespace cs221util;

/**
 * Returns an image that has been transformed to grayscale.
 *
 * The saturation of every pixel is set to 0, removing any color.
 *
 * @return The grayscale image.
 */
PNG grayscale(PNG image) {
  /// This function is already written for you so you can see how to
  /// interact with our PNG class.
  for (unsigned x = 0; x < image.width(); x++) {
    for (unsigned y = 0; y < image.height(); y++) {
      HSLAPixel *pixel = image.getPixel(x, y);

      // `pixel` is a pointer to the memory stored inside of the PNG `image`,
      // which means you're changing the image directly.  No need to `set`
      // the pixel since you're directly changing the memory of the image.
      pixel->s = 0;
    }
  }

  return image;
}



/**
 * Returns an image with a spotlight centered at (`centerX`, `centerY`).
 *
 * A spotlight adjusts the luminance of a pixel based on the distance the pixel
 * is away from the center by decreasing the luminance by 0.5% per 1 pixel euclidean
 * distance away from the center.
 *
 * For example, a pixel 3 pixels above and 4 pixels to the right of the center
 * is a total of `sqrt((3 * 3) + (4 * 4)) = sqrt(25) = 5` pixels away and
 * its luminance is decreased by 2.5% (0.975x its original value).  At a
 * distance over 200 pixels away, the luminance will always 0.
 *
 * The modified PNG is then returned.
 *
 * @param image A PNG object which holds the image data to be modified.
 * @param centerX The center x coordinate of the crosshair which is to be drawn.
 * @param centerY The center y coordinate of the crosshair which is to be drawn.
 *
 * @return The image with a spotlight.
 */
PNG createSpotlight(PNG image, int centerX, int centerY) {

  for (unsigned x = 0; x < image.width(); x++) {
    for (unsigned y = 0; y < image.height(); y++) {
      HSLAPixel *pixel = image.getPixel(x, y);

      double dist = sqrt((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY));

      pixel->l = dist > 200 ? 0 : pixel->l * (1 - 0.005 * dist);

    }
  }

  return image;
}

// precondition: a and b are both in the range [0, 360].
// postcondition: returns the angle between a and b, 
// which must be in the range [0, 180].
double angle_between(double a, double b) {
  double angle1 = abs(a - b);
  double angle2 = 360 - angle1;
  return min(angle1, angle2);
}

/**
 * Returns a image transformed to UBC colors.
 *
 * The hue of every pixel is set to the a hue value of either yellow or
 * blue, based on if the pixel's hue value is closer to yellow than blue.
 *
 * @param image A PNG object which holds the image data to be modified.
 *
 * @return The UBCify'd image.
**/
PNG ubcify(PNG image) {
  const double YELLOW = 40, BLUE = 210;
  for (unsigned x = 0; x < image.width(); x++) {
    for (unsigned y = 0; y < image.height(); y++) {
      HSLAPixel *pixel = image.getPixel(x, y);
      if (angle_between(pixel->h, YELLOW) < angle_between(pixel->h, BLUE)) {
        pixel->h = YELLOW;
      } else {
        pixel->h = BLUE;
      }
    }
  }

  return image;
}


/**
* Returns an immge that has been watermarked by another image.
*
* The luminance of every pixel of the second image is checked, if that
* pixel's luminance is 1 (100%), then the pixel at the same location on
* the first image has its luminance increased by 0.2.
*
* @param firstImage  The first of the two PNGs.
* @param secondImage The second of the two PNGs.
*
* @return The watermarked image.
*/
PNG watermark(PNG firstImage, PNG secondImage) {

  // the mins here guarantee that we never access out of bounds, without the need to resize
  for (unsigned x = 0; x < min(firstImage.width(), secondImage.width()); x++) {
    for (unsigned y = 0; y < min(firstImage.height(), secondImage.height()); y++) {
      HSLAPixel *pixel = secondImage.getPixel(x, y);
      if (pixel->l == 1) {
        HSLAPixel *markedPixel = firstImage.getPixel(x,y);
        if (markedPixel->l + 0.2 > 1.0)
          markedPixel->l = 1.0;
        else
          markedPixel->l += 0.2;
      }
    }
  }

  return firstImage;
}

