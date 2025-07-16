   canvas .c
   pack .c
   bind .c <ButtonPress-1> {%W scan mark   %x %y}
   bind .c <B1-Motion>     {%W scan dragto %x %y 1}
