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
	
	/*try {*/
		
		g_running = runner->step();	
		#ifdef __EMSCRIPTEN__
		if (!g_running) {
			print("emscripten_cancel_main_loop\n");
			emscripten_cancel_main_loop();
		}
		#endif
    
	/*} catch(std::exception const& e) {
		std::cerr << "main.cpp:loop_step: exception: " << e.what();
		g_running = false;
	} catch(...) {
		std::cerr << "main.cpp:loop_step: unknown exception!\n";
		g_running = false;
	}*/
	
}


int main(int argc, char* argv[])
{
	
	#ifdef __EMSCRIPTEN__
		// set main loop and continue main
		emscripten_set_main_loop_arg((em_arg_callback_func)loop_step, &runner, -1, 0);
	#endif
	
	
	if (argc == 2) {
		runner.init(argv[1], 1);
	}
	else {
		runner.init("", 1);
	}
		
	#ifndef __EMSCRIPTEN__
		while (g_running) {
			loop_step(&runner);
		}		
	#endif
	
	print("main EXIT\n");
	
	return 0;
}

