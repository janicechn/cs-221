/*
*  File:        ptree-private.h
*  Description: Private functions for the PTree class
*  Date:        2022-03-06 03:30
*
*               DECLARE YOUR PTREE PRIVATE MEMBER FUNCTIONS HERE
*/

#ifndef _PTREE_PRIVATE_H_
#define _PTREE_PRIVATE_H_

/////////////////////////////////////////////////
// DEFINE YOUR PRIVATE MEMBER FUNCTIONS HERE
//
// Just write the function signatures.
//
// Example:
//
// Node* MyHelperFunction(int arg_a, bool arg_b);
//
/////////////////////////////////////////////////
void ClearHelper(Node* root);
Node* CopyHelper(const Node* root);
void RenderHelper(Node* root);
void PruneHelper(Node* n, double &tolerance);
bool ShouldPrune(Node* n, HSLAPixel &avg, double &tolerance);
int SizeHelper(Node* root, int total) const;
int NumLeavesHelper(Node* root, int total) const;
void HorizontalHelper(Node* n, unsigned int w);
void VerticalHelper(Node* n, unsigned int h);
void RenderHelper(Node* n, PNG& img) const;



#endif