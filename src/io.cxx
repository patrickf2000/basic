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
