#ifdef __EMSCRIPTEN__
#include "emscripten/emscripten.h"
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif

#include <string.h>

#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include <iostream>
#include <memory>
#include <cassert>
#include <vector>
#include <map>


#include "pre.h"

//#include <boost/cstdint.hpp>
#include <boost/multi_array.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>



#include "col.hpp"
#include "env.h"
#include "csv.h"
#include "game.h"
#include "console.h"
#include "server/serialize.h"

#include "format.hpp"






using std::unique_ptr;

using std::string;
using std::add_pointer;
using std::to_string;

using std::cout;
using std::endl;

using std::pair;
using std::map;
using std::vector;






bool g_running = true;


col::Game game;

void loop_step(col::Game* game)
{
	g_running = game->step();	
	#ifdef __EMSCRIPTEN__
	if (!g_running) {
		print("emscripten_cancel_main_loop\n");
		emscripten_cancel_main_loop();
	}
	#endif
}




int main(int argc, char* argv[])
{
	
	#ifdef __EMSCRIPTEN__
		// set main loop and continue main
		emscripten_set_main_loop_arg((em_arg_callback_func)loop_step, &game, -1, 0);
	#endif
	
	
	if (argc == 2) {
		game.init(argv[1], 1);
	}
	else {
		game.init("", 1);
	}
		
	#ifndef __EMSCRIPTEN__
		while (g_running) {
			loop_step(&game);
		}		
	#endif
	
	print("main EXIT\n");
	
	return 0;
}

