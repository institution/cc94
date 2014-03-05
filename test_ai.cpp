#define BOOST_TEST_MODULE ai
#include <boost/test/unit_test.hpp>
#include <iostream>
#include "ai.h"
#include "roll.h"

using namespace std;

struct XOGame{
	
	using PlayerId = char;
	
	// 012
	// 345
	// 678
	char bs[9];
	char player;
	int moves;
	
	XOGame() {
		for (auto &b: bs) {
			b = '.';
		}
		player = 'O';
		moves = 1;
	}
	
	void end_turn() {
		assert(moves = 0);
		
		if (player == 'O') {
			player = 'X';
		}
		else {
			player = 'O';
		}
		moves = 1;
	}
		
	void move(int i) {
		assert(moves > 0);
		assert(0 <= i);
		assert(i < 9);
		
		bs[i] = player;
		--moves;
	}
	
	float get_result(PlayerId p) {
		
		// -
		if (bs[0] == bs[1] == bs[2] == p) return 1;
		if (bs[3] == bs[4] == bs[5] == p) return 1;
		if (bs[6] == bs[7] == bs[8] == p) return 1;
		// |
		if (bs[0] == bs[3] == bs[6] == p) return 1;
		if (bs[1] == bs[4] == bs[7] == p) return 1;
		if (bs[2] == bs[5] == bs[8] == p) return 1;
		// X
		if (bs[0] == bs[4] == bs[8] == p) return 1;
		if (bs[2] == bs[4] == bs[6] == p) return 1;
		
		return 0;
	}
	
	
	
};

struct XOGameWrapper {
	
	using PlayerId = typename XOGame::PlayerId;
	
	XOGame &game;
	
	XOGameWrapper(XOGame &g): game(g) {
		
	}
	
	struct Action {
		virtual void exec(XOGame &game) = 0;
	};
	
	struct Move: Action {
		int i;
		Move(int i_) {
			i = i_;
		}
		
		void exec(XOGame &game) {
			game.move(i);
		}
	};
	
	struct EndTurn: Action {
		void exec(XOGame &game) {
			game.end_turn();
		}
	};	
	
	void exec(Action *a) {
		if (a == nullptr) {
			EndTurn().exec(this->game);
		}
		else {
			a->exec(this->game);
		}
	}
	
	float get_result(XOGame::PlayerId p) {
		return game.get_result(p);		
	}
	
	Action *create_random_action() {
		if (game.moves) {
			return new Move(roll::roll(0,9));
		}
		else {
			return nullptr; // endturn
		}
	}
	
};




ostream& operator<<(ostream &out, XOGame const& g) {
	out << g.bs[0] << g.bs[1] << g.bs[2] << "\n"
	    << g.bs[3] << g.bs[4] << g.bs[5] << "\n"
	    << g.bs[6] << g.bs[7] << g.bs[8] << "\n";
}


BOOST_AUTO_TEST_CASE( test_base )
{
	
	XOGame game;
	
	XOGameWrapper gw(game);
	col::MCTS<XOGameWrapper> mt(gw, 'O');
	
	mt.think();
	
	
	/*
    // seven ways to detect and report the same error:
    BOOST_CHECK( add( 2,2 ) == 4 );        // #1 continues on error

    BOOST_REQUIRE( add( 2,2 ) == 4 );      // #2 throws on error

    if( add( 2,2 ) != 4 )
      BOOST_ERROR( "Ouch..." );            // #3 continues on error

    if( add( 2,2 ) != 4 )
      BOOST_FAIL( "Ouch..." );             // #4 throws on error

    if( add( 2,2 ) != 4 ) throw "Ouch..."; // #5 throws on error

    BOOST_CHECK_MESSAGE( add( 2,2 ) == 4,  // #6 continues on error
                         "add(..) result: " << add( 2,2 ) );

    BOOST_CHECK_EQUAL( add( 2,2 ), 4 );	  // #7 continues on error
	*/
}
