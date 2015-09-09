#include "gui.h"
#include "renderer.h"

namespace col{
	
	void Gui::init() {
		
		if (verbose >= 1) print("GUILoop: app.init\n");
		app.init(
			"cc94",
			v2i(conf.screen_w, conf.screen_h)
		);
		
		if (verbose >= 1) print("GUILoop: preload renderer\n");
		preload_renderer(app);
		

		if (verbose) print("GUILoop: set_logical_dim\n");
		app.set_logical_dim(v2i(conf.screen_w, conf.screen_h));
		
		
		last_mod_env = env.mod - 1;
		last_mod_con = con.mod - 1;

		last_tick = app.get_ticks();
		if (verbose >= 1) print("GUILoop: init; tick=%||\n", last_tick);
	}
	
	bool Gui::step() {
		
		if (verbose >= 2) print("GUILoop.step: {; app.done=%||\n", app.done);
		
		if ((env.mod != last_mod_env) || (con.mod != last_mod_con) || (last_tick + 100 > app.get_ticks())) {
			//cout << "RENDER:" << con.mod << ',' << env.mod << endl;
			con.time = app.get_ticks();
			last_tick = con.time;

			if (verbose >= 2) print("GUILoop.step: render; tick=%||\n", last_tick);
			
			render(app, env, con, verbose);

			last_mod_env = con.env.mod;
			last_mod_con = con.mod;
		}

		if (verbose >= 2) print("GUILoop.step: handle_events;\n");

		con.handle_events(app);
					
		if (verbose >= 2) print("GUILoop.step: }\n");
					
		return !app.done;		
		
	}
	
	
	
}
