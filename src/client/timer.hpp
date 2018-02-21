#pragma once

#include <thread>
#include "../front/front.hpp"

namespace col
{
	struct Timer
	{
		front::Tick t0;
		front::Tick delay;		
		
		Timer(front::Tick delay): delay(delay) {
			t0 = front::get_ticks();
		}


		front::Tick left() {
			return delay - (front::get_ticks() - t0);
		}
		
		void wait()
		{
			auto l = left();
			if (l > 0) {
				std::this_thread::sleep_for(std::chrono::milliseconds(l));	
			}
			t0 = front::get_ticks();
		}

	};

}
