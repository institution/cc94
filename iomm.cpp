#include "iomm.h"

namespace iomm{
		
	char read_byte(istream& f) {
		char c = f.get();
		if (!f.good()) {
			throw std::runtime_error(string("error while reading from stream"));
		}
		return c;	
	}
	
	uint8 read_uint8(istream& f) {
		return read_byte(f);
	}
	
	uint16 read_uint16(istream& f) {
		uint16 a = read_byte(f);
		uint16 b = read_byte(f);
		return (b << 8) | a;
	}
	
	uint32 read_uint32(istream& f) {
		uint32 a = read_byte(f);
		uint32 b = read_byte(f);
		uint32 c = read_byte(f);
		uint32 d = read_byte(f);		
		return (d << 24) | (c << 16) | (b << 8) | a;
	}
	
	string read_chars(istream &f, uint n) {
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
	
	string read_string(istream& f) {
		uint32 n = read_uint32(f);
		return read_chars(f, n);		
	}
	
	size_t write_byte(ostream &f, const char &x) {
		f.put(x);
		return 1;
	}

	size_t write_uint8(ostream& f, const uint8 &x) {
		write_byte(f, x);
		return 1;
	}
	
	size_t write_uint16(ostream& f, const uint16 &x) {
		write_byte(f, x & 0xff);
		write_byte(f, x >> 8);
		return 2;		
	}

	size_t write_uint32(ostream& f, const uint32 &x) {
		write_byte(f, (x >> 0) & 0xff);
		write_byte(f, (x >> 8) & 0xff);
		write_byte(f, (x >> 16) & 0xff);
		write_byte(f, (x >> 24) & 0xff);
		return 4;
	}
	
	size_t write_string(ostream& f, const string &s) {
		size_t l = 0;
		l += write_uint32(f, s.size());
		l += write_chars(f, s);
		return l;		
	}
	
	size_t write_chars(ostream &f, const string &s) {
		size_t l = 0;
		for (auto c: s) {
			l += write_byte(f, c);
		}
		return l;
	}
}