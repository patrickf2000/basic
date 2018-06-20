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
#include <fstream>

#include "interpreter.hh"
#include "str.hh"
#include "loop.hh"
#include "io.hh"
#include "math.hh"
#include "vars.hh"
#include "list.hh"
#include "file.hh"

Ret Interpreter::last_ret;
std::vector<std::string> Interpreter::include_paths;
std::vector<Func> Interpreter::functions;
std::vector<Var> Interpreter::vars;
std::vector<Var> Interpreter::backup_vars;
std::vector<List> Interpreter::lists;

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
	last_ret.prog = "BASIC";
	last_ret.func = false;
	last_ret.continue_exe = true;
	last_ret.func_name = "";
	
	//Loads the Include path vector
	//All paths are hard-coded right now; eventually,
	//they will be loaded from a config file.
	include_paths.push_back("");
#ifdef _WIN32
	include_paths.push_back("C:\\basic\\stdlib\\");
#else
	include_paths.push_back("/usr/include/basic/");
	include_paths.push_back("/usr/local/include/basic/");
	include_paths.push_back("/opt/basic/include/");
#endif
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
			currentF.name = get_name(second);
			ret.func_name = get_name(second);
			
			currentF.params = get_params(second);
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
			
			std::vector<std::string> bd_tmp;
			
			bool found = false;
			for (int i = 0; i<conditions.size(); i++) {
				if (eval_condition(conditions.at(i))) {
					bd_tmp = conditions.at(i).body;
					found = true;
					break;
				}
			}
				
			conditions.clear();	
			condition_bd.clear();
			
			//If found, run body
			//We do it like this to prevent conditional conflicts
			if (found) {
				for (int i = 0; i<bd_tmp.size(); i++) {
					run(bd_tmp.at(i),true);
				}
			}
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
	} else if (first=="Include") {
		bool f = false;
		
		for (int i = 0; i<include_paths.size(); i++) {
			f = include_file(second,include_paths.at(i));
			if (f) {
				break;
			}
		}
		
		if (!f) {
			std::cout << "Error: Unable to include non-existent file." << std::endl;
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
				call_func(second);
			} else if (first=="Print") {
				print(second,false);
			} else if (first=="PrintLine") {
				print(second,true);
			} else if (first=="Input") {
				input(second);
			} else if (first=="Color") {
				color(second);
			} else if (first=="ColorHelp") {
				color_help();
			} else if (first=="Comment" || line.length()==0 || line[0]=='#') {
				//We do nothing here
			} else if (first=="Clear") {
#ifdef _WIN32
				system("cls");
#else
				std::cout << "\033[2J" << std::endl;
				std::cout << "\033[1;1H";
#endif
				
			//Defining a var works this way:
			// 1) Var v-> This creates the variable.
			// 2) Define v as 10-> Sets 10 as the value of v
			} else if (first=="Var") {
				create_var(second);
			} else if (first=="Define") {
				def_var(second);
				
			//This destroys a particular variable
			} else if (first=="Kill") {
				for (int i = 0; i<vars.size(); i++) {
					if (vars.at(i).name==second) {
						vars.erase(vars.begin()+i);
					}
				}
				
			//List controls
			//Unlike other commands, all List-related commands are arguments
			//to the List command. This helps us from getting too many
			//secondary commands.
			} else if (first=="List") {
				list_command(second);
				
			//This kills all variables and clears the memory
			} else if (first=="Destroy") {
				if (second=="vars") {
					vars.clear();
				} else if (second=="lists") {
					lists.clear();
				} else if (second=="mem") {
					mem = "";
				} else if (second=="all") {
					vars.clear();
					lists.clear();
					mem = "";
				} else {
					std::cout << "Error: Unknown input." << std::endl;
					std::cout << "Destroy options:" << std::endl
					<< "\tvars\n\tlists\n\tmem\n\tall" << std::endl;
				}
				
			//Check for math functions
			} else if (check_math(line)) {
				//We do nothing in the body
			} else if (first=="Random") {
				try {
					second = get_var(second);
					int no = std::stoi(second);
					int rand = std::rand() % no+1;
					mem = std::to_string(rand);
				} catch (std::invalid_argument) {
					std::cout << "Error: Argument must be an integer." << std::endl;
				}
			
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
				mem = get_var(second);
				
			//These are our string functions
			} else if (first=="StrLen") {
				std::string val = get_var(second);
				
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
				
			//Sets the program name that appears in the shell
			} else if (first=="Program") {
				if (second=="none") {
					ret.prog = "";
				} else if (second=="default") {
					ret.prog = "BASIC";
				} else {
					ret.prog = second;
				}
				
			//Displays author information
			} else if (first=="Author") {
				std::cout << "BASIC 1.0" << std::endl
				<< "Written by Patrick Flynn" << std::endl << std::endl
				<< "This program is a simple, easy BASIC interpreter." << std::endl
				<< "You can use it via source files or from the command line."
				<< std::endl << std::endl
				<< "This program is licensed under the BSD-3 license." << std::endl
				<< "See the included license file (COPYING) for more information." << std::endl
				<< "If you do not have the license file, please follow the link below:" << std::endl
				<< "<https://github.com/patrickf2000/basic/blob/master/COPYING>" << std::endl;
				
			} else if (first=="Version") {
				std::cout << "BASIC" << std::endl
				<< "You are using version 1.0." << std::endl
				<< "Type \"Author\" for more information." << std::endl;
				
			//File operations
			//Most file logic is handeled by other functions
			} else if (first=="File") {
				file_command(second);
			
			//End with the unknown command message	
			} else {
				std::cout << "Error: Unknown command." << std::endl;
			}
		}
	}
	
	last_ret = ret;
	return ret;
}

//Get the name from a function line with params
std::string Interpreter::get_name(std::string line) {
	std::string ret = "";
	
	for (int i = 0; i<line.length(); i++) {
		if (line[i]=='[') {
			break;
		}
		ret+=line[i];
	}
	
	return ret;
}

//Get function parameters
std::vector<std::string> Interpreter::get_params(std::string line) {
	std::vector<std::string> params;
	
	std::string ln = "";
	
	//First, get everything inside the brackets
	for (int i = 0; i<line.length(); i++) {
		if (line[i]=='[') {
			for (int j = i+1; j<line.length(); j++) {
				if (line[j]==']') {
					break;
				}
				ln+=line[j];
			}
			break;
		}
	}
	
	//Remove any spaces
	std::string tmp = "";
	bool qt = false;		//We do not want to remove spaces if we are inside a quote
	
	for (int i = 0; i<ln.length(); i++) {
		if (ln[i]=='\"') {
			if (qt) {
				qt = false;
			} else {
				qt = true;
			}
		} else if (ln[i]==' ' && !qt) {
			continue;
		}
		tmp+=ln[i];
	}
	ln = tmp;
	
	//Now, separate the string into parameters
	std::string current = "";
	for (int i = 0; i<ln.length(); i++) {
		if (ln[i]==',') {
			params.push_back(current);
			current = "";
		} else {
			current+=ln[i];
		}
	}
	params.push_back(current);
	
	return params;
}

//The logic for calling functions
void Interpreter::call_func(std::string line) {
	std::vector<std::string> content;
	std::vector<std::string> func_params;
	std::vector<std::string> params = get_params(line);
	bool found = false;
	
	std::string name = get_name(line);
				
	for (int i = 0; i<functions.size(); i++) {
		if (functions.at(i).name==name) {
			content = functions.at(i).content;
			func_params = functions.at(i).params;
			found = true;
			break;
		}
	}
				
	if (found) {
		//Backup the array
		backup_vars = vars;
		
		//Check to make sure we have enough parameters.
		if (func_params.size()!=params.size()) {
			std::cout << "Error: Wrong number of function parameters." << std::endl;
			std::cout << "Required number: " << func_params.size() << std::endl;
			std::cout << "Your number: " << params.size() << std::endl;
			return;
		}
		
		//Pass in the parameters
		for (int i = 0; i<params.size(); i++) {
			std::string func_current = func_params.at(i);
			std::string current = params.at(i);
			
			Var v;
			v.name = func_current;
			v.value = get_var(current);
			vars.push_back(v);
		}
		
		//Run the content
		for (int i = 0; i<content.size(); i++) {
			run(content.at(i),true);
		}
			
		reset_vars();
	} else {
		std::cout << "Error: You are asking for a non-existent function." << std::endl;
	}
}

//The logic for including files (used by the Include command)
bool Interpreter::include_file(std::string line, std::string prefix) {
	std::string path = prefix+line;
	std::ifstream reader(path);
	if (reader.is_open()) {
		std::string line = "";
		while (std::getline(reader,line)) {
			run(trim(line),false);
		}
		reader.close();
	} else {
		return false;
	}
	return true;
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
//We cannot run the body here, because if we do
//so, we could cause a lock if a call is made to a
//function with conditionals. As a result, the main
//interpreter function will run the body.
bool Interpreter::eval_condition(Condition c) {
	std::string cmp = c.cmp;
	
	//If cmp length is 0, then we are at a last-resort else
	//Simply execute the body, and return
	if (cmp.length()==0) {
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
	
	return true;
}
