#include "kler.h"

/* NCURSES FUNCTIONS */

void updateTape()
{
}

/* print the string containing instructions to the inst window */
void updateInstructions(WINDOW *inst_win, char *inst_str, int cur) //{{{
{
	int height, width;	
	getmaxyx(inst_win, height, width);
	int length = strlen(inst_str);

	int y = height / 2;
	int x = width / 2;

	wattron(inst_win, A_BOLD);
	mvwaddchar(inst_win, y, x, inst_str[cur]);
	wattron(inst_win, A_BOLD);

	cur--;
	do {
		cur--;
	} while (x > INST_PADDING && cur >= 0);
} //}}}

/* BF EXECUTION FUNCTIONS */

/* Execute a _single_ character on the tape
 * (loops excluded) */
void execChar(const char inp) //{{{
{
	switch (inp) {
		case '+':
			(*tape.tc)++;
			break;
		case '-':
			(*tape.tc)--;
			break;
		case '>':
			tape.tc++;
			break;
		case '<':
			if (tape.start < tape.tc)
				tape.tc--;
			break;
		case ',':
			*tape.tc = getInput();
			break;
		case '.':
			putOutput(*tape.tc);
			break;
	}
}
//}}}

/* executes a null-terminated instruction string.
 * handles loops itself, calls execChar() for the rest*/
int execFile(FILE *in_file) //{{{
{
	unsigned int loop_depth = 0, no_exec_loop_depth = 0;
	int loop_stack[MAX_LOOP_DEPTH];
	bool exec = true;
	/* We will use loop_stack to hold the locations of the loop points so we
	 * can skip back to them. exec is a flag so that inside a loop we are only
	 * loocking for the matching bracket, not executing the code if we entered
	 * it with a zero on the cell */

	char inp;
	while ( (inp = fgetc(in_file)) != EOF) {
		/* FIXME nested loops still don't work?
		 * or something else maybe, but hello world don't work*/
		switch (inp) {
			/* Store the file position on encountering a 
			 * loop onto the stack */
			case '[':
				loop_stack[loop_depth] = ftell(in_file);
				/* If we are at a 0 cell, then we move forward to 
				 * the matching bracket without executing */
				if (*tape.tc == 0 && exec) {
					no_exec_loop_depth = loop_depth;
					exec = false;
				}
				loop_depth++;
				break;
			case ']':
				--loop_depth;
				/* If we are in exec mode, jump back to the matching bracket 
				 *  the -1 is to account for the extra fgetc() that will occur */
				if (exec)
					fseek(in_file, loop_stack[loop_depth] - 1, SEEK_SET);
				/* If we found the matching bracket for the loop which
				 * we were not supposed to exec */
				else if (loop_depth == no_exec_loop_depth)
					exec = true;
				break;
			default:
				if (exec)
					execChar(inp);
				break;
		}

		if (loop_depth > MAX_LOOP_DEPTH)
			return ERROR_LOOP_DEPTH_EXCEEDED;
	}
	
	if (loop_depth)
		return ERROR_UNMATCHED_BRACES;

	return NO_ERROR;
} //}}}

/* input function to be called by execChar() */
cell_t getInput() //{{{
{
	cell_t inp;
	if (flag.ncurses) {
		inp = getch();
	}
	else {
		inp = getchar();
	}

	return inp;
} //}}}

/* output function to be called by execChar() */
void putOutput(cell_t out) //{{{
{
	if (flag.ncurses) {
		addch(out);
	}
	else {
		putchar(out);
	}
} //}}}
