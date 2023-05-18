/*
*  File:        ptree.cpp
*  Description: Implementation of a partitioning tree class for CPSC 221 PA3
*  Date:        2022-03-03 01:53
*
*               ADD YOUR PRIVATE FUNCTION IMPLEMENTATIONS TO THE BOTTOM OF THIS FILE
*/

#include "ptree.h"
#include "ptree-private.h"
#include "hue_utils.h" // useful functions for calculating hue averages

using namespace cs221util;
using namespace std;

// The following definition may be convenient, but is not necessary to use
typedef pair<unsigned int, unsigned int> pairUI;

/////////////////////////////////
// PTree private member functions
/////////////////////////////////

/*
*  Destroys all dynamically allocated memory associated with the current PTree object.
*  You may want to add a recursive helper function for this!
*  POST: all nodes allocated into the heap have been released.
*/
void PTree::Clear() {
  ClearHelper(root);
}

void PTree::ClearHelper(Node* root) {
  if (root == NULL) {
    return;
  }

  ClearHelper(root->A);
  ClearHelper(root->B);
  delete(root);
}
/*
*  Copies the parameter other PTree into the current PTree.
*  Does not free any memory. Should be called by copy constructor and operator=.
*  You may want a recursive helper function for this!
*  PARAM: other - the PTree which will be copied
*  PRE:   There is no dynamic memory associated with this PTree.
*  POST:  This PTree is a physically separate copy of the other PTree.
*/
void PTree::Copy(const PTree& other) {
  root = CopyHelper(other.root);
}

Node* PTree::CopyHelper(const Node* root) {
  if (root == NULL) {
    return NULL;
  }

  Node* newNode = new Node(root->upperleft, root->width, root->height, root->avg);
  newNode->A = CopyHelper(root->A);
  newNode->B = CopyHelper(root->B);
  return newNode;
}

/*
*  Private helper function for the constructor. Recursively builds the tree
*  according to the specification of the constructor.
*  You *may* change this if you like, but we have provided here what we
*  believe will be sufficient to use as-is.
*  PARAM:  im - full reference image used for construction
*  PARAM:  ul - upper-left image coordinate of the currently building Node's image region
*  PARAM:  w - width of the currently building Node's image region
*  PARAM:  h - height of the currently building Node's image region
*  RETURN: pointer to the fully constructed Node
*/
Node* PTree::BuildNode(PNG& im, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h) {
  double avghX, avghY, avgH, avgS, avgL;
  double sumhX = 0, sumhY = 0, sumS = 0, sumL = 0;
  unsigned int numPixel = 0;
  
  HSLAPixel* curr;
  for (unsigned int x = ul.first; x < w+ul.first; x++) {
    for (unsigned int y = ul.second; y < h+ul.second; y++) {
      curr = im.getPixel(x, y);

      sumhX += Deg2X(curr->h);
      sumhY += Deg2Y(curr->h);
      sumS += curr->s;
      sumL += curr->l;

      numPixel++;
    }
  }
  avghX = sumhX/numPixel;
  avghY = sumhY/numPixel;
  avgH = XY2Deg(avghX, avghY);
  avgS = sumS/numPixel;
  avgL = sumL/numPixel;
  HSLAPixel avgp = HSLAPixel(avgH, avgS, avgL);

  Node* node = new Node(ul, w, h, avgp);
  
  if (w != 1 || h != 1) {
    unsigned int wA, hA, wB, hB;
    pair<unsigned int, unsigned int> ulB;
    if (h > w) {  // vertical
      wA = w; wB = w;
      hA = h / 2;
      hB = h - hA;
      ulB = make_pair(ul.first, ul.second + hA);
    } else { // horizontal
      hA = h; hB = h;
      wA = w / 2;
      wB = w - wA;
      ulB = make_pair(ul.first + wA, ul.second);
    }

    node->A = BuildNode(im, ul, wA, hA);
    node->B = BuildNode(im, ulB, wB, hB);
  }

  return node;
}

////////////////////////////////
// PTree public member functions
////////////////////////////////

/*
*  Constructor that builds the PTree using the provided PNG.
*
*  The PTree represents the sub-image (actually the entire image) from (0,0) to (w-1, h-1) where
*  w-1 and h-1 are the largest valid image coordinates of the original PNG.
*  Each node corresponds to a rectangle of pixels in the original PNG, represented by
*  an (x,y) pair for the upper-left corner of the rectangle, and two unsigned integers for the
*  number of pixels on the width and height dimensions of the rectangular sub-image region the
*  node defines.
*
*  A node's two children correspond to a partition of the node's rectangular region into two
*  equal (or approximately equal) size regions which are either tiled horizontally or vertically.
*
*  If the rectangular region of a node is taller than it is wide, then its two children will divide
*  the region into vertically-tiled sub-regions of equal height:
*  +-------+
*  |   A   |
*  |       |
*  +-------+
*  |   B   |
*  |       |
*  +-------+
*
*  If the rectangular region of a node is wider than it is tall, OR if the region is exactly square,
*  then its two children will divide the region into horizontally-tiled sub-regions of equal width:
*  +-------+-------+
*  |   A   |   B   |
*  |       |       |
*  +-------+-------+
*
*  If any region cannot be divided exactly evenly (e.g. a horizontal division of odd width), then
*  child B will receive the larger half of the two subregions.
*
*  When the tree is fully constructed, each leaf corresponds to a single pixel in the PNG image.
* 
*  For the average colour, this MUST be computed separately over the node's rectangular region.
*  Do NOT simply compute this as a weighted average of the children's averages.
*  The functions defined in hue_utils.h and implemented in hue_utils.cpp will be very useful.
*  Computing the average over many overlapping rectangular regions sounds like it will be
*  inefficient, but as an exercise in theory, think about the asymptotic upper bound on the
*  number of times any given pixel is included in an average calculation.
* 
*  PARAM: im - reference image which will provide pixel data for the constructed tree's leaves
*  POST:  The newly constructed tree contains the PNG's pixel data in each leaf node.
*/
PTree::PTree(PNG& im) {
  root = BuildNode(im, make_pair(0,0), im.width(), im.height());
}

/*
*  Copy constructor
*  Builds a new tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  This tree is constructed as a physically separate copy of other tree.
*/
PTree::PTree(const PTree& other) {
  Copy(other);
}

/*
*  Assignment operator
*  Rebuilds this tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  If other is a physically different tree in memory, all pre-existing dynamic
*           memory in this tree is deallocated and this tree is reconstructed as a
*           physically separate copy of other tree.
*         Otherwise, there is no change to this tree.
*/
PTree& PTree::operator=(const PTree& other) {
  if (this != &other) {
    ClearHelper(root);
    root = CopyHelper(other.root);
  }

  return *this;
}

/*
*  Destructor
*  Deallocates all dynamic memory associated with the tree and destroys this PTree object.
*/
PTree::~PTree() {
  Clear();
  
}

/*
*  Traverses the tree and puts the leaf nodes' color data into the nodes'
*  defined image regions on the output PNG.
*  For non-pruned trees, each leaf node corresponds to a single pixel that will be coloured.
*  For pruned trees, each leaf node may cover a larger rectangular region that will be
*  entirely coloured using the node's average colour attribute.
*
*  You may want to add a recursive helper function for this!
*
*  RETURN: A PNG image of appropriate dimensions and coloured using the tree's leaf node colour data
*/
PNG PTree::Render() const {
  // replace the line below with your implementation
  PNG img(root->width, root->height);
  RenderHelper(root, img);
  return img;
}

void PTree::RenderHelper(Node* n, PNG& img) const {
  if (n == NULL) {
    return;
  } else if (n->A == NULL && n->B == NULL) {
    for (unsigned int x = n->upperleft.first; x < n->width + n->upperleft.first; x++) {
      for (unsigned int y = n->upperleft.second; y < n->height + n->upperleft.second; y++) {
        HSLAPixel *pixel = img.getPixel(x, y);
        *pixel = n->avg;
      }
    }
    return;
  }
  RenderHelper(n->A, img);
  RenderHelper(n->B, img);
}

/*
*  Trims subtrees as high as possible in the tree. A subtree is pruned
*  (its children are cleared/deallocated) if ALL of its leaves have colour
*  within tolerance of the subtree root's average colour.
*  Pruning criteria should be evaluated on the original tree, and never on a pruned
*  tree (i.e. we expect that Prune would be called on any tree at most once).
*  When processing a subtree, you should determine if the subtree should be pruned,
*  and prune it if possible before determining if it has subtrees that can be pruned.
* 
*  You may want to add (a) recursive helper function(s) for this!
*
*  PRE:  This tree has not been previously pruned (and is not copied/assigned from a tree that has been pruned)
*  POST: Any subtrees (as close to the root as possible) whose leaves all have colour
*        within tolerance from the subtree's root colour will have their children deallocated;
*        Each pruned subtree's root becomes a leaf node.
*/
void PTree::Prune(double tolerance) {
  // add your implementation below
  PruneHelper(root, tolerance);
}

void PTree::PruneHelper(Node* n, double &tolerance) {
  if (n == NULL || (n->A == NULL && n->B == NULL)) {
    return;
  }
  if (ShouldPrune(n, n->avg, tolerance)) {
    ClearHelper(n->A);
    ClearHelper(n->B);
    n->A = NULL;
    n->B = NULL;
    return;
  } else {
    PruneHelper(n->A, tolerance);
    PruneHelper(n->B, tolerance);
  }
}

bool PTree::ShouldPrune(Node* n, HSLAPixel &avg, double &tolerance) {
  if (n->A == NULL && n->B == NULL) {
    return (avg.dist(n->avg) <= tolerance);
  } else {
    return (ShouldPrune(n->A, avg, tolerance) && ShouldPrune(n->B, avg, tolerance));
  }
}

/*
*  Returns the total number of nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::Size() const {
  return SizeHelper(root, 0);
}

int PTree::SizeHelper(Node* root, int total) const {
  if (root == NULL) {
    return 0;
  }
  total += SizeHelper(root->A, total) + SizeHelper(root->B, total) + 1;
  return total;
}

/*
*  Returns the total number of leaf nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::NumLeaves() const {
  return NumLeavesHelper(root, 0);
}

int PTree::NumLeavesHelper(Node* root, int total) const {
  if (root == NULL) {
    return 0;
  }
  if (root->A == NULL && root->B == NULL) {
    return 1;
  }
  total += NumLeavesHelper(root->A, total) + NumLeavesHelper(root->B, total);
  return total;
}

/*
*  Rearranges the nodes in the tree, such that a rendered PNG will be flipped horizontally
*  (i.e. mirrored over a vertical axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped horizontally.
*/
void PTree::FlipHorizontal() {
  HorizontalHelper(root, root->width);
}

void PTree::HorizontalHelper(Node* n, unsigned int w) {
  if (n == NULL) {
    return;
  }

  unsigned int upperright = n->upperleft.first + n->width - 1;
  n->upperleft.first = w-1-upperright;

  HorizontalHelper(n->A, w);
  HorizontalHelper(n->B, w);
}

/*
*  Like the function above, rearranges the nodes in the tree, such that a rendered PNG
*  will be flipped vertically (i.e. mirrored over a horizontal axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped vertically.
*/
void PTree::FlipVertical() {
  // add your implementation below
  VerticalHelper(root, root->height);
}

void PTree::VerticalHelper(Node* n, unsigned int h) {
  if (n == NULL) {
    return;
  }

  unsigned int lowerleft = n->upperleft.second + n->height - 1;
  n->upperleft.second = h-1-lowerleft;

  VerticalHelper(n->A, h);
  VerticalHelper(n->B, h);
}

/*
    *  Provides access to the root of the tree.
    *  Dangerous in practice! This is only used for testing.
    */
Node* PTree::GetRoot() {
  return root;
}

//////////////////////////////////////////////
// PERSONALLY DEFINED PRIVATE MEMBER FUNCTIONS
//////////////////////////////////////////////