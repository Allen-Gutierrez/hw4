#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;

// helper function
bool checkDepth(Node * root, int currentDepth, int &leafDepth) {

  // Base Case: If the root is null, return true since empty
    if (root == nullptr) {
        return true;  // Null node doesn't affect leaf path length.
    }

    // If at leaf node, check the leaf depth.
    if (root->left == nullptr && root->right == nullptr) {
        // If first leaf, set the leaf depth.
        if (leafDepth == -1) {
            leafDepth = currentDepth;
        } else {
            // If this leaf's depth is different from the first leaf's depth, return false.
            if (currentDepth != leafDepth) {
                return false;
            }
        }
        return true;
    }

    // Recursively check the left and right subtrees.
    bool leftResult = checkDepth(root->left, currentDepth + 1, leafDepth);
    bool rightResult = checkDepth(root->right, currentDepth + 1, leafDepth);
    
    return leftResult && rightResult;
}
 
bool equalPaths(Node * root)
{
    // Add your code below
    int leafDepth = -1;  // To track the depth of the first leaf node.
    return checkDepth(root, 0, leafDepth);
    
  
}

