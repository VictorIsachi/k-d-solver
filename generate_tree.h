#ifndef GENERATE_TREE_H
#define GENERATE_TREE_H

#include "basic_def.h"

/**
 * @brief given a set of points build a k-d tree for said set
 * @param pts the set of points to be arranged in a k-d tree
 * @return the root of the k-d tree
 */
node_t* build_tree(const point_t* pts[]);

/**
 * @brief deallocates the tree
 * @param node the root of the tree to be deallocated
 */
void free_tree(node_t* node);

#endif /*GENERATE_TREE_H*/