#include <iostream>

#include "io.hh"
#include "interpreter.hh"

//The logic for our Print command
void print(std::string entry, bool nl) {
	bool found_qt = false;
	std::string result = "";
	
	if (entry[0]=='\"' && entry[entry.length()-1]=='\"') {
		found_qt = true;
	}
	
	for (int i = 0; i<entry.length(); i++) {
		if (entry[i]=='\"') {
			continue;
		} else {
			result+=entry[i];
		}
	}
	
	if (found_qt) {
		std::cout << result;
		if (nl) {
			std::cout << std::endl;
		}
	} else {
		auto vars = Interpreter::vars;
		for (int i = 0; i<vars.size(); i++) {
			Var v = vars.at(i);
			if (v.name==result) {
				if (v.value[0]=='\"' && v.value[v.value.length()-1]=='\"') {
					print(v.value,nl);
				} else {
					std::cout << v.value;
					if (nl) {
						std::cout << std::endl;
					}
				}
				break;
			}
		}
	}
}

//The logic for the input command
void input(std::string line) {
	std::string ln = "";
	std::getline(std::cin,ln);
		
	if (line.length()==0) {
		std::cout << ln << std::endl;
		return;
	}
	
	try {
		std::stoi(ln);
	} catch (std::invalid_argument) {
		ln="\""+ln+"\"";
	}
	
	auto vars = Interpreter::vars;
	for (int i = 0; i<vars.size(); i++) {
		if (vars.at(i).name==line) {
			Var v;
			v.name = line;
			v.value = ln;
			vars.push_back(v);
			vars.erase(vars.begin()+i);
		}
	}
	Interpreter::vars = vars;
}

//The logic for the color command
//Our colors are below
const std::string red("\033[0;31m");
const std::string green("\033[1;32m");
const std::string yellow("\033[1;33m");
const std::string cyan("\033[0;36m");
const std::string magenta("\033[0;35m");
const std::string reset("\033[0m");

void color(std::string line) {
	if (line=="red") {
		std::cout << red;
	} else if (line=="green") {
		std::cout << green;
	} else if (line=="yellow") {
		std::cout << yellow;
	} else if (line=="cyan") {
		std::cout << cyan;
	} else if (line=="magenta") {
		std::cout << magenta;
	} else if (line=="none") {
		std::cout << reset;
	} else {
		std::cout << "Error: Unknown color." << std::endl;
		std::cout << "Type \"ColorHelp\" for a list of options." << std::endl;
	}
}

void color_help() {
	std::cout << "Below are a list of available colors:" << std::endl;
	std::cout << red << "red" << std::endl
	<< green << "green" << std::endl
	<< yellow << "yellow" << std::endl
	<< cyan << "cyan" << std::endl
	<< magenta << "magenta" << std::endl
	<< reset << "none" << std::endl;
}
