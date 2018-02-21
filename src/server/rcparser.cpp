#include <fstream>
#include "rcparser.hpp"

namespace col {
	

	void RCParser::accept_string()
	{
		if (not is_quote_str(r.peek())) { 
			set_error("expected double quote; c='%||'", r.peek());
			r.get(); // ignore this char
			return;
		}
		
		r.get();
		while (not eof())
		{
			// print("char %|| %||\n", char(r.peek()), is_quote_str(r.peek()));
			if (is_escape_char(r.peek()))
			{
				r.get();
				switch (r.peek())
				{
					case '"': w.put('"'); break;
					case 'n': w.put('\n'); break;
					case 't': w.put('\t'); break;
					case 'r': w.put('\r'); break;
					case '\\': w.put('\\'); break;
					default:
						set_error("invalid escaped character; c='%||'", r.peek());
						// ignore this char
						break;
				}
				r.get();
			}
			else if (is_quote_str(r.peek()))
			{
				r.get();
				break;
			}
			else if (is_body_str(r.peek()))
			{
				w.put(r.get());
			}			
			else {
				set_error("unexpected char inside string; c='%||'", r.peek());
				// ignore this char
				r.get();
				break;
			}
		}
		skip_whites();
	}


	
		
	void RCParser::accept_symbol()
	{
		if (not is_open_sym(r.peek())) { 			
			set_error("expected open symbol character; c='%||'", r.peek());
			r.get(); // ignore
			return;
		}
		
		w.put(r.get());
		while (not eof())
		{		
			if (is_body_sym(r.peek()))
			{
				w.put(r.get());
			}
			else {			
				break;
			}
		}

		skip_whites();
	}


	void RCParser::skip_whites()
	{
		while (not eof()) {
			if (is_white(r.peek())) {
				r.get();
			}
			else if (is_endline(r.peek())) {
				line_no += 1;
				line_beg = r.tellg();
				r.get();
			}
			else if (is_open_comment(r.peek())) {
				accept_comment();
			}
			else {
				break;
			}
		}
	}

	void RCParser::accept_comment()
	{
		if (not is_open_comment(r.peek())) { 			
			set_error("expected open comment; c='%||'", r.peek());
			r.get(); // ignore
			return;
		}
		r.get();
		
		while (not eof()) 
		{
			if (is_endline(r.peek())) {
				line_no += 1;
				line_beg = r.tellg();
				break;
			}
			else {				
				r.get();
			}
		}
		r.get();	
	}

	void RCParser::open(Path const& path)
	{
		this->path = path;
		
		std::string buf;
		{
			std::ifstream f(path);
			f.seekg(0, std::ios::end);
			buf.resize(f.tellg());
			f.seekg(0, std::ios::beg);
			f.read(&buf[0], buf.size());
			f.close();
		}

		
		new (&w) Writer();
		new (&r) Reader();

		error.clear();
		tmp = "";
		r.str(buf);
		line_no = 0;
		line_beg = 0;
		skip_whites();
		mark = r.tellg();
		w.str("");
	}

	

	int RCParser::to_int(std::string const& s)
	{
		if (s == "-") {
			return 0;
		}
		
		try {			
			return stoi(s);
		}
		catch (std::invalid_argument const&) {
			set_error("expected number; s=\"%||\"", s);			
		}
		catch (std::out_of_range const&) {
			set_error("number too large");
		}
		return 0;
	}
		

	


	uint32_t RCParser::read_uint32()
	{
		mark = r.tellg();
		w.str("");
		accept_symbol();
		return to_int(w.str());		
	}

	int32_t RCParser::read_int32()
	{
		mark = r.tellg();
		w.str("");
		accept_symbol();
		return to_int(w.str());		
	}

	uint16_t RCParser::read_uint16()
	{
		mark = r.tellg();
		w.str("");
		accept_symbol();
		return to_int(w.str());		
	}
	
	int16_t RCParser::read_int16()
	{
		mark = r.tellg();
		w.str("");
		accept_symbol();
		return to_int(w.str());		
	}
	
	uint8_t RCParser::read_uint8()
	{
		mark = r.tellg();
		w.str("");
		accept_symbol();
		return to_int(w.str());		
	}
	
	int8_t RCParser::read_int8()
	{
		mark = r.tellg();
		w.str("");
		accept_symbol();
		return to_int(w.str());		
	}
	

	
	std::string const& RCParser::read_string()
	{
		mark = r.tellg();
		w.str("");
		accept_string();
		tmp = w.str();
		return tmp;
	}
	
	std::string const& RCParser::read_symbol()
	{
		mark = r.tellg();
		w.str("");
		accept_symbol();
		tmp = w.str();
		return tmp;
	}
	





}
