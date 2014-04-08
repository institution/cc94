#ifndef IOMM_H
#define IOMM_H

#include <cassert>
#include <fstream>
#include <stdexcept>
#include <cstdint>


namespace col{
	
	using std::ostream;
	using std::istream;
	using std::string;
	using std::size_t;
	
	using std::uint8_t;
	
	using uint8 = std::uint8_t;
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;
	using uint = std::uint32_t;
	
	using int8 = std::int8_t;
	using int16 = std::int16_t;
	using int32 = std::int32_t;
		
	
	namespace io{

		char read_byte(istream &f);
		size_t write_byte(ostream &f, const char &x);

		string read_chars(istream &f, uint n);
		size_t write_chars(ostream &f, const string &s);


		template <class T>
		size_t write(ostream &f, const T &t);

		template <class T>
		void read(T& t, istream& f);
		
		
		template <class T>
		T read(istream &f) {
			T t;
			read(t, f);
			return t;
		}
		
	}
	
}

#endif