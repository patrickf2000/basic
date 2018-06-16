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
#pragma once

#include <string>
#include <vector>

struct Ret {
	std::string prog;
	bool func;
	std::string func_name;
	bool continue_exe;
};

struct Func {
	std::string name;
	std::vector<std::string> content;
};

struct Var {
	std::string name;
	std::string value;
};

struct List {
	std::string name;
	std::vector<std::string> contents;
};

struct Condition {
	std::string cmp;
	std::vector<std::string> body;
};

class Interpreter {
public:
	static void init();
	static Ret run(std::string line, bool ignore);
	static std::string get_name(std::string line);
	static void call_func(std::string line);
	static bool include_file(std::string line, std::string prefix);
	static void char_command(std::string line);
	static bool eval_condition(Condition c);
	
	//Public variables other functions may need
	static std::vector<Var> vars;
	static std::vector<Var> backup_vars;
	static std::vector<List> lists;
	static std::string mem;
private:
	static Ret last_ret;
	//General vectors
	static std::vector<std::string> include_paths;
	static std::vector<Func> functions;
	
	//Stuff for conditions
	static std::vector<std::string> condition_bd;
	static bool in_condition;
	
	//Stuff for simple loops
	static std::vector<std::string> loop_bd;
	static int loop_count;
	static bool in_loop;
	
	//Stuff for For loops
	static std::vector<std::string> for_bd;
	static std::string for_cmd;
	static bool in_for;
	
	//Other stuff
	static Func currentF;
};
