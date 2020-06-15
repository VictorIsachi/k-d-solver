#ifndef SEARCH_TREE_H
#define SEARCH_TREE_H

#include <stdbool.h>
#include "basic_def.h"

extern const point_t* query_point;//global variable that keeps track of the current query
extern float bounds[2*DIM_K];//global array that keeps track of the bounds of the current node
extern float min_dist;//global variable that keep track of the distrance from the nearest point found
extern const point_t* near_neighbor;//global variable that keeps track of the closest point found
extern bool flag_done;//global variable that keeps track of the state of search

/**
 * @brief searches the tree in order to find the nearest neighbor of the query_point in O(log(PTS_N))
 * @param node the root of the tree to be searched
 * @return no return value. Keeps the nearest point found in the global variable 
 * near_neighbor. When the function returns the nearest neighbor of query_point is stored
 * in near_neighbor
 */
void search_tree(const node_t* node);

/**
 * @brief searches the tree in order to find the nearest neighbor of the query_point
 * @param node the root of the tree to be searched
 * @return no return value. Keeps the nearest point found in the global variable 
 * near_neighbor. When the function returns the nearest neighbor of query_point is stored
 * in near_neighbor
 */
void search_tree_m(const node_t* node);

/**
 * @brief searches the tree in order to find the nearest neighbor of the query_point
 * @param node the root of the tree to be searched
 * @return no return value. Keeps the nearest point found in the global variable 
 * near_neighbor. When the function returns the nearest neighbor of query_point is stored
 * in near_neighbor
 */
void search_tree_eu(const node_t* node);

/**
 * @brief searches the tree in order to find the nearest neighbor of the query_point
 * @param node the root of the tree to be searched
 * @return no return value. Keeps the nearest point found in the global variable 
 * near_neighbor. When the function returns the nearest neighbor of query_point is stored
 * in near_neighbor
 */
void search_tree_eu_m(const node_t* node);

/**
 * @brief searches the array of points in order to find the nearest neighbor of the query_point in O(PTS_N)
 * @param pts array of points to be scanned
 * @return no return value. Keeps the nearest point found in the global variable 
 * near_neighbor. When the function returns the nearest neighbor of query_point is stored
 * in near_neighbor
 */
void linear_search(const point_t* pts[]);

#endif /*SEARCH_TREE_H*/