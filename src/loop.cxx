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

#include "loop.hh"
#include "str.hh"
#include "interpreter.hh"

void run_for_loop(std::string cmd, std::vector<std::string> body) {
	//First, split the string
	TriStr tri = split_three(cmd);
	std::string var = tri.part1;
	std::string middle = tri.part2;
	std::string counter = tri.part3;
	
	if (middle!="to") {
		std::cout << "Error: Unknown keyword." << std::endl;
		return;
	}
	
	//Now, see if the counter references a variable
	auto vars = Interpreter::vars;
	
	for (int i = 0; i<vars.size(); i++) {
		if (vars.at(i).name==counter) {
			counter = vars.at(i).value;
		}
	}
	
	//Convert the counter variable to a number
	int count = 0;
	
	try {
		count = std::stoi(counter);
	} catch (std::invalid_argument) {
		std::cout << "Error: You must use integers as counters in for loops." << std::endl;
		return;
	}
	
	//Now, loop.
	//On each iteration, we must update our counter variable
	//After that, run the body
	int index = 0;
	while (index<count) {
		for (int i = 0; i<vars.size(); i++) {
			if (vars.at(i).name==var) {
				Var v;
				v.name = vars.at(i).name;
				v.value = std::to_string(index);
				vars.push_back(v);
				vars.erase(vars.begin()+i);
			}
		}
		Interpreter::vars = vars;
		
		for (int i = 0; i<body.size(); i++) {
			Interpreter::run(body.at(i),true);
		}
		
		index++;
	}
}
