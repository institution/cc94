#include "input.hpp"

namespace front {
	
	


	void print_window_event(Event const& event)
	{
		if (event.type == SDL_WINDOWEVENT) {
			switch (event.window.event) {
			case SDL_WINDOWEVENT_SHOWN:
				print("Window %|| shown\n", event.window.windowID);
				break;
			case SDL_WINDOWEVENT_HIDDEN:
				print("Window %|| hidden\n", event.window.windowID);
				break;
			case SDL_WINDOWEVENT_EXPOSED:
				print("Window %|| exposed\n", event.window.windowID);
				break;
			case SDL_WINDOWEVENT_MOVED:
				print("Window %|| moved to %||,%||\n",
						event.window.windowID, event.window.data1,
						event.window.data2);
				break;
			case SDL_WINDOWEVENT_RESIZED:
				print("Window %|| resized to %||x%||\n",
						event.window.windowID, event.window.data1,
						event.window.data2);
				break;
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				print("Window %|| size changed to %||x%||\n",
						event.window.windowID, event.window.data1,
						event.window.data2);
				break;
			case SDL_WINDOWEVENT_MINIMIZED:
				print("Window %|| minimized\n", event.window.windowID);
				break;
			case SDL_WINDOWEVENT_MAXIMIZED:
				print("Window %|| maximized\n", event.window.windowID);
				break;
			case SDL_WINDOWEVENT_RESTORED:
				print("Window %|| restored\n", event.window.windowID);
				break;
			case SDL_WINDOWEVENT_ENTER:
				print("Mouse entered window %||\n",
						event.window.windowID);
				break;
			case SDL_WINDOWEVENT_LEAVE:
				print("Mouse left window %||\n", event.window.windowID);
				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				print("Window %|| gained keyboard focus\n",
						event.window.windowID);
				break;
			case SDL_WINDOWEVENT_FOCUS_LOST:
				print("Window %|| lost keyboard focus\n",
						event.window.windowID);
				break;
			case SDL_WINDOWEVENT_CLOSE:
				print("Window %|| closed\n", event.window.windowID);
				break;
			default:
				print("Window %|| got unknown event %||\n",
						event.window.windowID, event.window.event);
				break;
			}
		}
	}

}
