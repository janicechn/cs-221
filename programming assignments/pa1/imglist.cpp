// File:        imglist.cpp
// Date:        2022-01-27 10:21
// Description: Contains partial implementation of ImgList class
//              for CPSC 221 2021W2 PA1
//              Function bodies to be completed by yourselves
//
// ADD YOUR FUNCTION IMPLEMENTATIONS IN THIS FILE
//

#include "imglist.h"

#include <math.h> // provides fmax, fmin, and fabs functions

/**************************
* MISCELLANEOUS FUNCTIONS *
**************************/

/*
* This function is NOT part of the ImgList class,
* but will be useful for one of the ImgList functions.
* Returns the "difference" between two hue values.
* PRE: hue1 is a double between [0,360).
* PRE: hue2 is a double between [0,360).
* 
* The hue difference is the absolute difference between two hues,
* but takes into account differences spanning the 360 value.
* e.g. Two pixels with hues 90 and 110 differ by 20, but
*      two pixels with hues 5 and 355 differ by 10.
*/
double HueDiff(double hue1, double hue2) {
  return fmin(fabs(hue1 - hue2), fabs(360 + fmin(hue1, hue2) - fmax(hue1, hue2)));
}

// Calculate average with respect to angular distance
double ChannelAverage(double nodeColour1, double nodeColour2) {
  double result = 0;
  double diff = HueDiff(nodeColour1, nodeColour2) / 2;
  if (diff == fabs(nodeColour1 - nodeColour2) / 2) {
    result = fmin(nodeColour1, nodeColour2) + diff;
  } else {
    if (fmin(nodeColour1, nodeColour2) - diff < 0) {
      result = fmax(nodeColour1, nodeColour2) + diff;
    } else {
      result = fmin(nodeColour1, nodeColour2) - diff;
    }
  }
  return result;
}

/*********************
* CONSTRUCTORS, ETC. *
*********************/

/*
* Default constructor. Makes an empty list
*/
ImgList::ImgList() {
  // set appropriate values for all member attributes here
  northwest = nullptr;
  southeast = nullptr;
}

/*
* Creates a list from image data
* PRE: img has dimensions of at least 1x1
*/
ImgList::ImgList(PNG& img) {
  // build the linked node structure and set the member attributes appropriately
  ImgNode* prevX = nullptr;
  ImgNode* prevY = nullptr;
  ImgNode* nextPreviousY = nullptr;
  // loop through each pixel
  for (unsigned y = 0; y < img.height(); y++) {
    for (unsigned x = 0; x < img.width(); x++) {
      ImgNode* curr = new ImgNode(); // create node
      curr->colour = *img.getPixel(x, y); // set current node colour to be current pixel
      if (y == 0 && x == 0) {
        northwest = curr; // set northwest node
      }
      if (y == img.height()-1 && x == img.width()-1) {
        southeast = curr; // set southeast node
      }
      if (prevX != nullptr) {
        prevX->east = curr; // current node is east of the previous node
        curr->west = prevX; // prev node is west of current node
      }
      if (prevY != nullptr) {
        prevY->south = curr; // current node is south of the previous Y (above) node
        curr->north = prevY; // previous Y (above) node is north of current node
        prevY = prevY->east; // update previous (north) node to be next (east) node for next iteration
      }
      if (x == 0) {
        nextPreviousY = curr; // will be the starting prevY for next Y iteration
      }
      
      prevX = curr; // set current node to be the previous node (west) for next x iteration
      // curr = nullptr;
    }
    prevX = nullptr; // for the next row, east value of node at x=0 will be null
    prevY = nextPreviousY; // next prevY for next y iteration
    // nextPreviousY = nullptr;
  }
  // prevY = nullptr;
}

/*
* Copy constructor.
* Creates this this to become a separate copy of the data in otherlist
*/
ImgList::ImgList(const ImgList& otherlist) {
  // build the linked node structure using otherlist as a template
  Copy(otherlist);
}

/*
* Assignment operator. Enables statements such as list1 = list2;
*   where list1 and list2 are both variables of ImgList type.
* POST: the contents of this list will be a physically separate copy of rhs
*/
ImgList& ImgList::operator=(const ImgList& rhs) {
  // Re-build any existing structure using rhs as a template
  
  if (this != &rhs) { // if this list and rhs are different lists in memory
    // release all existing heap memory of this list
    Clear();    
    
    // and then rebuild this list using rhs as a template
    Copy(rhs);
  }
  
  return *this;
}

/*
* Destructor.
* Releases any heap memory associated with this list.
*/
ImgList::~ImgList() {
  // Ensure that any existing heap memory is deallocated
  Clear();
}

/************
* ACCESSORS *
************/

/*
* Returns the horizontal dimension of this list (counted in nodes)
* Note that every row will contain the same number of nodes, whether or not
*   the list has been carved.
* We expect your solution to take linear time in the number of nodes in the
*   x dimension.
*/
unsigned int ImgList::GetDimensionX() const {
  // replace the following line with your implementation
  ImgNode* currX = northwest; // count across the first row starting from northwest node
  unsigned int countX = 0;
  // loop across node links
  while (currX != nullptr) {
    countX++; // add count for current node
    currX = currX->east; // continue for next node in row
  }
  return countX;
}

/*
* Returns the vertical dimension of the list (counted in nodes)
* It is useful to know/assume that the grid will never have nodes removed
*   from the first or last columns. The returned value will thus correspond
*   to the height of the PNG image from which this list was constructed.
* We expect your solution to take linear time in the number of nodes in the
*   y dimension.
*/
unsigned int ImgList::GetDimensionY() const {
  // replace the following line with your implementation
  ImgNode* currY = northwest; // count down the first column starting from northwest node
  unsigned int countY = 0;
  // loop down node links
  while (currY != nullptr) {
    countY++; // add count for current node (no skipped nodes on first column)
    currY = currY->south; // continue for next node
  }
  return countY;
}

/*
* Returns the horizontal dimension of the list (counted in original pixels, pre-carving)
* The returned value will thus correspond to the width of the PNG image from
*   which this list was constructed.
* We expect your solution to take linear time in the number of nodes in the
*   x dimension.
*/
unsigned int ImgList::GetDimensionFullX() const {
  // replace the following line with your implementation
  ImgNode* currentNode = northwest; // count across the first row starting from northwest node
  unsigned int count = 0;
  // loop across node links
  while (currentNode != nullptr) {
    count = count + 1 + currentNode->skipright; // add count for current node and any skipped nodes beside
    currentNode = currentNode->east; // continue for next node
  }
  return count;
}

/*
* Returns a pointer to the node which best satisfies the specified selection criteria.
* The first and last nodes in the row cannot be returned.
* PRE: rowstart points to a row with at least 3 physical nodes
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: rowstart - pointer to the first node in a row
* PARAM: selectionmode - criterion used for choosing the node to return
*          0: minimum luminance across row, not including extreme left or right nodes
*          1: node with minimum total of "hue difference" with its left neighbour and with its right neighbour.
*        In the (likely) case of multiple candidates that best match the criterion,
*        the left-most node satisfying the criterion (excluding the row's starting node)
*        will be returned.
* A note about "hue difference": For PA1, consider the hue value to be a double in the range [0, 360).
* That is, a hue value of exactly 360 should be converted to 0.
* The hue difference is the absolute difference between two hues,
* but be careful about differences spanning the 360 value.
* e.g. Two pixels with hues 90 and 110 differ by 20, but
*      two pixels with hues 5 and 355 differ by 10.
*/
ImgNode* ImgList::SelectNode(ImgNode* rowstart, int selectionmode) {
  // add your implementation below
  ImgNode* currNode = rowstart->east; // first column node cannot be returned, start at next
  ImgNode* selected = currNode; // current node that best satisfies specified selection criteria
  // nodes that don't have null nodes beside and are not the first/last node
  while(currNode->east != NULL) { // loop across row except last column node
    if (selectionmode == 0) {
      if (currNode->colour.l < selected->colour.l) {
        selected = currNode; // update selected node to be the one that has strictly smaller luminance
      }
    } else { // if selectionmode == 1
      // total hue difference with left and right neighbour
      double currHueDiffTotal = HueDiff(currNode->colour.h, currNode->east->colour.h) + 
                                HueDiff(currNode->colour.h, currNode->west->colour.h);
      double selectedHueDiffTotal = HueDiff(selected->colour.h, selected->east->colour.h) + 
                                    HueDiff(selected->colour.h, selected->west->colour.h);
      if (currHueDiffTotal < selectedHueDiffTotal) {
        selected = currNode; // set selected node to be the one that has min total hue diff
      }
    }
    currNode = currNode->east; // loop across the row
  }
  return selected;
  selected = nullptr;
}

/*
* Renders this list's pixel data to a PNG, with or without filling gaps caused by carving.
* PRE: fillmode is an integer in the range of [0,2]
* PARAM: fillgaps - whether or not to fill gaps caused by carving
*          false: render one pixel per node, ignores fillmode
*          true: render the full width of the original image,
*                filling in missing nodes using fillmode
* PARAM: fillmode - specifies how to fill gaps
*          0: solid, uses the same colour as the node at the left of the gap
*          1: solid, using the averaged values (all channels) of the nodes at the left and right of the gap
*             Note that "average" for hue will use the closer of the angular distances,
*             e.g. average of 10 and 350 will be 0, instead of 180.
*             Average of diametric hue values will use the smaller of the two averages
*             e.g. average of 30 and 210 will be 120, and not 300
*                  average of 170 and 350 will be 80, and not 260
*          2: *** OPTIONAL - FOR BONUS ***
*             linear gradient between the colour (all channels) of the nodes at the left and right of the gap
*             e.g. a gap of width 1 will be coloured with 1/2 of the difference between the left and right nodes
*             a gap of width 2 will be coloured with 1/3 and 2/3 of the difference
*             a gap of width 3 will be coloured with 1/4, 2/4, 3/4 of the difference, etc.
*             Like fillmode 1, use the smaller difference interval for hue,
*             and the smaller-valued average for diametric hues
*/
PNG ImgList::Render(bool fillgaps, int fillmode) const {
  // Add/complete your implementation below
  
  PNG outpng; //this will be returned later. Might be a good idea to resize it at some point.
  if (fillgaps == false) {
    outpng = PNG(GetDimensionX(), GetDimensionY()); // create image based on dimension of nodes
    ImgNode* head = northwest; // start at northwest
    ImgNode* temp = northwest->south; // head for next row
    int xRender = 0; // starting pixel x-coordinate
    int yRender = 0; // starting pixel y-coordinate
    while (head != nullptr) { // while there are nodes to render...
      // assign pixel with head colour data
      *outpng.getPixel(xRender, yRender) = head->colour;
      //update the node to render and the xRender/yRender for pixel
      if (head->east != nullptr) {
        head = head->east;
        xRender++;
      } else if (temp != nullptr) {
        head = temp;
        temp = temp->south;
        xRender = 0;
        yRender++;
      } else {
        head = nullptr;
      }
    }
  } else { // if fillgaps == true
    outpng = PNG(GetDimensionFullX(), GetDimensionY()); // create image based on original img dimensions
    ImgNode* head = northwest; // start at northwest
    ImgNode* temp = northwest->south; // node to start rendering for next row
    int xRender = 0; // starting pixel x-coordinate
    int yRender = 0; // starting pixel y-coordinate
    while (head != nullptr) { // while there are nodes to render...
      if (head->skipright != 0) { // if there are gaps...
        *outpng.getPixel(xRender, yRender) = head->colour;
        for(unsigned int i = 1; i <= head->skipright; i++) { // loop through current node and the gaps to render
          xRender++; // update pixel to render across the gaps with corresponding xRender coordinate
          if (fillmode == 0) {
            // assign the pixel colour data with the left node of the gap
            *outpng.getPixel(xRender, yRender) = head->colour;
          } else if (fillmode == 1) {
            // assign the pixel colour data calculated (averaged values) for the gap
            *outpng.getPixel(xRender, yRender) = HSLAPixel(ChannelAverage(head->colour.h, head->east->colour.h), 
                                                          ChannelAverage(head->colour.s, head->east->colour.s), 
                                                          ChannelAverage(head->colour.l, head->east->colour.l),
                                                          ChannelAverage(head->colour.a, head->east->colour.a));
          } else { // if fillmode == 2
            // assign the pixel colour data calculated (gradient values) for the gap
            // calculated gradient values depends on i and head->skipright for proportion of the difference
            *outpng.getPixel(xRender, yRender) = HSLAPixel(head->colour.h + (HueDiff(head->colour.h, head->east->colour.h)*i) / (head->skipright + 1),
                                                          head->colour.s + (HueDiff(head->colour.s, head->east->colour.s)*i) / (head->skipright + 1), 
                                                          head->colour.l + (HueDiff(head->colour.l, head->east->colour.l)*i) / (head->skipright + 1),
                                                          head->colour.a + (HueDiff(head->colour.a, head->east->colour.a)*i) / (head->skipright + 1));
          }
        }
      } else { // if there is no gap, assign pixel with head colour data as usual
        *outpng.getPixel(xRender, yRender) = head->colour;
      }
      //update the node to render and the xRender/yRender for pixel across the row (for all fillmodes)
      if (head->east != nullptr) {
        head = head->east;
        xRender++;
      } else if (temp != nullptr) {
        head = temp;
        temp = temp->south;
        xRender = 0;
        yRender++;
      } else {
        head = nullptr;
      }
    }
  }
  return outpng;
}

/************
* MODIFIERS *
************/

/*
* Removes exactly one node from each row in this list, according to specified criteria.
* The first and last nodes in any row cannot be carved.
* PRE: this list has at least 3 nodes in each row
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: selectionmode - see the documentation for the SelectNode function.
* POST: this list has had one node removed from each row. Neighbours of the created
*       gaps are linked appropriately, and their skip values are updated to reflect
*       the size of the gap.
*/
void ImgList::Carve(int selectionmode) {
  // add your implementation here
  ImgNode* currRowStart = northwest; // start at top row
  ImgNode* removeNode = nullptr; // node to remove
  while (currRowStart != nullptr) {
    removeNode = SelectNode(currRowStart, selectionmode); // select node to remove in this row
    if (removeNode->north != nullptr) {
      removeNode->north->skipdown += removeNode->skipdown + 1; // add skipdown count to the node above removed node
      removeNode->north->south = removeNode->south; // change north node's south node to be the node south of the removed node
    }
    if (removeNode->south != nullptr) {
      removeNode->south->skipup += removeNode->skipup + 1; // add skipup count to the node below removed node
      removeNode->south->north = removeNode->north; // change south node's north node to be the node north of the removed node
    }
    removeNode->east->skipleft += removeNode->skipleft + 1; // add skipleft count to the node right removed node
    removeNode->west->skipright += removeNode->skipright + 1; // add skipright count to the node left removed node
    removeNode->east->west = removeNode->west; // change right node's west node to be the node west of the removed node
    removeNode->west->east = removeNode->east; // change left node's east node to be the node east of the removed node
    delete removeNode; // remove selected node
    removeNode = nullptr;
    currRowStart = currRowStart->south; // loop through each row going down for each rowStart node
  }
}

// note that a node on the boundary will never be selected for removal
/*
* Removes "rounds" number of nodes (up to a maximum of node width - 2) from each row,
* based on specific selection criteria.
* Note that this should remove one node from every row, repeated "rounds" times,
* and NOT remove "rounds" nodes from one row before processing the next row.
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: rounds - number of nodes to remove from each row
*        If rounds exceeds node width - 2, then remove only node width - 2 nodes from each row.
*        i.e. Ensure that the final list has at least two nodes in each row.
* POST: this list has had "rounds" nodes removed from each row. Neighbours of the created
*       gaps are linked appropriately, and their skip values are updated to reflect
*       the size of the gap.
*/
void ImgList::Carve(unsigned int rounds, int selectionmode) {
  // add your implementation here
  int removeRounds = rounds; // number of nodes to remove from each row
  if (rounds > GetDimensionX() - 2) { // if rounds exceeds node width - 2...
    removeRounds = GetDimensionX() - 2; // remove only node width - 2 nodes from each row 
  }
  for (int r = 1; r <= removeRounds; r++) {
    Carve(selectionmode); // remove a node from each row; loop to remove another -- removeRounds times
  }
}


/*
* Helper function deallocates all heap memory associated with this list,
* puts this list into an "empty" state. Don't forget to set your member attributes!
* POST: this list has no currently allocated nor leaking heap memory,
*       member attributes have values consistent with an empty list.
*/
void ImgList::Clear() {
  // add your implementation here
  if (northwest != nullptr) {
    ImgNode* tempX; // node to delete
    ImgNode* tempY = northwest->south; // start of next row
    ImgNode* head = northwest; // start at northwest
    northwest = nullptr;
    while (head != nullptr) { // loop till there are no more nodes
      tempX = head; // set node to delete as the head
      if (head->east == nullptr) { // if reached end of row...
        if (tempY != nullptr) { // if theres another row below...
          head = tempY; // next node to delete is tempY (first node to delete on new row)
          tempY = tempY->south; // next new row is the node below the last tempY
        } else { // if there is no row below...
          head = nullptr; // set head to nullptr (end the while loop)
          southeast = nullptr;
        }
      } else { // if there is a next node beside head...
        head = head->east; // set the next node to delete to be the one beside the one that will be deleted
      }
      delete(tempX); // delete the node
      tempX = nullptr;
    }
  }
}

/* ************************
*  * OPTIONAL - FOR BONUS *
** ************************
* Helper function copies the contents of otherlist and sets this list's attributes appropriately
* PRE: this list is empty
* PARAM: otherlist - list whose contents will be copied
* POST: this list has contents copied from by physically separate from otherlist
*/
void ImgList::Copy(const ImgList& otherlist) {
  // add your implementation here
  
}

/*************************************************************************************************
* IF YOU DEFINED YOUR OWN PRIVATE FUNCTIONS IN imglist.h, YOU MAY ADD YOUR IMPLEMENTATIONS BELOW *
*************************************************************************************************/

