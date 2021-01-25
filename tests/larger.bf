>>>>,>,<           take in two numbers
[-<<<<<+>>>>+>]    make two copies as n1|_|n2|n2|n1
>
[-<<<<+>+>>>]
<<<<<<             back to cell 1

[
   ->>-            subtract from n1 and n2
   [<]             if n2 is 0 (n2 leq n1), stay; if it's positive (n2 gt n1), go left
]

>>.                two cells to the right, max is stored
