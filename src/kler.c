#include "kler.h"

/* NCURSES FUNCTIONS */

/* update the visual represent of the tape */
void updateTape() //{{{
{
	int height, width;
	getmaxyx(win.tape, height, width);
	const int cur_cell = TAPE_POS;
	int cur;

	int y = PADDING + 1;
	int x = PADDING + 1;

	werase(win.tape);
	/* decide the starting cell such that we print the cur cell as well as
	 * nearby tape cells */
	if (cur_cell > height - 2 * PADDING)
		cur = cur_cell - (height - 2 * PADDING);
	else
		cur = 0;

	/* print the tape cells, highlighting cur cell */
	while (y < height - PADDING && cur < tape.size) {
		if (cur == cur_cell) {
			wattron(win.tape, A_STANDOUT);
			mvwprintw(win.tape, y++, x, "> #%u: %u", cur, tape.start[cur]);
			cur++;
			wattroff(win.tape, A_STANDOUT);
		}
		else {
			mvwprintw(win.tape, y++, x, "#%u: %u", cur, tape.start[cur]);
			cur++;
		}
	}
} //}}}

/* print the string containing instructions to the inst window */
void updateInstructions(char *inst_str, const int cur_inst) //{{{
{
	int height, width;	
	getmaxyx(win.inst, height, width);
	int length = strlen(inst_str);
	int cur = cur_inst;

	int y = height / 2;
	int x = width / 2;

	werase(win.inst);
	// Print cur_inst
	wattron(win.inst, A_STANDOUT);
	mvwaddch(win.inst, y, x, inst_str[cur]);
	wattron(win.inst, A_STANDOUT);

	// Print the chars before cur_inst
	do {
		mvwaddch(win.inst, y, --x, inst_str[--cur]);
	} while (x > PADDING && cur > 0);

	cur = cur_inst + 1;
	x = (width / 2) + 1;

	// Print the chars after cur_inst
	do {
		mvwaddch(win.inst, y, x++, inst_str[cur++]);
	} while (x < width - PADDING && cur < length);
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

	char inst_str[COLS];
	int cur_inst = -1;

	char inp;
	while ( (inp = fgetc(in_file)) != EOF) {
		/* FIXME nested loops still don't work?
		 * or something else maybe, but hello world don't work*/

		/* if in ncurses mode, we also have to display the things */
		if (flag.ncurses) {
			/* Push it to the string if it is a valid bf instruction */
			switch (inp) { 
				case '[': case ']': case '<': case '>': case '+': case '-': case '.': case ',':
					inst_str[++cur_inst] = inp;
					break;
			}

			updateInstructions(inst_str, cur_inst);
			wrefresh(win.inst);
		}

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

		if (flag.ncurses) {
			updateTape();
			wrefresh(win.tape);
			refresh();
		}
		if (loop_depth > MAX_LOOP_DEPTH)
			return ERROR_LOOP_DEPTH_EXCEEDED;

		usleep(DELAY);
		refresh();
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
