/*
* File:        priority.cpp
* Description: Implements a priority ordering structure specially made for determining the
*              order of neighbours visited in PA2's filler::fill function.
*              Note that this does NOT serve the same purpose as the stack/queue
*              ordering structure which you have also been asked to implement.
*
*              Ignoring resize costs, any single insertion or removal operation
*              should cost at most O(n), where n is the number of PixelPoints
*              stored in the collection.
*
* Date:        2022-02-10 02:04
*
*/

#include "priority.h"
#include <assert.h>

#define DELTA 0.0000001
#include <cmath>

using namespace cs221util;
using namespace std;

/*
*  Default constructor
*  Initializes refcolor to the default color according to the HSLAPixel implementation.
*/
PriorityNeighbours::PriorityNeighbours() {
  // complete your implementation below
  refcolor = HSLAPixel();
}

/*
*  Parameterized constructor
*  Initializes refcolor to the supplied value.
*/
PriorityNeighbours::PriorityNeighbours(HSLAPixel ref) {
  // complete your implementation below
  refcolor = ref;
}

/*
*  Inserts an item into the collection
*  PARAM: p - item to be inserted
*  POST:  the collection contains p, along with all previously existing items.
*/
void PriorityNeighbours::Insert(PixelPoint p) {
  // complete your implementation below
  if (points.size() == 0) {
    points.push_back(p);
    return;
  }

  if (points.size() == 2) {
    if (priority(p, points[0])) {
      points.insert(points.begin(), p);
    } else if (priority(points[1], p)) {
      points.push_back(p);
    } else {
      points.insert(points.begin()+1, p);
    }
    return;
  } 

  if (priority(p, points[0])) {
    points.insert(points.begin(), p);
    return;
  } else if (priority(points[points.size()-1], p)) {
    points.push_back(p);
    return;
  }

  points.insert(points.begin() + position(p, 0, points.size()-1), p);
}

/*
*  Removes and returns an item from the collection satisfying the priority condition
*  PRE:    the collection is not empty (but it would be good to check anyway!).
*  POST:   the item satisfying the priority condition is removed from the collection.
*  RETURN: the item satisfying the priority condition
*
*  Priority condition:
*    The PixelPoint in the collection whose color is the minimum distance away
*    from the reference color is the priority item to be returned.
*
*    In case of multiple items with the same priority value (i.e. minimal distance
*    away from the reference color), the one with the minimal y-coordinate will be
*    selected.
*    In case of multiple items with the same priority value and the same y-coordinate,
*    the one with the minimal x-coordinate will be selected.
*
*    ***ATTENTION*** The dist() function in HSLAPixel.h will be very helpful!
*
*  Combined with Insert(), think about the time complexity of maintaining the
*  priority order and/or accessing the priority element in this specific application!
*/
PixelPoint PriorityNeighbours::Remove() {
  // complete your implementation below
  PixelPoint p(points[0].x, points[0].y, points[0].color);

  points.erase(points.begin());
  return p;
}

/*
*  Checks if the collection is empty
*  RETURN: true, if the collection is empty
*          false, otherwise
*/
bool PriorityNeighbours::IsEmpty() const {
  // complete your implementation below
  
  return points.empty();
}

/*
*  Returns the value of the reference color
*/
HSLAPixel PriorityNeighbours::GetReferenceColor() const {
  // complete your implementation below
  
  return refcolor;
}

/*
*  Sets the reference color attribute
*  POST: refcolor is set to the supplied value
*/
void PriorityNeighbours::SetReferenceColor(HSLAPixel ref) {
  // complete your implementation below
  refcolor = ref;
}

int PriorityNeighbours::position(PixelPoint p, int start, int end) const {
  if (start <= end) {
    int mid = (start + (end - start)) / 2;
    if (start == end) {
      mid = start;
    }

    if (priority(points[mid-1], p) && priority(p, points[mid])) {
      return mid;
    }

    if (priority(p, points[mid])) {
      return position(p, start, mid-1);
    }

    return position(p, mid+1, end);
  }

  return -1;
}

bool PriorityNeighbours::priority(PixelPoint p1, PixelPoint p2) const {
  double p1Dist = p1.color.dist(refcolor);
  double p2Dist = p2.color.dist(refcolor);

  if (p2Dist - p1Dist > DELTA) {
    return true;
  }

  if (fabs(p1Dist - p2Dist) < DELTA) {
    if (p1.y < p2.y) {
      return true;
    } else if (p1.y == p2.y) {
      if (p1.x <= p2.x) {
        return true;
      }
    }
  }

  return false;
}