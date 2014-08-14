#ifndef OX2_H
#define OX2_H

#include <iostream>
#include <map>
#include <string>
#include <boost/format.hpp>

namespace ox{

	using std::cout;
	using std::endl;
	using std::map;
	using std::ostream;
	using std::string;
	using boost::format;

	struct OX;
	void dump(OX const& ox);
	ostream& operator<<(ostream &out, OX const& g);

			
	struct OX{

		struct InvalidMove {

		};

		using PlayerId = char;

		// 012
		// 345
		// 678
		char bs[9];
		char player;
		int free;
		map<char,float> score;
		bool end;
		int verbose;

		OX& load(string const& state, PlayerId const& current_player) {
			assert(state.size() == 9);
			
			for (int i=0; i<9; ++i) {
				char c = state[i];
				assert(c == '.' or c == 'O' or c == 'X');
				bs[i] = c;
			}
			
			assert(current_player == 'O' or current_player == 'X');
			player = current_player;
			
			return *this;
		}
		
		OX(int verbosee = 0) {
			for (auto &b: bs) {
				b = '.';
			}
			player = 'O';
			free = 9;

			score['O'] = 0;
			score['X'] = 0;

			end = false;
			verbose = verbosee;
		}


		char get(int x, int y) {
			return bs[x + y*3];
		}

		bool bound(int x, int y) {
			return 0 <= x and x < 3 and 0 <= y and y < 3;
		}

		void move(PlayerId pid, int i);

		bool check_pos(int x, int y) {
			if (1 + check_dir(x,y,0,-1) + check_dir(x,y,0,1) >= 3) return 1;
			if (1 + check_dir(x,y,-1,0) + check_dir(x,y,1,0) >= 3) return 1;
			if (1 + check_dir(x,y,-1,-1) + check_dir(x,y,1,1) >= 3) return 1;
			if (1 + check_dir(x,y,1,-1) + check_dir(x,y,-1,1) >= 3) return 1;
			return 0;
		}

		int check_dir(int x, int y, int dx, int dy) {
			char pl = get(x,y);
			
			int i = x + dx;
			int j = y + dy;
			int l = 0;

			while (bound(i,j) and (get(i, j) == pl)) {
				++l;

				i += dx;
				j += dy;
			}
			
			return l;
		}


		float get_result(PlayerId p) {
			if (verbose > 1) 
				cout << format("OS::get_result('%||') -> %|| \n") % p % score.at(p);

			return score.at(p);
		}


	};

	

	


}

#endif
