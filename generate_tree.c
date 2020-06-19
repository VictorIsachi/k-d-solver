#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "generate_tree.h"

#define SWAP(x, y) {const point_t* temp = x; x = y; y = temp;}

/**
 * @brief generates a leaf node
 * @param pts is a set that contains one point
 * @return a node is created with the point in pts and a pointer to it is returned
 */
static node_t* leaf_node(const point_t* pts[]);

/**
 * @brief generate a node of the tree with the specified parametres
 * @param d the discriminant of the node
 * @param pto the point to be associated with the node
 * @param l_son the left son of the node
 * @param r_son the right son of the node
 * @return pointer to the created node
 */
static node_t* gen_node(int d, const point_t *pto, const node_t *l_son, const node_t *r_son);

/**
 * @brief calculates the variance of the d-th keys of points in pts 
 * @param d index of the keys to be checked
 * @param pts points to be checked
 * @return the variance
 */
static float var_nodes(int d, const point_t* pts[]);

/**
 * @brief calculates the point with median key of index d
 * @param d index of the keys to be checked
 * @param pts points to be checked
 * @return pointer to the median point
 */
static point_t* median_point(int d, const point_t* pts[]);

/**
 * @brief finds the sets of the acceptable points for the left and right subtrees of a node Q
 * @param l_pts array of valid points for the left subtree of Q
 * @param r_pts array of valid points for the right subtree of Q
 * @param pts the set of all points to be evaluated in order to decide which can be in l_pts and which in r_pts
 * @param d the discriminant of Q
 * @param med the point to be associated with Q
 * @return the function does not return any value but keeps the valid points in l_pts and r_pts
 * (which should be empty at call)
 */
static void sub_tree_find(const point_t* l_pts[], const point_t* r_pts[], const point_t* pts[], int d, const point_t* med);

static node_t* leaf_node(const point_t* pts[])
{
	node_t* leaf = (node_t*) malloc(sizeof(node_t));
	if (leaf == NULL){fprintf(stderr, "Can't allocate enought memory for the leaf"); exit(1);}
	leaf -> pto = pts[0];
	leaf -> disc = -1;
	leaf -> l_son = NULL;
	leaf -> r_son = NULL;
	return leaf;
}

static node_t* gen_node(int d, const point_t *pto, const node_t *l_son, const node_t *r_son)
{
	node_t* internal = (node_t*) malloc(sizeof(node_t));
	if (internal == NULL){fprintf(stderr, "Can't allocate enought memory for the non-leaf"); exit(1);}
	internal -> pto = pto;
	internal -> disc = d;
	internal -> l_son = (node_t*)l_son;
	internal -> r_son = (node_t*)r_son;
	return internal;
}

//This function scans the entire dataset and since it is called at each level of the
//k-d tree the overall time complexity is O(Nlog(N)). IS THERE A BETTER WAY?
static float var_nodes(int d, const point_t* pts[])
{
	double sq_sum = 0;
	double sum_sq = 0;
	int i;
	for (i = 0; i < PTS_N; ++i)
	{
		if (pts[i] == NULL)
			break;
		//DBG
		//printf("VAR: READ POINT #%d:", i);
		//for (int j = 0; j < DIM_K; ++j)
		//	printf(" %f", pts[i] -> keys[j]);
		//printf("\n");

		sq_sum += pts[i] -> keys[d];
		sum_sq += (pts[i] -> keys[d])*(pts[i] -> keys[d]);
	}
	//if (i == 0)//useless since build_tree returns before calling this function if it recives 0 points in input
	//	return -1;
	sq_sum = sq_sum*sq_sum;
	return (float) ((double) 1/(i))*(sum_sq - ((double) 1/(i))*sq_sum);
}

//Lomuto partition
static int partition(const point_t* pts[], int d, int beginning, int end, int ind_pivot)
{
	const point_t* pivot = pts[ind_pivot];
	SWAP(pts[ind_pivot], pts[end])

	int p_ind = beginning;
	for (int i = beginning; i < end; ++i)
	{
		if ((pts[i] -> keys[d]) <= (pivot -> keys[d]))
		{
			SWAP(pts[i], pts[p_ind]);
			p_ind++;
		}
	}
	SWAP(pts[p_ind], pts[end]);

	return p_ind;
}

//This algorithm has expected complexity O(N) (so it does not affect the 
//performace of the algorithm which remains polylogarithmic), but worst case complexity is 
//O(N^2).
static point_t* quick_select(const point_t* pts[], int d, int beginning, int end, int med_index)
{
	if (beginning == end)
		return (point_t*)pts[beginning];

	int pivot_index = beginning + rand() % (end - beginning + 1);
	pivot_index = partition(pts, d, beginning, end, pivot_index);

	if (pivot_index == med_index)
		return (point_t*)pts[pivot_index];
	else if (pivot_index > med_index)
		return quick_select(pts, d, beginning, pivot_index - 1, med_index);
	else
		return quick_select(pts, d, pivot_index + 1, end, med_index);
}

//Potentially some outlier instances might take more than usual due to the
//impletentation.
static point_t* median_point(int d, const point_t* pts[])
{
	int num_pts;
	for (num_pts = 0; num_pts < PTS_N; num_pts++)
		if (pts[num_pts] == NULL)
			break;
	int med_index = num_pts/2;
	return quick_select(pts, d, 0, num_pts - 1, med_index);
}

//This function scans the entire dataset and since it is called at each level of the
//k-d tree the overall time complexity is O(Nlog(N)). IS THERE A BETTER WAY?
static void sub_tree_find(const point_t* l_pts[], const point_t* r_pts[], const point_t* pts[], int d, const point_t* med)
{
	int i, j;
	i = j = 0;
	for (int h = 0; h < PTS_N; h++)
	{
		if (med == pts[h])
			continue;
		if (pts[h] == NULL)
			return;

		if((med -> keys[d]) >= (pts[h] -> keys[d]))
			l_pts[i++] = pts[h];
		else
			r_pts[j++] = pts[h];
	}
}

node_t* build_tree(const point_t* pts[])
{
	//DBG
	//printf("NEW INSTANCE OF BUILD_TREE\n");

	int num_pts;
	for (num_pts = 0; num_pts < PTS_N; num_pts++)
		if (pts[num_pts] == NULL)
			break;

	//DBG
	//printf("COUNT BUILD_TREE SUCCESSFUL WITH RESULT %d\n", num_pts);
	//printf("FOUND POINTS IN BUILD_TREE:\n");
	//for (int i = 0; i < PTS_N; ++i)
	//{
	//	if (pts[i] == NULL)
	//		break;
	//	printf("POINT #%d:", i);
	//	for (int j = 0; j < DIM_K; ++j)
	//	{
	//		printf(" %f", pts[i] -> keys[j]);
	//	}
	//	printf("\n");
	//}

	if (num_pts == 0)
	{
		//DBG
		//printf("NO POINTS IN BUILD_TREE\n");

		return NULL;
	}
	if (num_pts == 1)
	{
		//DBG
		//printf("BASE CASE IN BUILD_TREE WITH POINT:");
		//for (int j = 0; j < DIM_K; ++j)
		//{
		//	printf(" %f", pts[0] -> keys[j]);
		//}
		//printf("\n");

		return leaf_node(pts);
	}

	int disc = -1;
	float temp_var = 0.0f;
	const point_t *med = NULL;
	const point_t *l_pts[num_pts], *r_pts[num_pts];
	for (int i = 0; i < num_pts; ++i){l_pts[i] = r_pts[i] = NULL;}

	//DBG
	//printf("INITIALIZATION OF BUILD_TREE SUCCESSFUL\n");

	float max_var = -1.0f;
	for (int i = 0; i < DIM_K; ++i)
	{
		if ((temp_var = var_nodes(i, pts)) > max_var)
		{
			max_var = temp_var;
			disc = i;
		}
		//DBG
		//printf("CALCULATED VARIANCE: %f AT INDEX %d\n", temp_var, i);
	}

	//MEASURE PERFORMANCE WITHOUT THE CALCULATION OF VARIANCE
	//static int disc_tracker = 0;
	//disc = disc_tracker;
	//disc_tracker = (disc_tracker+1)%DIM_K;

	//DBG
	//printf("FOUND DISC: %d\n", disc);

	med = median_point(disc, pts);

	//MEASURE PERFORMANCE WITHOUT THE CALCULATION OF MEDIAN
	//med = pts[0];

	//DBG
	//printf("FOUND MEDIAN:");
	//for (int i = 0; i < DIM_K; ++i)
	//	printf(" %f", med -> keys[i]);
	//printf("\n");

	sub_tree_find(l_pts, r_pts, pts, disc, med);

	//DBG
	//printf("SUCCESSFUL PARTITION INTO SUBSETS:\n");
	//printf("LEFT SUBSET:\n");
	//for (int i = 0; i < PTS_N; ++i)
	//{
	//	if (l_pts[i] == NULL)
	//		break;
	//	for (int j = 0; j < DIM_K; ++j)
	//		printf("%f ", l_pts[i] -> keys[j]);
	//	printf("\n");
	//}
	//printf("RIGHT SUBSET:\n");
	//for (int i = 0; i < PTS_N; ++i)
	//{
	//	if (r_pts[i] == NULL)
	//		break;
	//	for (int j = 0; j < DIM_K; ++j)
	//		printf("%f ", r_pts[i] -> keys[j]);
	//	printf("\n");
	//}
	//printf("GENERATING NEW NODE\n");

	return gen_node(disc, med, build_tree(l_pts), build_tree(r_pts));
}

void free_tree(node_t* node)
{
	if (node -> l_son != NULL)
		free_tree(node -> l_son);
	if (node -> r_son != NULL)
		free_tree(node -> r_son);

	free(node);
	node = NULL;
	return;
}