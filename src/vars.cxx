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

#include "vars.hh"
#include "interpreter.hh"
#include "str.hh"

//This resets variables to limit scope to only within functions.
//However, we must maintain any global variables. Here's how we do it:
//1) Create a backup copy of the array
//2) Run function,....
//3) At the end of the function, go through the main array
//		* If we find a variable that is also in the backup
//		array, than it is global. Pass it (and the new value)
//		to the array.
//		* NOTE: This action is called by the logic for the Call command.
//		If we do it after the End command, then we destroy everything too
//		early.
//4) Destroy the main array by clearing it and setting it equal to the
//		backup array
//
//Step 1 and 2 are handled by the main interpreter, since they are easy.
//Step 3 and 4 is handled by this function to keep things clean.
void reset_vars() {
	auto bk_vars = Interpreter::backup_vars;
	auto vars = Interpreter::vars;
	
	for (int i = 0; i<vars.size(); i++) {
		Var current = vars.at(i);
		
		for (int j = 0; j<bk_vars.size(); j++) {
			Var bk_current = bk_vars.at(j);
			
			if (current.name==bk_current.name) {
				bk_vars.at(j).value = current.value;
			}
		}
	}
	
	vars.clear();
	vars = bk_vars;
	
	Interpreter::backup_vars.clear();
	Interpreter::vars = vars;
}

//Creates new variables
void create_var(std::string line) {
	auto vars = Interpreter::vars;
	bool found = false;
		
	for (int i = 0; i<vars.size(); i++) {
		if (vars.at(i).name==line) {
			found = true;
			break;
		}
	}
			
	if (!found) {
		Var v;
		v.name = line;
		vars.push_back(v);
	}
	
	Interpreter::vars = vars;
}

//This gets the value of a variable name
//If no such variable exists, the original value is returned
std::string get_var(std::string var) {
	std::string ret = var;
	for (int i = 0; i<Interpreter::vars.size(); i++) {
		if (Interpreter::vars.at(i).name==var) {
			ret = Interpreter::vars.at(i).value;
		}
	}
	return ret;
}

//The logic for our variable definitions
void def_var(std::string line) {
	TriStr strs = split_three(line);
	std::string name = strs.part1;
	std::string middle = strs.part2;
	std::string val = strs.part3;
	
	if (middle!="as") {
		std::cout << "Error: Unknown keyword." << std::endl;
		return;
	}
	
	//See if val references a command
	//If so, parse
	if (val[0]=='(') {
		std::string cmd = "";
		for (int i = 0; i<val.size(); i++) {
			if (val[i]=='(' || val[i]==')') {
				continue;
			}
			cmd+=val[i];
		}
		Interpreter::run(cmd,true);
		val = Interpreter::mem;
	}
	
	bool fv = false;
	auto vars = Interpreter::vars;
	
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
	
	Interpreter::vars = vars;
	
	if (!fv) {
		std::cout << "Error: Unknown variable. Please define it first." << std::endl;
	}
}
