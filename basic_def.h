#ifndef BASIC_DEF_H
#define BASIC_DEF_H
#include <float.h>

#define DIM_K 2 //the dimensionality of the point 
#define PTS_N 100//the number of poins
#define PTS_Q 5 //the number of query points to be evaluated
#define BUILD_REP 1//number of times the k-d tree needs to be built, used for performance measurements
#define MINK_M (2.0)//the index of the minkowski metric used
#define INFTY_M (-1)//indicates the use of the minkowski metric of index = +infinity
#define PINFTY FLT_MAX//positive infinity
#define NINFTY (-FLT_MAX)//negative infinity

typedef struct point
{
	float keys[DIM_K];
} point_t;

typedef struct node{
	const point_t *pto;
	int disc;
	struct node *l_son;
	struct node *r_son;
} node_t;

#endif /*BASIC_DEF_H*/