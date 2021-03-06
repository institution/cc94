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

//#include <boost/cstdint.hpp>
//#include <boost/multi_array.hpp>
//#include <boost/format.hpp>
//#include <boost/algorithm/string.hpp>

#include "col.hpp"
#include "env.hpp"
#include "runner.hpp"
#include "console.hpp"
#include "server/serialize.hpp"
#include "../ext/format.hpp"


/*
void loop_step(col::Runner * runner)
{
	
	gf_stop = runner->f_stop;	
	#ifdef __EMSCRIPTEN__
	if (gf_stop) {
		print("emscripten_cancel_main_loop\n");
		emscripten_cancel_main_loop();
	}
	#endif

	
}
*/

col::Runner runner;


int main(int argc, char* argv[])
{
	
	//#ifdef __EMSCRIPTEN__
	//	// set main loop and continue main
	//	emscripten_set_main_loop_arg((em_arg_callback_func)loop_step, &runner, -1, 0);
	//#endif
	
	if (argc == 2) {
		runner.run(argv[1], 1);
	}
	else {
		runner.run("res/48free/main.rc", 1);
	}
		
	//#ifndef __EMSCRIPTEN__
	//	while (not gf_stop) {
	//		loop_step(&runner);
	//	}		
	//#endif
	
	print("main EXIT\n");	
	return 0;
}

