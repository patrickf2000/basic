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

Ret Interpreter::last_ret;
std::vector<Func> Interpreter::functions;
std::vector<Var> Interpreter::vars;
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
	} else {
		if (ret.func && !ignore) {
			currentF.content.push_back(line);
		} else {
			if (first=="Exit") {
				std::exit(0);
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
				print(second);
			} else if (first=="Comment") {
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
				
			//These are the math functions
			} else if (first=="Add") {
				math('+',second);
			} else if (first=="Sub") {
				math('-',second);
			} else if (first=="Mp") {
				math('*',second);
			} else if (first=="Div") {
				math('/',second);
			} else if (first=="Remainder") {
				math('%',second);
			
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
			
			//End with the unknown command message	
			} else {
				std::cout << "Error: Unknown command." << std::endl;
			}
		}
	}
	
	last_ret = ret;
	return ret;
}

//The logic for our Print command
void Interpreter::print(std::string entry) {
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
		std::cout << result << std::endl;
	} else {
		for (int i = 0; i<vars.size(); i++) {
			Var v = vars.at(i);
			if (v.name==result) {
				std::cout << v.value << std::endl;
				break;
			}
		}
	}
}

//The logic for our variable definitions
void Interpreter::def_var(std::string line) {
	std::string name = "";
	std::string middle = "";
	std::string val = "";
	bool fs1 = false;
	bool fs2 = false;
	
	for (int i = 0; i<line.length(); i++) {
		if (line[i]==' ') {
			if (!fs1) {
				fs1 = true;
			} else {
				fs2 = true;
			}
		} else {
			if (fs1 && fs2) {
				val+=line[i];
			} else if (fs1 && !fs2) {
				middle+=line[i];
			} else {
				name+=line[i];
			}
		}
	}
	
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

//The logic for our math solver
void Interpreter::math(char op, std::string line) {
	//First, break up the string
	std::string str1 = "";
	std::string str2 = "";
	std::string middle = "";
	bool fs1 = false;
	bool fs2 = false;
	
	for (int i = 0; i<line.length(); i++) {
		if (line[i]==' ') {
			if (!fs1) {
				fs1 = true;
			} else {
				fs2 = true;
			}
		} else {
			if (fs1 && fs2) {
				str2+=line[i];
			} else if (fs1 && !fs2) {
				middle+=line[i];
			} else {
				str1+=line[i];
			}
		}
	}
	
	if (middle!="and") {
		std::cout << "Error: Unknown keyword." << std::endl;
		return;
	}
	
	//Now, see if either of the parts are variables
	for (int i = 0; i<vars.size(); i++) {
		Var v = vars.at(i);
		if (v.name==str1) {
			str1 = v.value;
		}
		if (v.name==str2) {
			str2 = v.value;
		}
	}
	
	//Convert and add
	try {
		int no1 = std::stoi(str1);
		int no2 = std::stoi(str2);
		
		int sum = 0;
		if (op=='+') {
			sum = no1+no2;
		} else if (op=='-') {
			sum = no1-no2;
		} else if (op=='*') {
			sum = no1*no2;
		} else if (op=='/') {
			sum = no1/no2;
		} else if (op=='%') {
			sum = no1%no2;
		}
		mem = std::to_string(sum);
	} catch (std::invalid_argument) {
		if (op=='+') {
			std::string n = str1+str2;
			mem = n;
		} else {
			std::cout << "Error: Invalid arguments." << std::endl;
		}		
	}
}
