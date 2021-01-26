#ifndef KLER_H
#define KLER_H

#include <curses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define INIT_TAPE_SIZE 1024
#define MAX_LOOP_DEPTH 64
#define BF_BUF_INIT 1024
#define DELAY 100000
#define PADDING 2

#define TAPE_POS (unsigned int) (tape.tc - tape.start)

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
	WINDOW *tape;
	WINDOW *inst;
	WINDOW *io;
} win_grp_t;
extern win_grp_t win;

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

extern int delay;

/* NCURSES FUNCTIONS */
void updateTape();
void updateInstructions(char *, const int);

/* BF EXECUTION FUNCTIONS */
void execChar(const char);
int execFile(FILE *);
cell_t getInput();
void putOutput(cell_t);

#endif
