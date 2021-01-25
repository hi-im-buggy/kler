# Kler
Kler is a visual brainfuck interpreter written in C.

**TODO:** Planning
- [ ] fix loop handling
- [ ] exec multiple lines at once
- [ ] interactive mode should handle incomplete loops gracefully (prompt for more input till loop is closed properly)
- [ ] visualize program counter and tape counter

**TODO:** Visualization
1. have execChar() func, no case in it for '[' & ']'
2. handle looping in execString(), 
3. modify execString() to behave differently 
if file vs nofile
