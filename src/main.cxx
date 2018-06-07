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
#include <string>
#include <cstdlib>
#include <fstream>

#include "interpreter.hh"
#include "str.hh"

int main(int argc, char **argv) {
	Interpreter::init();
	
	if (argc>1) {
		for (int i = 1; i<argc; i++) {
			std::string current = std::string(argv[i]);
			
			//For speed, we will dynamically read and execute
			std::ifstream reader(current);
			if (reader.is_open()) {
				std::string line = "";
				
				while (std::getline(reader,line)) {
					std::string input = trim(line);
					if (input.length()==0) {
						continue;
					}
				
					Ret r = Interpreter::run(input,false);
					
					if (r.continue_exe==false) {
						break;
					}
				}
				
				reader.close();
			} else {
				std::cout << "Fatal error: Unknown file." << std::endl;
				std::exit(1);
			}
		}
	} else {
		std::cout << "BASIC 1.0" << std::endl;
		std::cout << "Welcome to the BASIC shell." << std::endl << std::endl;
	
		std::string func = "";
	
		while (true) {
			std::cout << "BASIC> "+func;
			
			std::string line = "";
			std::getline(std::cin,line);
			
			Ret r = Interpreter::run(line,false);
			if (r.func==true) {
				func = "["+r.func_name+"] ";
			} else {
				func = "";
			}
			
			if (r.continue_exe==false) {
				break;
			}
		}
	}
	
	return 0;
}
