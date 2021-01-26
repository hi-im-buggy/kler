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
	tape.tc = tape.start;

	// Initialize things if in ncurses mode
	if (flag.ncurses) {
		initscr();
		cbreak();
		noecho();
		curs_set(0);

		win.inst = newwin(5, 40, 0, 0);
		win.tape = newwin(30, 40, 7, 0);

		box(win.inst, 0, 0);
		box(win.tape, 0, 0);
		wrefresh(win.inst);
		wrefresh(win.tape);
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
		endwin();
	}
	return 0;
}
