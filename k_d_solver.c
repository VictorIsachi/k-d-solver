#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "search_tree.h"
#include "generate_tree.h"

const point_t* query_point;//global variable that keeps track of the current query
float bounds[2*DIM_K];//global array that keeps track of the bounds of the current node
float min_dist;//global variable that keep track of the distrance from the nearest point found
const point_t* near_neighbor;//global variable that keeps track of the closest point found
bool flag_done;//global variable that keeps track of the state of search

void search_alg(bool f_eu, bool f_bwb, const node_t* node)
{
	if (f_eu && f_bwb)
		search_tree_eu_m(node);
	else if (!f_eu && !f_bwb)
		search_tree(node);
	else if (f_eu && !f_bwb)
		search_tree_eu(node);
	else
		search_tree_m(node);
}

static void initialize()
{
	for (int i = 0; i < DIM_K; ++i)
	{
		bounds[2*i] = NINFTY;
		bounds[2*i+1] = PINFTY;
	}
	min_dist = PINFTY;
	near_neighbor = NULL;
	flag_done = false;
}

void print_tree(const node_t* node)
{
	if (node == NULL)
		return;
	printf("NODE:");
	for (int i = 0; i < DIM_K; ++i)
		printf(" %f", node -> pto -> keys[i]);
	printf(" ");
	if (node -> l_son != NULL)
	{
		printf("has LEFT son:");
		for (int i = 0; i < DIM_K; ++i)
			printf(" %f", node -> l_son -> pto -> keys[i]);
		printf("; ");

	}
	if (node -> r_son != NULL)
	{
		printf("has RIGHT son:");
		for (int i = 0; i < DIM_K; ++i)
			printf(" %f", node -> r_son -> pto -> keys[i]);
		printf("; ");
	}
	if (node -> l_son == NULL && node -> r_son == NULL)
		printf("has NO son;");
	printf("\n");
	print_tree(node -> l_son);
	print_tree(node -> r_son);
	return;
}

static double time_passed(clock_t start, clock_t end)
{
	return (end - start) / (double) CLOCKS_PER_SEC;
}

void search_proc(bool f_eu, bool f_bwb, clock_t start, clock_t end, node_t* root)
{
	initialize();
	printf("Initialized the global variables for the search\n");
	printf("Algorithm options enabled:");
	if (f_eu == true)
		printf(" -e");
	if (f_bwb == true)
		printf(" -m");
	if (f_eu == false && f_bwb == false)
		printf(" none");
	printf("\n");
	//printf("Started keeping track of time for search\n");
	start = clock();
	search_alg(f_eu, f_bwb, root);
	end = clock();
	//printf("Found nearest neighbor\n");
	//printf("Stoped the clock\n");
	printf("NN found in %f seconds\n", time_passed(start, end));
	printf("Nearest neighbor:");
	for (int j = 0; j < DIM_K; ++j)
		printf(" %f", near_neighbor -> keys[j]);
	printf("\n");
}

int main(int argc, char *argv[])
{
	(void)argc;
	bool heur=false, no_bwb=false, test=false;
	clock_t start, end;

	char* first_command = argv[1];
	if (first_command == NULL){fprintf(stderr, "Invalid command\n"); return 1;}
	if (first_command[0] == '-')
	{
		if (first_command[1] == 'e')
			heur = true;
		else if (first_command[1] == 'm')
			no_bwb = true;
		else if (first_command[1] == 't')
			test = true;
		else {fprintf(stderr, "Invalid command\n"); return 1;}
	}
	char* second_command = argv[2];
	if (second_command == NULL){fprintf(stderr, "Invalid command\n"); return 1;}
	if (second_command[0] == '-')
	{
		if (test != true)
		{
			if (second_command[1] == 'e')
			{
				if (heur != true)
					heur = true;
				else {fprintf(stderr, "Invalid command\n"); return 1;}
			}
			else if (second_command[1] == 'm')
			{
				if (no_bwb != true)
					no_bwb = true;
				else {fprintf(stderr, "Invalid command\n"); return 1;}
			}
			else {fprintf(stderr, "Invalid command\n"); return 1;}
		}
		else {fprintf(stderr, "Invalid command\n"); return 1;}
	}

	FILE *points_file, *query_file;
	if (heur && no_bwb)
	{
		points_file = fopen(argv[3], "r");
		if (points_file == NULL){fprintf(stderr, "Can't open points_file\n"); return 1;}
		query_file = fopen(argv[4], "r");
		if (query_file == NULL){fprintf(stderr, "Can't open query_file\n"); return 1;}
	}
	else if (!heur && !no_bwb && !test)
	{
		points_file = fopen(argv[1], "r");
		if (points_file == NULL){fprintf(stderr, "Can't open points_file\n"); return 1;}
		query_file = fopen(argv[2], "r");
		if (query_file == NULL){fprintf(stderr, "Can't open query_file\n"); return 1;}
	}
	else
	{
		points_file = fopen(argv[2], "r");
		if (points_file == NULL){fprintf(stderr, "Can't open points_file\n"); return 1;}
		query_file = fopen(argv[3], "r");
		if (query_file == NULL){fprintf(stderr, "Can't open query_file\n"); return 1;}
	}

	point_t points[PTS_N];
	for (int i = 0; i < PTS_N; ++i)
		for (int j = 0; j < DIM_K; ++j)
			if (fscanf(points_file, "%f", &points[i].keys[j]) != 1)
			{fprintf(stderr, "Error reading points\n"); return 1;}

	point_t querys[PTS_Q];
	for (int i = 0; i < PTS_Q; ++i)
		for (int j = 0; j < DIM_K; ++j)
			if (fscanf(query_file, "%f", &querys[i].keys[j]) != 1)
			{fprintf(stderr, "Error reading querys\n"); return 1;}

	fclose(points_file);
	fclose(query_file);

	
	//CHECK IF THE FILES HAVE BEEN READ CORRECTLY
	//printf("Read points:\n");
	//for (int i = 0; i < PTS_N; ++i)
	//{
	//	printf("Point #%d:", i+1);
	//	for (int j = 0; j < DIM_K; ++j)
	//		printf(" %f", points[i].keys[j]);
	//	printf("\n");
	//}
	//for (int i = 0; i < PTS_Q; ++i)
	//{
	//	printf("Query point #%d:", i+1);
	//	for (int j = 0; j < DIM_K; ++j)
	//		printf(" %f", querys[i].keys[j]);
	//	printf("\n");
	//}
	

	const point_t* p_points[PTS_N];
		for (int i = 0; i < PTS_N; ++i)
			p_points[i] = &points[i];
	printf("Generated list of pointers to the points\n");

	node_t* root;
	for (int i = 0; i < BUILD_REP; ++i)
	{
		printf("build_tree iteration #%d\n", i+1);
		//printf("Started keeping track of time for tree generation\n");
		start = clock();
		root = build_tree(p_points);
		end = clock();
		printf("Built the k-d tree\n");
		//printf("Stoped the clock\n");
		printf("Tree generated in %f seconds\n", time_passed(start, end));
	}
	
	//CHECK IF THE TREE HAS BEEN GENERATED CORRECTLY
	//printf("Printing the k-d tree\n");
	//print_tree(root);

	for (int i = 0; i < PTS_Q; ++i)
	{
		
		printf("Query point #%d\n", i+1);
		query_point = &querys[i];
		if (test == true)
		{
			search_proc(false, false, start, end, root);
			search_proc(false, true, start, end, root);
			search_proc(true, false, start, end, root);
			search_proc(true, true, start, end, root);
		}
		else
			search_proc(heur, no_bwb, start, end, root);
		initialize();
		printf("Initialized the global varibles for the linear scan\n");
		//printf("Started keeping track of time for linear scan\n");
		start = clock();
		linear_search(p_points);
		end = clock();
		//printf("Found nearest neighbor\n");
		//printf("Stoped the clock\n");
		printf("NN found in %f seconds\n", time_passed(start, end));
		printf("Nearest neighbor:");
		for (int j = 0; j < DIM_K; ++j)
			printf(" %f", near_neighbor -> keys[j]);
		printf("\n");
	}

	free_tree(root);
	root = NULL;
	return 0;
}