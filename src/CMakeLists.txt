cmake_minimum_required(VERSION 3.0.0)
project(basic)

set(SRC
	main.cxx
	interpreter.cxx
	str.cxx
	loop.cxx
	io.cxx
	math.cxx
	vars.cxx
	list.cxx
	file.cxx
)

add_executable(basic ${SRC})
