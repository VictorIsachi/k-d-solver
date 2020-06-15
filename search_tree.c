#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include "search_tree.h"

/**
 * @brief checks if the ball with center query_point and radius min_dist is entirely
 * contained within the bounds[] of the current node
 * @param no explicit parametre. Uses global variables
 * @return true if the test succeedes, false otherwise
 */

static bool ball_within_bounds();

/**
 * @brief checks if the bounds[] of the current node overlap the ball with center 
 * query_point and radius min_dist
 * @param no explicit parametre. Uses global variables
 * @return true if the test succeedes, false otherwise
 */
static bool bounds_overlap_ball(); 

//calculates the (Minkowski) distance between 2 points
static float dist(const point_t* point1, const point_t* point2)
{
	double dist = 0;
	if (MINK_M == INFTY_M)
	{
		//DBG
		//printf("MINKOWSKI INFTY METRIC\n");

		double found_dist = 0;
		for (int i = 0; i < DIM_K; ++i)
		{
			found_dist = fabs(point1 -> keys[i] - point2 -> keys[i]);
			if (found_dist > dist)
				dist = found_dist;
		}
		return (float) dist;
	}
	else
	{
		for (int i = 0; i < DIM_K; ++i)
			dist += pow(fabs(point1 -> keys[i] - point2 -> keys[i]), MINK_M);
		return (float) pow(dist, (double) 1/MINK_M);
	}
}

static bool ball_within_bounds()
{
	//careful with the bounds! the keys range in [0, DIM_K-1] and not [1, DIM_k]
	//as written in the paper
	for (int i = 0; i < DIM_K; ++i)
		if (((bounds[2*i] + min_dist) > query_point -> keys[i]) || 
			((bounds[(2*i)+1] - min_dist) < query_point -> keys[i]))
		{
			//DBG
			//printf("BWB FAILED\n");

			return false;
		}
	//DBG
	//printf("BWB SUCCESS\n");

	return true;
}

static bool bounds_overlap_ball()
{
	if (MINK_M == INFTY_M)
	{
		//DBG
		//printf("MINKOWSKI INFTY METRIC\n");

		for (int i = 0; i < DIM_K; ++i)
		{
			if (query_point -> keys[i] < bounds[2*i])
			{
				if (fabs(query_point -> keys[i] - bounds[2*i]) > min_dist)
				{
					//DBG
					//printf("BOB FAILED\n");

					return false;
				}
			}
			else if (query_point -> keys[i] > bounds[2*i+1])
			{
				if (fabs(query_point -> keys[i] - bounds[2*i+1]) > min_dist)
				{
					//DBG
					//printf("BOB FAILED\n");

					return false;
				}
			}
		}
		//DBG
		//printf("BOB SUCCESS\n");

		return true;
	}
	else
	{
		double sum = 0;
		for (int i = 0; i < DIM_K; ++i)
		{
			if (query_point -> keys[i] < bounds[2*i])
			{
				sum += pow(fabs(query_point -> keys[i] - bounds[2*i]), MINK_M);
				if (sum > pow(min_dist, MINK_M))
				{
					//DBG
					//printf("BOB FAILED\n");

					return false;
				}
			}
			else if (query_point -> keys[i] > bounds[2*i+1])
			{
				sum += pow(fabs(query_point -> keys[i] - bounds[2*i+1]), MINK_M);
				if (sum > pow(min_dist, MINK_M))
				{
					//DBG
					//printf("BOB FAILED\n");

					return false;
				}
			}
		}

		//DBG
		//printf("BOB SUCCESS\n");

		return true;
	}
}

void search_tree(const node_t* node)
{
	//DBG
	//printf("SEARCH_TREE\n");
	//printf("CURRENT NODE:");
	//for (int i = 0; i < DIM_K; ++i)
	//	printf(" %f", node -> pto -> keys[i]);
	//printf("\n");

	float found_dist = dist(node -> pto, query_point);

	//DBG
	//printf("CALCULATED NEW DISTANCE: %f\n", found_dist);

	if (found_dist < min_dist)
	{
		near_neighbor = node -> pto;
		min_dist = found_dist;

		//DBG
		//printf("NEW NN FOUND:");
		//for (int i = 0; i < DIM_K; ++i)
		//	printf(" %f", node -> pto -> keys[i]);
		//printf("\n");
	}

	float temp_bound;
	if (node -> disc != -1)
	{
		//DBG
		//printf("NOT A LEAF NODE\n");

		if (node -> pto -> keys[node -> disc] >= query_point -> keys[node -> disc])
		{
			//DBG
			//printf("QUERY TO THE LEFT\n");

			if (node -> l_son != NULL)
			{
				//DBG
				//printf("SEARCHING LEFT SUBTREE\n");

				temp_bound = bounds[2*(node -> disc)+1];
				bounds[2*(node -> disc)+1] = node -> pto -> keys[node -> disc];

				//DBG
				//printf("UPDATED BOUNDS:\n");
				//for (int i = 0; i < DIM_K; ++i)
				//	printf("[%f, %f]\n", bounds[2*i], bounds[2*i+1]);

				search_tree(node -> l_son);
				bounds[2*(node -> disc)+1] = temp_bound;

				//DBG
				//printf("RESTORED BOUNDS:\n");
				//for (int i = 0; i < DIM_K; ++i)
				//	printf("[%f, %f]\n", bounds[2*i], bounds[2*i+1]);

				if (flag_done)
					return;
			}
			temp_bound = bounds[2*(node -> disc)];
			bounds[2*(node -> disc)] = node -> pto -> keys[node -> disc];

			//DBG
			//printf("UPDATED BOUNDS FOR POTENTIAL RIGHT SUBTREE SEARCH:\n");
			//for (int i = 0; i < DIM_K; ++i)
			//	printf("[%f, %f]\n", bounds[2*i], bounds[2*i+1]);

			if (node -> r_son != NULL && bounds_overlap_ball())
			{
				//DBG
				//printf("SEARCHING RIGHT SUBTREE\n");

				search_tree(node -> r_son);
			}
			bounds[2*(node -> disc)] = temp_bound;

			//DBG
			//printf("RESTORED BOUNDS:\n");
			//for (int i = 0; i < DIM_K; ++i)
			//	printf("[%f, %f]\n", bounds[2*i], bounds[2*i+1]);

			if (flag_done)
				return;
		}
		else
		{
			//DBG
			//printf("QUERY TO THE RIGHT\n");

			if (node -> r_son != NULL)
			{
				//DBG
				//printf("SEARCHING RIGHT SUBTREE\n");

				temp_bound = bounds[2*(node -> disc)];
				bounds[2*(node -> disc)] = node -> pto -> keys[node -> disc];

				//DBG
				//printf("UPDATED BOUNDS:\n");
				//for (int i = 0; i < DIM_K; ++i)
				//	printf("[%f, %f]\n", bounds[2*i], bounds[2*i+1]);

				search_tree(node -> r_son);
				bounds[2*(node -> disc)] = temp_bound;

				//DBG
				//printf("RESTORED BOUNDS:\n");
				//for (int i = 0; i < DIM_K; ++i)
				//	printf("[%f, %f]\n", bounds[2*i], bounds[2*i+1]);

				if (flag_done)
					return;
			}
			temp_bound = bounds[2*(node -> disc)+1];
			bounds[2*(node -> disc)+1] = node -> pto -> keys[node -> disc];

			//DBG
			//printf("UPDATED BOUNDS FOR POTENTIAL LEFT SUBTREE SEARCH:\n");
			//for (int i = 0; i < DIM_K; ++i)
			//	printf("[%f, %f]\n", bounds[2*i], bounds[2*i+1]);

			if (node -> l_son != NULL && bounds_overlap_ball())
			{
				//DBG
				//printf("SEARCHING LEFT SUBTREE\n");

				search_tree(node -> l_son);
			}
			bounds[2*(node -> disc)+1] = temp_bound;

			//DBG
			//printf("RESTORED BOUNDS:\n");
			//for (int i = 0; i < DIM_K; ++i)
			//	printf("[%f, %f]\n", bounds[2*i], bounds[2*i+1]);

			if (flag_done)
				return;
		}
	}
	if (ball_within_bounds())
		flag_done = true;
	return;
}

void search_tree_m(const node_t* node)
{
	//DBG
	//printf("SEARCH_TREE_M\n");

	float found_dist = dist(node -> pto, query_point);
	if (found_dist < min_dist)
	{
		near_neighbor = node -> pto;
		min_dist = found_dist;
	}

	float temp_bound;
	if (node -> disc != -1)
	{
		if (node -> pto -> keys[node -> disc] >= query_point -> keys[node -> disc])
		{
			if (node -> l_son != NULL)
			{
				temp_bound = bounds[2*(node -> disc)+1];
				bounds[2*(node -> disc)+1] = node -> pto -> keys[node -> disc];
				search_tree_m(node -> l_son);
				bounds[2*(node -> disc)+1] = temp_bound;
			}
			temp_bound = bounds[2*(node -> disc)];
			bounds[2*(node -> disc)] = node -> pto -> keys[node -> disc];
			if (node -> r_son != NULL && bounds_overlap_ball())
				search_tree_m(node -> r_son);
			bounds[2*(node -> disc)] = temp_bound;
		}
		else
		{
			if (node -> r_son != NULL)
			{
				temp_bound = bounds[2*(node -> disc)];
				bounds[2*(node -> disc)] = node -> pto -> keys[node -> disc];
				search_tree_m(node -> r_son);
				bounds[2*(node -> disc)] = temp_bound;
			}
			temp_bound = bounds[2*(node -> disc)+1];
			bounds[2*(node -> disc)+1] = node -> pto -> keys[node -> disc];
			if (node -> l_son != NULL && bounds_overlap_ball())
				search_tree_m(node -> l_son);
			bounds[2*(node -> disc)+1] = temp_bound;
		}
	}
	return;
}

void search_tree_eu(const node_t* node)
{
	//DBG
	//printf("SEARCH_TREE_EU\n");

	float found_dist = dist(node -> pto, query_point);
	if (found_dist < min_dist)
	{
		near_neighbor = node -> pto;
		min_dist = found_dist;
	}

	float temp_bound;
	if (node -> disc != -1)
	{
		if (node -> pto -> keys[node -> disc] >= query_point -> keys[node -> disc])
		{
			if (node -> l_son != NULL)
			{
				temp_bound = bounds[2*(node -> disc)+1];
				bounds[2*(node -> disc)+1] = node -> pto -> keys[node -> disc];
				search_tree_eu(node -> l_son);
				bounds[2*(node -> disc)+1] = temp_bound;
				if (flag_done)
					return;
			}
			temp_bound = bounds[2*(node -> disc)];
			bounds[2*(node -> disc)] = node -> pto -> keys[node -> disc];
			if ((node -> r_son != NULL) && 
				(fabs(node -> pto -> keys[node -> disc] - query_point -> keys[node -> disc]) < min_dist))
				search_tree_eu(node -> r_son);
			bounds[2*(node -> disc)] = temp_bound;
			if (flag_done)
				return;
		}
		else
		{
			if (node -> r_son != NULL)
			{
				temp_bound = bounds[2*(node -> disc)];
				bounds[2*(node -> disc)] = node -> pto -> keys[node -> disc];
				search_tree_eu(node -> r_son);
				bounds[2*(node -> disc)] = temp_bound;
				if (flag_done)
					return;
			}
			temp_bound = bounds[2*(node -> disc)+1];
			bounds[2*(node -> disc)+1] = node -> pto -> keys[node -> disc];
			if (node -> l_son != NULL && 
				(fabs(node -> pto -> keys[node -> disc] - query_point -> keys[node -> disc]) < min_dist))
				search_tree_eu(node -> l_son);
			bounds[2*(node -> disc)+1] = temp_bound;
			if (flag_done)
				return;
		}
	}
	if (ball_within_bounds())
		flag_done = true;
	return;
}

void search_tree_eu_m(const node_t* node)
{
	//DBG
	//printf("SEARCH_TREE_EU_M\n");

	float found_dist = dist(node -> pto, query_point);
	if (found_dist < min_dist)
	{
		near_neighbor = node -> pto;
		min_dist = found_dist;
	}

	if (node -> disc != -1)
	{
		if (node -> pto -> keys[node -> disc] >= query_point -> keys[node -> disc])
		{
			if (node -> l_son != NULL)
				search_tree_eu_m(node -> l_son);
			if ((node -> r_son != NULL) && 
				(fabs(node -> pto -> keys[node -> disc] - query_point -> keys[node -> disc]) < min_dist))
				search_tree_eu_m(node -> r_son);
		}
		else
		{
			if (node -> r_son != NULL)
				search_tree_eu_m(node -> r_son);
			if (node -> l_son != NULL && 
				(fabs(node -> pto -> keys[node -> disc] - query_point -> keys[node -> disc]) < min_dist))
				search_tree_eu_m(node -> l_son);
		}
	}
	return;
}

void linear_search(const point_t* pts[])
{
	float found_dist = -1;

	for (int i = 0; i < PTS_N; ++i)
	{
		found_dist = dist(pts[i], query_point);
		if (found_dist < min_dist)
		{
			near_neighbor = pts[i];
			min_dist = found_dist;
		}
	}
}