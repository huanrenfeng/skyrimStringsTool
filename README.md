# skyrimStringsTool


Usage:

    skyrimStringsTool -f modname.strings
    skyrimStringsTool -f modname.strings -swap



-f filename
     read a strings file and generate a totrans.txt. You edit totrans.txt and generate a transed.txt by yourself( you can copy the totrans.txt to transed.txt and edit the file) for the next step. 
     
     contents of the generated file is like this:
     
     1 string1
     3 string2
     23 string3
     
     the number is the id for the string. Note that the id doesn't need to be sequential

-swap
     read a strings file and a transed.txt and swap the cooresponding entries appeared in the transed.txt in strings file.
     
-fix
     fix protential bug in the ilstrings/dlstrings file.

