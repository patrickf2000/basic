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
#include "str.hh"

std::string str_first(std::string line) {
	std::string ret = "";
	bool fs = false;
	
	for (int i = 0; i<line.length(); i++) {
		if (line[i]==' ') {
			fs = true;
		} else {
			if (!fs) {
				ret+=line[i];
			}
		}
	}
	
	return ret;
}

std::string str_second(std::string line) {
	std::string ret = "";
	bool fs = false;
	
	for (int i = 0; i<line.length(); i++) {
		if (line[i]==' ') {
			if (fs) {
				ret+=line[i];
			} else {
				fs = true;
			}
		} else {
			if (fs) {
				ret+=line[i];
			}
		}
	}
	
	return ret;
}

std::string trim(std::string line) {
	std::string ret = "";
	bool found = false;
	
	for (int i = 0; i<line.length(); i++) {
		if (line[i]==' ' || line[i]=='\t') {
			if (found) {
				ret+=line[i];
			}
		} else {
			found = true;
			ret+=line[i];
		}
	}
	
	return ret;
}

std::string rm_quotes(std::string line) {
	if (line[0]!='\"' || line[line.length()-1]!='\"') {
		return line;
	}
	
	std::string ret = "";
	
	for (int i = 1; i<line.length()-1; i++) {
		ret+=line[i];
	}
	
	return ret;
}

TriStr split_three(std::string line) {
	TriStr ret;
	
	std::string str1 = "";
	std::string str2 = "";
	std::string middle = "";
	bool fs1 = false;
	bool fs2 = false;
	
	for (int i = 0; i<line.length(); i++) {
		if (line[i]==' ') {
			if (fs1 && fs2) {
				str2+=line[i];
			}
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
	
	ret.part1 = str1;
	ret.part2 = middle;
	ret.part3 = str2;
	
	return ret;
}
