#include "kler.h"

/*********************
 * NCURSES FUNCTIONS *
 *********************/

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
	wattroff(win.inst, A_STANDOUT);

	// Print the chars before cur_inst
	do {
		mvwaddch(win.inst, y, --x, inst_str[--cur]);
	} while (x > PADDING && cur > 0);

	cur = cur_inst + 1;
	x = (width / 2) + 1;

	// Print the chars after cur_inst
	while (x < width - PADDING && cur < length)
		mvwaddch(win.inst, y, x++, inst_str[cur++]);

} //}}}

/**************************
 * BF EXECUTION FUNCTIONS *
 **************************/

/* Take a file stream and return a null-terminated char string containing only
 * the valid BF characters */
char * onlyCode(char *in_string) //{{{
{
	size_t buf_size = BF_BUF_INIT;
	char *string = (char *) calloc(buf_size, sizeof(char));
	char ch;
	int i = 0;

	while ((ch = in_string[i]) != '\0') {
		switch(ch) {
			default:
				break;
			case '[': case ']': case '<': case '>': case '+': case '-': case '.': case ',':
				string[i++] = ch;
				break;
		}

		if (i >= buf_size) {
			buf_size *= 2;
			string = realloc(string, buf_size * sizeof(char));
		}
	}

	string[i] = '\0';

	return string;
} //}}}

/* Open a filestream, turn it into a string and then call onlyCode() */
char * onlyCodeF(FILE *instream) //{{{
{
	size_t buf_size = BF_BUF_INIT;
	char *string = (char *) calloc(buf_size, sizeof(char));
	char ch;
	int i = 0;
	
	while (( ch = fgetc(instream)) != EOF) {
		string[i++] = ch;
		if (i >= buf_size) {
			buf_size *= 2;
			string = realloc(string, buf_size * sizeof(char));
		}
	}

	string[i] = '\0';	
	char * remove_old = string;
	string = onlyCode(string);
	free(remove_old);

	return string;
} //}}}

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

	/* Strip the code to only BF characters first */
	char *inst_str = onlyCodeF(in_file);
	int i = 0;

	while (inst_str[i] != '\0') {
		if (flag.ncurses) {
			updateInstructions(inst_str, i);
			wrefresh(win.inst);
		}

		/* Loop logic is handled here, and if the character is not a loop char
		 * it gets sent to execChar() to be dealt with */
		switch(inst_str[i]) {
			case '[':
				/* push the index to stack */
				loop_stack[loop_depth] = i;
				/* and go into no_exec mode if cur tape cell is 0 */
				if (*tape.tc == 0 && exec) {
					no_exec_loop_depth = loop_depth;
					exec = false;
				}
				loop_depth++;
				break;
			case ']':
				--loop_depth;
				/* pop the top index of the stack if in exec_mode. The -1 is to
				 * counteract the increment after the switch statement so we
				 * run an entry check at the '[' for the cur cell */
				if (exec) {
					i = loop_stack[loop_depth] - 1;
				}
				/* if we are at the same depth where we started a no_exec
				 * loop then we go back to exec mode */
				else if (loop_depth == no_exec_loop_depth) {
					exec = true;
				}
				break;
			default:
				if (exec)
					execChar(inst_str[i]);
				break;
		}

		i++; // increment after switching case

		if (flag.ncurses) {
			updateTape();
			wrefresh(win.tape);
			refresh();
		}

		if (loop_depth > MAX_LOOP_DEPTH)
			return ERROR_LOOP_DEPTH_EXCEEDED;

		usleep(delay);
		refresh();
	}

	if (loop_depth)
		return ERROR_UNMATCHED_BRACES;

	return NO_ERROR;
} //}}}

/* input function to be called by execChar() */

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
