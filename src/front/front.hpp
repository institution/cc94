#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "base.hpp"
#include "glm.hpp"
#include "color.hpp"
#include "input.hpp"
#include "image.hpp"
#include "my.hpp"

namespace front {

	using namespace front::Color;
	
	using TexUnit = GLint;

	
	struct Texture 
	{
		GLuint id{0};
		v2s dim;
		GLenum format{0};

		void create();
		void destroy();

		Texture() = default;
		Texture(Texture const& o) = delete;		
		Texture(Texture && o):
			id(o.id),
			dim(o.dim),
			format(o.format)
		{			
			o.id = 0;
		}		
		void operator=(Texture && o) {
			assert(id == 0);
			id = o.id;
			dim = o.dim;
			format = o.format;
			o.id = 0;
		}
		~Texture() {
			if (id != 0) {
				destroy();
			}
		}

		//v2s dim() const { return dim; }
	};
	
	

	Texture make_texture(GLenum format, uint8_t const* data, v2s dim);
	Texture make_texture(ImageRGBA8 const& img);
	Texture make_texture(ImageR8 const& img);

	
	struct Front
	{		
		// window & context
		SDL_Window * win{nullptr};
		SDL_GLContext ctx;

		v2s win_dim{0,0};  // physical dim
		v2s ctx_dim{0,0};  // logical dim

		// opengl stuff
		GLuint vao[1];
		GLuint vbo[1];
		GLuint prog[1];

		glm::mat4 proj;
		
		// misc
		bool verbose{false};
		bool done{false};


		Front() = default;
		Front(Front const&) = delete;

		void create_SDL(std::string const& title, v2s dim);
		void destroy_SDL();

		void create_GL();
		void destroy_GL();
		
		void create(std::string const& title, v2s dim)
		{
			create_SDL(title, dim);
			create_GL();
		}

		void destroy() {
			if (win) {				
				destroy_GL();
				destroy_SDL();
			}
		}

		void resize_view(v2s win_dim, v2s ctx_dim);
		
		v2s get_real_win_dim() const;

		void stop() {
			done = true;
		}

		void clear() {
			glClear(GL_COLOR_BUFFER_BIT);
			CHECK_GL();
		}

		void flip() {
			SDL_GL_SwapWindow(win);
		}

	};


	int const
		MODE_DEBUG = 0,
		MODE_TEXTURE = 1,
		MODE_AAMASK = 2,
		MODE_FILL = 3,
		MODE_REPLACE = 4;

	struct RenderCall
	{
		Front * win{nullptr};
		int mode;
		TexUnit texu{0};
		RGBA8 color;
		b2s dst;
		aabb2f uv0;
		aabb2f uv1;

		void operator()();
	};

	inline void bind_texture(TexUnit texu, Texture const& tex) {
		myBindTexture(texu, tex.id);
	}


		
	


	inline Tick get_ticks() {
		#ifdef __EMSCRIPTEN__
			return emscripten_get_now();
		#else
			return SDL_GetTicks();
		#endif
	}

	




	// --
	inline uint8_t const* get_keyboard_state();



}


