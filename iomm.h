#ifndef TEXT_H
#define TEXT_H

#include <cassert>
#include <fstream>
#include <stdexcept>
#include "uint.h"



namespace iomm{
	
	using std::ifstream;
	using std::ofstream;
	using std::string;


	char read_byte(ifstream& f);
	uint8 read_uint8(ifstream& f);
	uint16 read_uint16(ifstream& f);
	uint32 read_uint32(ifstream& f);
	string read_ascii(ifstream& f, uint32 n);	
	
	void write_byte(ofstream &f, char x);
	void write_uint8(ofstream &f, uint8 x);	
	void write_uint16(ofstream &f, uint16 x);
	void write_uint32(ofstream &f, uint32 x);
	void write_ascii(ofstream &f, string s);
}

#endif