/*
*  File: negativeColorPicker.cpp
*  Implements the negative color picker for CPSC 221 PA2.
*
*/

#include "negativeColorPicker.h"

NegativeColorPicker::NegativeColorPicker(PNG& inputimg)
{
  // complete your implementation below
  img = inputimg;
}

HSLAPixel NegativeColorPicker::operator()(PixelPoint p)
{
  // complete your implementation below
  HSLAPixel before = *img.getPixel(p.x, p.y);

  double afterH = before.h < 180 ? before.h + 180 : before.h - 180;

  return HSLAPixel(afterH, before.s, 1.0-before.l, before.a);
}
