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
Func Interpreter::currentF;

void Interpreter::init() {
	last_ret.func = false;
	last_ret.continue_exe = true;
	last_ret.func_name = "";
}

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
		if (ret.func_name!="") {
			ret.func = true;
		}
	} else if (first=="End") {
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
				std::cout << second << std::endl;
			} else if (first=="Comment") {
				//We do nothing here
			} else if (first=="Clear") {
				std::cout << "\033[2J" << std::endl;
				std::cout << "\033[1;1H";
			} else {
				std::cout << "Error: Unknown command." << std::endl;
			}
		}
	}
	
	last_ret = ret;
	return ret;
}
