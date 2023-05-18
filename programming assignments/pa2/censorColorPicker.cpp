/*
*  File: censorColorPicker.cpp
*  Implements the censor color picker for CPSC 221 PA2.
*
*/

#include "censorColorPicker.h"

#include <math.h> // gives access to sqrt function

/*
*  Useful function for computing the Euclidean distance between two PixelPoints
*/
double PointDistance(PixelPoint a, PixelPoint b) {
  unsigned int diff_x, diff_y;
  if (a.x > b.x)
    diff_x = a.x - b.x;
  else
    diff_x = b.x - a.x;
  if (a.y > b.y)
    diff_y = a.y - b.y;
  else
    diff_y = b.y - a.y;
  return sqrt(diff_x * diff_x + diff_y * diff_y);
}

CensorColorPicker::CensorColorPicker(unsigned int b_width, PixelPoint ctr, unsigned int rad, PNG& inputimage)
{
  // complete your implementation below
  blockwidth = b_width;
  center = ctr;
  radius = rad;
  img = inputimage;
  blockyimg.resize(img.width(), img.height());
  HSLAPixel avg;
  // loop for each block
  for (unsigned int y = 0; y < img.height(); y += blockwidth) {
    for (unsigned int x = 0; x < img.width(); x += blockwidth) {
      // total sum of each attribute for each pixel in the block from original img
      double h = 0;
      double s = 0;
      double l = 0;
      double a = 0;
      int count = 0;
      // looping through each pixel in block
      for (unsigned int y_img = y; y_img < y+blockwidth; y_img++) {
        for (unsigned int x_img = x; x_img < x+blockwidth; x_img++) {
          // sum the attributes of pixels that exist within the block (account edge blocks)
          if (x_img < img.width() && y_img < img.height()) {
            h += img.getPixel(x_img, y_img)->h;
            s += img.getPixel(x_img, y_img)->s;
            l += img.getPixel(x_img, y_img)->l;
            a += img.getPixel(x_img, y_img)->a;
            count++;
          }
        }
      }
      avg = HSLAPixel(h/count, s/count, l/count, a/count);

      // looping through each pixel in block for blockyimg and setting as average attributes
      for (unsigned int y_blocky = 0; y_blocky < blockwidth && y_blocky < img.height(); y_blocky++) {
        for (unsigned int x_blocky = 0; x_blocky < blockwidth && x_blocky < img.width(); x_blocky++) {
          // apply HSLA to blockyimg within its dimensions (account edge blocks)
          if (x_blocky < blockyimg.width() && y_blocky < blockyimg.height()) {
            *blockyimg.getPixel(x_blocky+x, y_blocky+y) = avg;
          }
        }
      }
    }
  }
}

HSLAPixel CensorColorPicker::operator()(PixelPoint p)
{
  // complete your implementation below

  if (PointDistance(center, p) <= radius) {
    return *blockyimg.getPixel(p.x, p.y);
  } else {
    return *img.getPixel(p.x, p.y);
  }

}
