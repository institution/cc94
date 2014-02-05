#include "csv.h"

#include <boost/spirit/include/support_istream_iterator.hpp>

namespace col{
	
	vector<vector<string>> read_conf(const string &fname)
	{
		namespace spirit = boost::spirit;
		namespace qi = spirit::qi;
		namespace ascii = spirit::ascii;
		using qi::char_;
		using qi::eol;
		using qi::lit;
		using std::string;
		using std::cout;
		using std::endl;
		using std::ifstream;
		using std::vector;

		ifstream in(fname);
		//in.unsetf(std::ios::skipws);

		string c{
			 std::istreambuf_iterator<char>(in),
			 std::istreambuf_iterator<char>()
		};

		using It = decltype(c.begin());

		qi::rule<It> ws;
		qi::rule<It> comment;
		qi::rule<It, string()> word;
		qi::rule<It, vector<string>()> line;
		qi::rule<It, vector<vector<string>>()> lines;

		ws = (*lit(' '));
		comment = lit(';') >> *char_("a-zA-Z0-9 ");
		word = *(char_(" a-zA-Z0-9.") | char_('-'));
		line = (word % lit(',')) >> (-comment);	
		lines  = (-line) % eol;

		vector<vector<string>> vss;

		bool r = qi::phrase_parse(c.begin(), c.end(),
			//((*~char_(',')) % ','),
			lines,
			//((ws >> data >> ws) % sep),
			qi::blank, 
			vss
		);
		
		return vss;
	}
	
	
}