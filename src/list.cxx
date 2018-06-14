#include <iostream>

#include "list.hh"
#include "str.hh"
#include "interpreter.hh"
#include "vars.hh"

//All we do here is see what kind of list operation we wish
//to do. However, if the user simply wants to create a new
//list, we will do that here.
void list_command(std::string line) {
	std::string first = str_first(line);
	std::string second = str_second(line);
	
	if (first=="Add") {
		list_add(second);
	} else if (first=="Length") {
		list_length(second);
	} else if (first=="Show") {
		list_show(second);
	} else if (first=="Remove") {
		list_rm(second);
	} else if (first=="Clear") {
		list_cls(second);
	} else if (first=="Kill") {
		list_kill(second);
	} else {
		//Unknown command-> Assume new list
		List l;
		l.name = first;
		
		bool found = false;
		auto list = Interpreter::lists;
		
		for (int i = 0; i<list.size(); i++) {
			if (list.at(i).name==second) {
				found = true;
				break;
			}
		}
		
		if (!found) {
			list.push_back(l);
		}
		
		Interpreter::lists = list;	
	}
}

//Add an item to a list
void list_add(std::string line) {
	//First, split the string
	TriStr parts = split_three(line);
	std::string add = parts.part1;
	std::string middle = parts.part2;
	std::string list = parts.part3;
	
	if (middle!="to") {
		std::cout << "Error: Unknown keyword." << std::endl;
		return;
	}
	
	//See if we have any variables
	add = get_var(add);
	
	//Add the item to the list
	auto lists = Interpreter::lists;
	
	for (int i = 0; i<lists.size(); i++) {
		if (lists.at(i).name==list) {
			lists.at(i).contents.push_back(add);
		}
	}
	
	Interpreter::lists = lists;
}

//Get the length of a list
void list_length(std::string line) {
	std::string key = str_first(line);
	std::string name = str_second(line);
	
	if (key!="of") {
		std::cout << "Error: Unknown keyword." << std::endl;
		return;
	}
	
	Interpreter::mem = "0";
	
	for (int i = 0; i<Interpreter::lists.size(); i++) {
		List current = Interpreter::lists.at(i);
		if (current.name==name) {
			int len = current.contents.size();
			Interpreter::mem = std::to_string(len);
		}
	}
}

//Get a particular item in a list
void list_show(std::string line) {
	//First, split the string
	TriStr parts = split_three(line);
	std::string no = parts.part1;
	std::string middle = parts.part2;
	std::string list = parts.part3;
	
	if (middle!="in") {
		std::cout << "Error: Unknown keyword." << std::endl;
		return;
	}
	
	//Convert to number
	no = get_var(no);
	int index = 0;
	
	try {
		index = std::stoi(no);
	} catch (std::invalid_argument) {
		std::cout << "Error: You must past an integer argument as a list index." << std::endl;
		return;
	}
	
	//Add the item to the list
	auto lists = Interpreter::lists;
	
	for (int i = 0; i<lists.size(); i++) {
		List current = lists.at(i);
		if (current.name==list) {
			if (index>current.contents.size()) {	//Make sure we do not cause an out-of-bounds exception
				std::cout << "Error: You have exceeded the bounds of this list." << std::endl;
				return;
			}
			
			//Get the item
			std::string item = current.contents.at(index);
			Interpreter::mem = item;
			break;
		}
	}	
}

//Removes a particular item from a list
void list_rm(std::string line) {
	//First, split the string
	TriStr parts = split_three(line);
	std::string no = parts.part1;
	std::string middle = parts.part2;
	std::string list = parts.part3;
	
	if (middle!="in") {
		std::cout << "Error: Unknown keyword." << std::endl;
		return;
	}
	
	//Convert to number
	no = get_var(no);
	int index = 0;
	
	try {
		index = std::stoi(no);
	} catch (std::invalid_argument) {
		std::cout << "Error: You must past an integer argument as a list index." << std::endl;
		return;
	}

	//Make sure we are not exceeding the bounds
	auto lists = Interpreter::lists;
	
	for (int i = 0; i<lists.size(); i++) {
		List current = lists.at(i);
		if (current.name==list) {
			//Make sure we do not cause an out-of-bounds exception
			if (index>current.contents.size()) {
				std::cout << "Error: You have exceeded the bounds of this list." << std::endl;
				return;
			}
			
			//Remove the item
			lists.at(i).contents.erase(lists.at(i).contents.begin()+index);
			break;
		}
	}
	
	Interpreter::lists = lists;
}

//Clears a list completely
void list_cls(std::string line) {
	auto lists = Interpreter::lists;
	
	for (int i = 0; i<lists.size(); i++) {
		if (lists.at(i).name==line) {
			lists.at(i).contents.clear();
		}
	}
	
	Interpreter::lists = lists;
}

//Removes an item from the list
void list_kill(std::string line) {
	auto lists = Interpreter::lists;
	
	for (int i = 0; i<lists.size(); i++) {
		if (lists.at(i).name==line) {
			lists.erase(lists.begin()+i);
		}
	}
	
	Interpreter::lists = lists;
}
