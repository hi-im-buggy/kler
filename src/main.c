/* Kler is a visual brainfuck interpreter 
 * Author: Pratyaksh Gautam */

#include "kler.h"
#include <getopt.h>

flag_t flag;
tape_t tape;
win_grp_t win;
int delay = DELAY;

int main(int argc, char * const argv[])
{
	// init flags for defaults
	flag.ncurses = true;
	flag.output_file = false;
	char *in_file = NULL, *out_file = NULL;
	FILE *instream, *outstream;

	// Parse commandline args
	int opt;
	while ((opt = getopt(argc, argv, "hno:")) != -1) {
		switch (opt) {
			case 'n':
				flag.ncurses = false;	
				break;
			case 'o':
				flag.output_file = true;
				out_file = optarg;
				break;
			case 'h': default:
				fprintf(stderr,
						"\nUsage: %s [infile] [options]\n\n"
						"Options:\n"
						"-h : print this help text\n"
						"-n : turn off ncurses mode\n"
						"-o outfile : output to file\n", argv[0]);
				return 0;
		}
	}

	if (optind > argc && argc != 1) {
		fprintf(stderr, "Expected argument after options\n");
		return 0;
	}

	// initalize the instream
	in_file = argv[optind];
	instream = (in_file == NULL)? stdin: fopen(in_file, "r");
	outstream = (out_file == NULL)? stdout: fopen(out_file, "w");

	// Set up the tape before we continue and initialize it
	tape.size = INIT_TAPE_SIZE;
	tape.start = (cell_t *) calloc(tape.size, sizeof(cell_t));
	tape.tc = (cell_t *) tape.start;

	// Initialize things if in ncurses mode
	if (flag.ncurses) {
		initscr();
		cbreak();
		noecho();
		curs_set(0);

		int height, width;
		getmaxyx(stdscr, height, width);
		win.inst = newwin(INST_WIN_HEIGHT, width, 0, 0);
		win.tape = newwin(height - INST_WIN_HEIGHT, width / 2, INST_WIN_HEIGHT, 0);
		win.io = newwin(height - INST_WIN_HEIGHT, width / 2, INST_WIN_HEIGHT, width / 2);

		/* The initialization for only win.io is also done here,
		 * other wins are initd when called */
		wmove(win.io, PADDING, PADDING);
		box(win.io, 0, 0);
		wrefresh(win.io);
		refresh();
	}

	int retval = 0;
	retval = execFile(instream);

	switch (retval) {
		case ERROR_UNMATCHED_BRACES:
			fprintf(stderr, "error unmatched braces\n");
			break;
		case ERROR_LOOP_DEPTH_EXCEEDED:
			fprintf(stderr, "error loop depth exceeded\n");
			break;
	}

	if (flag.ncurses) {
		sleep(2);
		getch();
		endwin();
	}
	return 0;
}
