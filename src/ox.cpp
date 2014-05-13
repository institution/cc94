#include "ox.h"

namespace ox{

	ostream& operator<<(ostream &out, OX const& g) {
		out << g.bs[0] << g.bs[1] << g.bs[2] << "\n"
			<< g.bs[3] << g.bs[4] << g.bs[5] << "   player: '" << g.player << "'\n"
			<< g.bs[6] << g.bs[7] << g.bs[8] << "\n";
		return out;
	}

	void dump(OX const& ox) {
		cout << ox;
	}

	void OX::move(OX::PlayerId pid, int i) {
		int x = i%3;
		int y = i/3;

		if (verbose)
			cout << format("OX::move(%|| to %||,%||)\n") % pid % x % y;

		assert(pid == player);
		assert(0 <= i);
		assert(i < 9);
		assert(bs[i] == '.');
		assert(!end);

		bs[i] = player;
		--free;




		auto r = check_pos(x, y);
		if (r) {
			score[player] = 1;
			end = true;
			if (verbose) {
				dump(*this);
				cout << format("OX And the winner is: '%||'\n") % player;
			}

		}
		else		
		if (free == 0) {
			score['O'] = 0.5;
			score['X'] = 0.5;
			end = true;

			if (verbose) {
				dump(*this);
				cout << format("OX And the winner is: TIE\n");
			}
		}

		if (player == 'O') {
			player = 'X';
		}
		else {
			player = 'O';
		}


	}

}