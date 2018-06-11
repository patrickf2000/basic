## BASIC

This is a simple BASIC interpreter. Its still in its early stages, so its fairly limited. You can use this program from the shell or via the command line with a source file. Below is a list of commands. A language definition will come later

#### Commands
Print-> Print to the console (no new line)   
PrintLine-> Print to the console (with new line)   
Exit-> Exit the program   
Function (name)-> Create a function   
Begin-> Begin a function body   
End-> End a function Body   
Clear-> Clear the console screen   
Comment-> Source file comment   
Var-> Define a variable (Syntax: Var v, no equals sign)   
Define-> Set a variable (Syntax: Define v as 10, for example)   
Add-> Add two numbers (Syntax: Add 5 and 6); can also be used for strings   
Sub-> Subtract two numbers (Same as add)   
Mp-> Multiply two numbers   
Div-> Divide two numbers   
Remainder-> Get the remainder of two numbers   
Memset-> Set the local memory   
Set-> Set a variable with the contents of local memory   
StrLen-> Get the length of a string   
Char-> Get the character at a specific location, or the location of a specific character   
Kill-> Clear a particular variable   
Destroy-> Clear all variables and the local memory   
Abs-> Get the absolute value of a number   
Random-> Generate a random number   
Color-> Set the text color   
ColorHelp-> Get a list of available text colors   
Input-> Get user keyboard input   

#### Conditionals
Conditionals have the following syntax:   

If (var) (Operator) (var)   
... Body   
Else (var) (operator) (var)   
... Body   
Else   
... Body   
Stop   

Below is a list of valid operators:   
1. Greater   
2. Less   
3. Equals   

#### Loops
This is (and will be) the simplest type of loop. All you do is use the 'Loop' command, followed by a number (or variable representing a number). Any commands you type after that will be part of the loop body until you type 'Stop'. Below is an example that prints "Hello!" five times:   

Loop 5   
Print "Hello!"   
Stop   

#### For loops
For loops are very similar to the Loop command, except that they allow you to set and access a variable containing the current index. Basically, you set a variable, followed by the keyword 'to', and end with a loop counter. The loop counter may be a variable or a hard-coded example. Below is an example that will print all numbers from 0 to 9.   

Var i   
Define i as 0   
For i to 10   
PrintLine i   
Stop   
