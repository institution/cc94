#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "base.hpp"
#include "glm.hpp"
#include "color.hpp"
#include "input.hpp"
#include "image.hpp"

namespace front {

	using namespace front::Color;
	
	
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

		v2s get_dim() const { return dim; }
	};
	
	

	struct Front {
		
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
		Texture white1x1;
		Texture line_seg;
		
		// misc
		bool verbose{false};
		bool done{false};

		// public interface
		Front() = default;
		~Front();
		void init(std::string const& title, v2s dim);			
		
		void flip() { SDL_GL_SwapWindow(win); }

		v2s get_ctx_dim() const { return ctx_dim; }
		
		v2s get_win_dim() const { 
			int w,h;
			SDL_GetWindowSize(win, &w, &h);
			return v2s(w,h);
		}
		


		void set_ctx_dim(v2s);
		void set_ctx_dim();
		


		bool pool_event(SDL_Event & event);

		Texture make_texture(GLenum format, uint8_t const* data, v2s dim);
		Texture make_texture(ImageRGBA8 const& img);
		Texture make_texture(ImageA8 const& img);

		void render_texture(Texture const& tex, v2s pos);
		void render_texture(Texture const& tex, v2s pos, b2s src);
		void render_aamask(Texture const& tex, v2s pos, b2s src, RGBA8 color);
		void render_fill(b2s dst, RGBA8 color);
		void render_aaline_test(v2s a, v2s b, RGBA8 c, int8_t thick);
		
		Tick get_ticks();		
		inline uint8_t const* get_keyboard_state();

		void stop() { done = true; }

		void clear();

	private:
		//void set_blend_font(RGBA8 c);
		//void set_blend_fill(RGBA8 c);
		//void set_blend_norm();
		
		void render_call(int mode, GLint tex_unit, RGBA8 fg, b2s dst, v2f uva, v2f uvb);

		void create_SDL(std::string const& title, v2s dim);
		void destroy_SDL();

		void create_GL();
		void destroy_GL();
	};

	inline Front::~Front() {
		if (win) {
			destroy_GL();
			destroy_SDL();
		}
	}

	inline void Front::init(std::string const& title, v2s dim) {
		create_SDL(title, dim);
		create_GL();		
	}


	inline Tick get_ticks() {
		#ifdef __EMSCRIPTEN__
			return emscripten_get_now();
		#else
			return SDL_GetTicks();
		#endif
	}
	
	inline Tick Front::get_ticks() {
		return front::get_ticks();
	}
	

	inline bool Front::pool_event(SDL_Event & event) {
		return SDL_PollEvent(&event);
	}



}


