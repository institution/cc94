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
#include "runner.h"
#include "console.h"
#include "server/serialize.h"

#include "format.hpp"



bool g_running = true;


col::Runner runner;

void loop_step(col::Runner* runner)
{
	g_running = runner->step();	
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
		emscripten_set_main_loop_arg((em_arg_callback_func)loop_step, &runner, -1, 0);
	#endif
	
	
	if (argc == 2) {
		runner.init(argv[1], "tile64", 1);
	}
	if (argc == 3) {
		runner.init(argv[1], argv[2], 1);
	}
	else {
		runner.init("", "tile64", 1);
	}
		
	#ifndef __EMSCRIPTEN__
		while (g_running) {
			loop_step(&runner);
		}		
	#endif
	
	print("main EXIT\n");
	
	return 0;
}

