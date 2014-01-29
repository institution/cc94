#include "iomm.h"

namespace iomm{
		
	char read_byte(ifstream& f) {
		char c = f.get();
		if (!f.good()) {
			throw std::runtime_error(string("error while reading from stream"));
		}
		return c;	
	}
	
	uint8 read_uint8(ifstream& f) {
		return read_byte(f);
	}
	
	uint16 read_uint16(ifstream& f) {
		uint16 a = read_byte(f);
		uint16 b = read_byte(f);
		return (b << 8) | a;
	}
	
	uint32 read_uint32(ifstream& f) {
		uint32 a = read_byte(f);
		uint32 b = read_byte(f);
		uint32 c = read_byte(f);
		uint32 d = read_byte(f);		
		return (d << 24) | (c << 16) | (b << 8) | a;
	}
	

	string read_ascii(ifstream& f, uint32 n) {
		string s;
		s.reserve(n);
		uint32 i = 0;
		while (i < n) {
			char c = read_byte(f);
			assert(0 <= c && c <= 127);  // ascii range
			s.push_back(c);
			++i;
		}
		return s;
	}
	
	void write_byte(ofstream &f, char x) {
		f.put(x);		
	}

	void write_uint8(ofstream& f, uint8 x) {
		write_byte(f, x);	
	}
	
	void write_uint16(ofstream& f, uint16 x) {
		write_byte(f, x & 0xff);
		write_byte(f, x >> 8);		
	}

	void write_uint32(ofstream& f, uint32 x) {
		write_byte(f, (x >> 0) & 0xff);
		write_byte(f, (x >> 8) & 0xff);
		write_byte(f, (x >> 16) & 0xff);
		write_byte(f, (x >> 24) & 0xff);		
	}

	void write_ascii(ofstream& f, string s) {
		uint32 i = 0;
		while (i < s.size()) {
			write_byte(f, s[i]);
			++i;
		}
	}
}