#pragma once

#include "../ext/ext.hpp"
#include <vector>
#include <sstream>
#include "cerror.hpp"

using ext::fail;

namespace col {

	inline bool is_quote_str(char c)     { return c == '"';}
	inline bool is_escape_char(char c)   { return c == '\\';}
	inline bool is_body_str(char c)      { return char(32) <= c and c <= char(127) and c != '\\' and c != '"';}
	inline bool is_open_sym(char c)      { return char(33) <= c and c <= char(127);}
	inline bool is_body_sym(char c)      { return char(33) <= c and c <= char(127);}
	inline bool is_open_comment(char c)  { return c == '#'; }
	inline bool is_white(char c)         { return c == '\t' or c == ' '; }
	inline bool is_endline(char c)       { return c == '\n'; }
	inline bool is_eof(char c)           { return c == EOF; }





	using Reader = std::stringstream;
	using Writer = std::stringstream;
		
	struct RCParser
	{
		Writer w;
		Reader r;
		std::string tmp;
		size_t mark{0};
		Path path;
		int64_t line_no{0};
		int64_t line_beg{0};
		CError error;
		
		void accept_string();
		void accept_symbol();
		void accept_comment();
		void skip_whites();

		bool eof() const {
			return r.eof();
		}

		bool ok() const {
			return not eof() and not error.has();
		}

		void end()
		{
			if (error.has()) {
				ext::fail(error.get().c_str());
			}
		}

		void close()
		{
			r.str("");
			end();
		}

		template <class... Args>
		void set_error(char const* fmt, Args... args)
		{
			error.set("ERROR %||:%||:%||: %||\n", path, line_no + 1, int64_t(r.tellg()) - line_beg + 1, format(fmt, args...));
		}

		int to_int(std::string const& s);
		

		void open(Path const& s);

		uint32_t read_uint32();
		int32_t read_int32();

		uint16_t read_uint16();
		int16_t read_int16();

		uint8_t read_uint8();
		int8_t read_int8();

			
		std::string const& read_string();
		std::string const& read_symbol();

		void unread() { r.seekg(mark); }
		
		
		RCParser() = default;
		RCParser(Path const& path)
		{
			open(path);
		}
		
		~RCParser() {
			close();
		}

	};

	inline void read(RCParser & p, uint32_t & x) { x = p.read_uint32(); }
	inline void read(RCParser & p, int32_t & x) { x = p.read_int32(); }
	inline void read(RCParser & p, uint16_t & x) { x = p.read_uint16(); }
	inline void read(RCParser & p, int16_t & x) { x = p.read_int16(); }
	inline void read(RCParser & p, uint8_t & x) { x = p.read_uint8(); }
	inline void read(RCParser & p, int8_t & x) { x = p.read_int8(); }
	
	inline void read(RCParser & p, std::string & x) { x = p.read_string(); }

}
