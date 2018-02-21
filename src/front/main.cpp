#include "front.hpp"
#include <thread>
//#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/string_cast.hpp>


using namespace front;

Front win2;

void run()
{
	win2.create("Test", {320*3, 200*3});

	auto tex = front::make_texture(front::load_png_RGBA8("res/tile-48-free/texture1.png"));

	front::bind_texture(2, tex);

	for (size_t i = 0; i < 9; ++i)
	{
		/*{
			RenderCall rc;
			rc.win = &win2;
			rc.mode = MODE_FILL;
			rc.color = RGBA8(128,0,128,255);
			rc.dst = b2s({200, 100}, {100, 100});
			rc();
		}*/

		{		
			RenderCall rc;
			rc.win = &win2;
			rc.mode = MODE_AAMASK;
			rc.color = RGBA8(0,255,0,255);
			rc.texu = 2;
			rc.dst = b2s({0, 0}, {320*3, 200*3});
			rc.uv0 = aabb2f(0,0,1,1);
			rc();
		}
		
		win2.flip();
		SDL_Delay(1000);
	}
	
	win2.destroy();

}

int main()
{
	
	std::thread t(run);

	t.join();
	
	return 0;
}
