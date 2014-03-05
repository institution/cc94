#define BOOST_TEST_MODULE ai
#include <boost/test/unit_test.hpp>

// Very Simple Game
struct XOGame{
	
	// 012
	// 345
	// 678
	char xs[9];
	char player;
	int moved;
	
	XOGame() {
		for (auto &x: xs) {
			x = '.';
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
	
	struct Action {
		virtual void exec(Game &game) = 0;
	};
	
	struct Move {
		int i;
		Move(int i_) {
			i = i_;
		}
		
	};
	
	struct EndTurn {
		int i;
		Action(int i_) {
			i = i_;
			assert(0 <= i);
			assert(i < 9);		
		}
	};
	
	
	void exec(Action *a) {
			
	}
	
	float get_result(char p) {
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


BOOST_AUTO_TEST_CASE( test1 )
{
	
	
	
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
}
