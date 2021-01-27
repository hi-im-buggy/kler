# Kler
Kler is a visual Brainfuck interpreter written in C.

It will run a BF program and show you the current state of the tape reel, and what instruction is being executed.

# Installation

Clone the repository and build using the given Makefile.
```
git clone https://github.com/hi-im-buggy/kler
cd kler
make
```
`bin/kler` provides the binary then.
If `~/.local/bin` is in your path, you could also run `make install` instead to copy the binary into your path.
You can then run the binary with just `kler`

## Usage

```
	Usage: %s [infile] [options]
	Options:
	-h : print this help text
	-n : turn off ncurses mode
	-o outfile : output to file
	-d delay : set the delay between instructions in microseconds for ncurses mode
```

## **TODO:**
- [X] visualize program counter and tape counter
- [X] fix loop handling
- [X] implement io window
- [ ] ensure tape window output is safe to print 1 byte
- [ ] implement writing to outfile
- [ ] fix io window border redraw before first input/output
- [ ] implement interactive mode, handle incomplete loops gracefully (prompt for loop closure)
