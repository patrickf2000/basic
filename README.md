## BASIC

This is a simple BASIC interpreter. It is written entirely in C++. This interpreter was not written to be compatible with other dialects of BASIC; however, because it is very similar in many places, and has BASIC properties, it is considered a BASIC interpreter.   

This program is still in its early stages, so it has its share of limitations. Additionally, several concepts, such as variables, are different from that of other languages. For more information, run 'make doc' to generate a pdf document of the language. There are also several examples in the examples/ directory.   

#### Building
Because BASIC is written solely using the C++ standard library, there is no reason for a build system. Therefore, simply run Make on the included makefile. The binary is called 'basic'. If you run the binary without any arguments, you will end up in shell mode. See the documentation for more information on running it.

#### Installing
Run "make install" as root.

#### Platform Support
BASIC is developed on and has only been tested on Linux. However, it should build and work without any problems on other platforms (especially UNIX-like platforms).

#### License
The program is licensed under the BSD-3 license. See the COPYING file for more information.
