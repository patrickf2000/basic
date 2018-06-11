// Copyright 2018 Patrick Flynn
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, 
//	this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this 
//	list of conditions and the following disclaimer in the documentation and/or 
//	other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may 
//	be used to endorse or promote products derived from this software 
//	without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#include <iostream>
#include <cstdlib>

#include "interpreter.hh"
#include "str.hh"
#include "loop.hh"
#include "io.hh"
#include "math.hh"

Ret Interpreter::last_ret;
std::vector<Func> Interpreter::functions;
std::vector<Var> Interpreter::vars;

std::vector<std::string> Interpreter::condition_bd;
bool Interpreter::in_condition;

std::vector<std::string> Interpreter::loop_bd;
int Interpreter::loop_count = 0;
bool Interpreter::in_loop = false;

std::vector<std::string> Interpreter::for_bd;
std::string Interpreter::for_cmd = "";
bool Interpreter::in_for = false;

Func Interpreter::currentF;
std::string Interpreter::mem = "";

void Interpreter::init() {
	last_ret.func = false;
	last_ret.continue_exe = true;
	last_ret.func_name = "";
}

//The main interpreter
Ret Interpreter::run(std::string line, bool ignore) {
	Ret ret = last_ret;
	
	std::string first = str_first(line);
	std::string second = str_second(line);
	
	if (first=="Function") {
		if (ret.func) {
			std::cout << "Error: You cannot nest functions." << std::endl;
			std::cout << "Use \"End\" to stop the current function." << std::endl;
		} else {
			currentF.name = second;
			ret.func_name = second;
		}
	} else if (first=="Begin") {
		//Used for functions
		if (ret.func_name!="") {
			ret.func = true;
		}
	} else if (first=="End") {
		//Used for functions
		if (ret.func) {
			ret.func = false;
			ret.func_name = "";
			functions.push_back(currentF);
			currentF.content.clear();
			currentF.name = "";
		}
	} else if (first=="Stop" && !ret.func) {
		//Conditionals
		if (in_condition) {
			in_condition = false;
			
			std::vector<Condition> conditions;
			Condition current;
			
			for (int i = 0; i<condition_bd.size(); i++) {
				std::string ln = condition_bd.at(i);
				
				std::string f = str_first(ln);
				if (f=="If") {
					current.cmp = str_second(ln);
				} else if (f=="Else") {
					conditions.push_back(current);
					current.body.clear();
					current.cmp = str_second(ln);
				} else {
					current.body.push_back(ln);
				}
			}
			conditions.push_back(current);
			current.body.clear();
			
			for (int i = 0; i<conditions.size(); i++) {
				if (eval_condition(conditions.at(i))) {
					break;
				}
			}
				
			conditions.clear();	
			condition_bd.clear();
		}
		
		//Loops
		if (in_loop) {
			in_loop = false;

			int index = 0;
			do {
				for (int i = 0; i<loop_bd.size(); i++) {
					run(loop_bd.at(i),true);
				}
				index++;
			} while (index<loop_count);
			
			loop_bd.clear();
			loop_count = 0;
		}
		
		//For loops
		if (in_for) {
			in_for = false;
			
			run_for_loop(for_cmd,for_bd);
			
			for_bd.clear();
			for_cmd = "";
		}
	} else {
		if (ret.func && !ignore) {
			currentF.content.push_back(line);
		} else if (in_condition) {
			condition_bd.push_back(line);
		} else if (in_loop) {
			loop_bd.push_back(line);
		} else if (in_for) {
			for_bd.push_back(line);
		} else {
			if (first=="Exit") {
				ret.continue_exe = false;
			} else if (first=="Call") {
				std::vector<std::string> content;
				bool found = false;
				
				for (int i = 0; i<functions.size(); i++) {
					if (functions.at(i).name==second) {
						content = functions.at(i).content;
						found = true;
						break;
					}
				}
				
				if (found) {
					for (int i = 0; i<content.size(); i++) {
						run(content.at(i),true);
					}
				} else {
					std::cout << "Error: You are asking for a non-existent function." << std::endl;
				}
			} else if (first=="Print") {
				print(second,false);
			} else if (first=="PrintLine") {
				print(second,true);
			} else if (first=="Input") {
				input(second);
			} else if (first=="Comment" || line.length()==0) {
				//We do nothing here
			} else if (first=="Clear") {
				std::cout << "\033[2J" << std::endl;
				std::cout << "\033[1;1H";
				
			//Defining a var works this way:
			// 1) Var v-> This creates the variable.
			// 2) Define v as 10-> Sets 10 as the value of v
			} else if (first=="Var") {
				Var v;
				v.name = second;
				vars.push_back(v);
			} else if (first=="Define") {
				def_var(second);
				
			//Check for math functions
			} else if (check_math(line)) {
				//We do nothing in the body
			
			//This sets the value of the memory to a variable
			} else if (first=="Set") {
				bool found = false;
			
				for (int i = 0; i<vars.size(); i++) {
					if (vars.at(i).name==second) {
						found = true;
						
						Var v;
						v.name = second;
						v.value = mem;
						vars.push_back(v);
						vars.erase(vars.begin()+i);
					}
				}
				
				if (found) {
					mem = "";
				} else {
					std::cout << "Error: Unknown variable." << std::endl;
					std::cout << "The memory is still retained." << std::endl;
				}
			
			//Sets the memory	
			} else if (first=="Memset") {
				mem = second;
				for (int i = 0; i<vars.size(); i++) {
					if (vars.at(i).name==second) {
						mem = vars.at(i).value;
						break;
					}
				}
				
			//These are our string functions
			} else if (first=="StrLen") {
				std::string val = second;
				
				for (int i = 0; i<vars.size(); i++) {
					if (vars.at(i).name==second) {
						val = vars.at(i).value;
						break;
					}
				}
				
				if (val[0]!='\"' || val[val.length()-1]!='\"') {
					std::cout << "Error: Invalid string." << std::endl;
				} else {
					int len = val.length()-2;
					mem = std::to_string(len);
				}
			} else if (first=="Char") {
				char_command(second);
				
			//Our conditionals
			} else if (first=="If") {
				in_condition = true;
				condition_bd.push_back(line);
			} else if (first=="Else") {
				if (in_condition) {
					condition_bd.push_back(line);
				} else {
					std::cout << "Error: You cannot have an Else statement without an opening If statement." << std::endl;
				}
				
			//Loops
			} else if (first=="Loop") {
				in_loop = true;
				
				try {
					loop_count = std::stoi(second);
				} catch (std::invalid_argument) {
					//See if we have a variable
					std::string n = second;
					for (int i = 0; i<vars.size(); i++) {
						if (vars.at(i).name==second) {
							 n = vars.at(i).value;
						}
					}
					
					//Try to convert it
					try {
						loop_count = std::stoi(n);
					} catch (std::invalid_argument) {
						std::cout << "Error: Loop count number must be an integer." << std::endl;
						in_loop = false;
					}
				}
			} else if (first=="For") {
				in_for = true;
				for_cmd = second;
			
			//End with the unknown command message	
			} else {
				std::cout << "Error: Unknown command." << std::endl;
			}
		}
	}
	
	last_ret = ret;
	return ret;
}

//The logic for our variable definitions
void Interpreter::def_var(std::string line) {
	TriStr strs = split_three(line);
	std::string name = strs.part1;
	std::string middle = strs.part2;
	std::string val = strs.part3;
	
	if (middle!="as") {
		std::cout << "Error: Unknown keyword." << std::endl;
		return;
	}
	
	bool fv = false;
	for (int i = 0; i<vars.size(); i++) {
		if (vars.at(i).name==name) {
			fv = true;
			
			Var v;
			v.name = name;
			v.value = val;
			vars.push_back(v);
			vars.erase(vars.begin()+i);
		}
	}
	
	if (!fv) {
		std::cout << "Error: Unknown variable. Please define it first." << std::endl;
	}
}

//The Char command
//The char command can do the following:
//Char <int> in <string>	This returns the character at the specified location
//Char <string> in <string>	This returns the location of a specified character
void Interpreter::char_command(std::string line) {
	//First, break up the string
	TriStr strs = split_three(line);
	std::string str1 = strs.part1;
	std::string middle = strs.part2;
	std::string str2 = strs.part3;
	
	if (middle!="in") {
		std::cout << "Error: Unknown keyword." << std::endl;
		return;
	}
	
	//If str1 is greater than 1 (length), then we might have a variable
	//If so, check
	if (str1.length()>1) {
		for (int i = 0; i<vars.size(); i++) {
			if (vars.at(i).name==str1) {
				str1 = vars.at(i).value;
				break;
			}
		}
	}
	
	//See if str2 is a variable
	for (int i = 0; i<vars.size(); i++) {
		if (vars.at(i).name==str2) {
			str2 = vars.at(i).value;
			break;
		}
	}
	
	//Now, we have to figure out what kind of operation we are doing
	try {
		int loco = std::stoi(str1);
		
		if (loco>str2.length()-1) {
			std::cout << "Error: The position exceeds the length of the string." << std::endl;
			return;
		}
		
		char c = str2[loco];
		std::string ln = "";
		ln+=c;
		mem=ln;
	} catch (std::invalid_argument) {
		int loco = -1;
		char c = str1[0];
		
		for (int i = 1; i<str2.length()-1; i++) {
			if (str2[i]==c) {
				loco = i;
				break;
			}
		}
		
		mem = std::to_string(loco);
	}
}

//The logic for evaluating conditionals
bool Interpreter::eval_condition(Condition c) {
	std::string cmp = c.cmp;
	
	//If cmp length is 0, then we are at a last-resort else
	//Simply execute the body, and return
	if (cmp.length()==0) {
		for (int i = 0; i<c.body.size(); i++) {
			run(c.body.at(i),true);
		}
		return true;
	}
	
	//First, break up the string
	TriStr strs = split_three(cmp);
	std::string str1 = strs.part1;
	std::string middle = strs.part2;
	std::string str2 = strs.part3;
	
	//See if either str1 or str2 is a variable
	for (int i = 0; i<vars.size(); i++) {
		if (vars.at(i).name==str1) {
			str1 = vars.at(i).value;
		}
		if (vars.at(i).name==str2) {
			str2 = vars.at(i).value;
		}
	}
	
	//Try to convert to numbers, then compare
	bool r = false;
	
	try {
		int no1 = std::stoi(str1);
		int no2 = std::stoi(str2);
		
		if (middle=="Greater") {
			if (no1>no2) {
				r = true;
			}
		} else if (middle=="Less") {
			if (no1<no2) {
				r = true;
			}
		} else if (middle=="Equals") {
			if (no1==no2) {
				r = true;
			}
		} else {
			std::cout << "Error: Invalid comparison." << std::endl;
		}
	} catch (std::invalid_argument) {
		if (middle=="Equals") {
			if (str1==str2) {
				r = true;
			}
		} else {
			std::cout << "Error: You cannot compare strings with Greater or Less." << std::endl;
			std::cout << "Or, you may have an invalid operator." << std::endl;
		}
	}
	
	if (!r) {
		return false;
	}
	
	//Run comparison body
	for (int i = 0; i<c.body.size(); i++) {
		run(c.body.at(i),true);
	}
	
	return true;
}
