#ifndef KLER_H
#define KLER_H

#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define INIT_TAPE_SIZE 1024
#define MAX_LOOP_DEPTH 64
#define BF_LINE_SIZE 1024
#define DELAY 1
#define INST_PADDING 2

enum {
	NO_ERROR,
	ERROR_UNMATCHED_BRACES,
	ERROR_LOOP_DEPTH_EXCEEDED
};

typedef char cell_t;

typedef struct {
	bool ncurses;
	bool output_file;
} flag_t;
extern flag_t flag;

typedef struct {
	cell_t *start;	
	cell_t *tc;
	size_t size;
} tape_t;
extern tape_t tape;

typedef struct {
	cell_t *start;
	cell_t *end;
} loop_t;

/* NCURSES FUNCTIONS */
/* BF EXECUTION FUNCTIONS */
void execChar(const char);
int execFile(FILE *);
cell_t getInput();
void putOutput(cell_t);

#endif
