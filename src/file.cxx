#include <fstream>
#include <iostream>
#include <vector>

#include "file.hh"
#include "interpreter.hh"
#include "str.hh"
#include "vars.hh"

//Handles the file command
void file_command(std::string line) {
	std::string first = str_first(line);
	std::string second = str_second(line);
	
	if (first=="Read") {
		file_read(second);
	} else if (first=="Write") {
		file_write(second);
	} else {
		std::cout << "Error: Unknown command." << std::endl;
	}
}

//Reads a file to a list
void file_read(std::string line) {
	//First, break up the string
	TriStr tri = split_three(line);
	std::string file = tri.part1;
	std::string middle = tri.part2;
	std::string list = tri.part3;
	
	if (middle!="to") {
		std::cout << "Error: Unknown keyword." << std::endl;
		return;
	}
	
	file = get_var(file);
	
	//Read the file to a vector
	std::vector<std::string> contents;
	
	std::ifstream reader(file);
	if (reader.is_open()) {
		std::string line = "";
		
		while (std::getline(reader,line)) {
			contents.push_back(line);
		}
		
		reader.close();
	} else {
		std::cout << "Error: Unknown file." << std::endl;
	}
	
	//Find our list variable, and push the contents to it
	auto lists = Interpreter::lists;
	
	for (int i = 0; i<lists.size(); i++) {
		if (lists.at(i).name==list) {
			lists.at(i).contents = contents;
			break;
		}
	}
	
	Interpreter::lists = lists;
}

//Writes a list to a file
void file_write(std::string line) {
	//First, break up the string
	TriStr tri = split_three(line);
	std::string list = tri.part1;
	std::string middle = tri.part2;
	std::string file = tri.part3;
	
	if (middle!="to") {
		std::cout << "Error: Unknown keyword." << std::endl;
		return;
	}
	
	file = get_var(file);
	
	//Get the contents
	std::vector<std::string> contents;
	auto lists = Interpreter::lists;
	
	for (int i = 0; i<lists.size(); i++) {
		if (lists.at(i).name==list) {
			contents = lists.at(i).contents;
			break;
		}
	}
	
	//Now, write the file
	std::ofstream writer;
	writer.open(file);
	
	for (int i = 0; i<contents.size(); i++) {
		writer << rm_quotes(contents.at(i)) << std::endl;
	}
	
	writer.flush();
	writer.close();
}
