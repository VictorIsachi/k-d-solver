CFLAGS = -W -Wall -O2


.PHONY: clean
	rm *o

.PHONY: all
all: basic_def.h generate_tree.h generate_tree.c search_tree.h search_tree.c k_d_solver.c
	gcc $(CFLAGS) -o k_d_solver k_d_solver.c search_tree.c generate_tree.c -lm


basic_def: basic_def.h
	gcc $(CFLAGS) basic_def.h -o basic_def

generate_tree: generate_tree.h generate_tree.c
	gcc $(CFLAGS) -c generate_tree.c

search_tree: search_tree.h search_tree.c
	gcc $(CFLAGS) -c search_tree.c
