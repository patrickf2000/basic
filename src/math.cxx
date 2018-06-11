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

#include "math.hh"
#include "str.hh"
#include "interpreter.hh"

bool check_math(std::string line) {
	std::string first = str_first(line);
	std::string second = str_second(line);

	if (first=="Add") {
		math('+',second);
	} else if (first=="Sub") {
		math('-',second);
	} else if (first=="Mp") {
		math('*',second);
	} else if (first=="Div") {
		math('/',second);
	} else if (first=="Remainder") {
		math('%',second);
	} else {
		return false;
	}
	return true;
}

//The logic for our math solver
void math(char op, std::string line) {
	//First, break up the string
	TriStr strs = split_three(line);
	std::string str1 = strs.part1;
	std::string middle = strs.part2;
	std::string str2 = strs.part3;
	
	if (middle!="and") {
		std::cout << "Error: Unknown keyword." << std::endl;
		return;
	}
	
	//Now, see if either of the parts are variables
	str1 = Interpreter::get_var(str1);
	str2 = Interpreter::get_var(str2);
	
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
		Interpreter::mem = std::to_string(sum);
	} catch (std::invalid_argument) {
		if (op=='+') {
			if (str1[0]!='\"' || str1[str1.length()-1]!='\"' || 
				str2[0]!='\"' || str2[str2.length()-1]!='\"') {
				std::cout << "Error: You can only add numbers or strings." << std::endl;
				return;
			}
			
			std::string final_s = "";
			
			for (int i = 0; i<str1.length()-1; i++) {
				final_s+=str1[i];
			}
			
			for (int i = 1; i<str2.length(); i++) {
				final_s+=str2[i];
			}
			
			Interpreter::mem = final_s;
		} else {
			std::cout << "Error: Invalid arguments." << std::endl;
		}		
	}
}
