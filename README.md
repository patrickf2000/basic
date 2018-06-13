## BASIC

This is a simple BASIC interpreter. It is written entirely in C++. This interpreter was not written to be compatible with other dialects of BASIC; however, because it is very similar in many places, and has BASIC properties, it is considered a BASIC interpreter.   

This program is still in its early stages, so it has its share of limitations. Additionally, several concepts, such as variables, are different from that of other languages. For more information, see the files in the doc/ directory. If you are interested in this language, I encourage you to read them in order. There are also several examples in the examples/ directory.   

#### Building
BASIC has no dependencies other than the C++ standard library. To build, simply run CMake (no arguments, unless there are some you want), followed by make. The binary is called 'basic'. If you run the binary without any arguments, you will end up in shell mode. See the documentation for more information on running it.

#### Installing
Installation is not supported at this time. However, if you want it on your system, just copy the binary to /usr/local/bin or somewhere like that.

#### Platform Support
BASIC is developed on and has only been tested on Linux. However, it should build and work without any problems on other platforms (especially UNIX-like platforms).

#### License
The program is licensed under the BSD-3 license. See the COPYING file for more information.
