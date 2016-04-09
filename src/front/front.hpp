#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "base.hpp"
#include "glm.hpp"
#include "color.hpp"
#include "input.hpp"


namespace front {


	

	struct Texture {
		GLuint id{0};
		v2s dim;
		GLenum format{GL_RGBA};

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

	struct PixFont;


		

		


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
		
		// misc
		bool verbose{false};
		bool done{false};

		// public interface
		Front() = default;
		~Front();
		void init(std::string const& title, v2s dim);			
		
		void flip() { SDL_GL_SwapWindow(win); }

		v2s get_logical_dim() const { return ctx_dim; }
		v2s get_physical_dim() const { return win_dim; }

		void set_logical_dim(v2s dim);
		void set_physical_dim(v2s dim);


		bool pool_event(Event & event);

		Texture make_texture(filesys::Path const& path);
		Texture make_texture(Image const& img);
		Texture make_texture(uint8_t const* rgba, v2s dim);
		
		PixFont make_font(filesys::Path const& path, int adv);


		void render_texture(Texture const& t, v2s pos_);
		void render_texture(Texture const& t, v2s trg, b2s src);
		void render_texture(Texture const& t, v2s trg, b2s src, Color fg);
		void render(Texture const& t, v2s pos) { render_texture(t, pos); }
		void render(Texture const& t, v2s trg, b2s src) { render_texture(t, trg, src); }
		void render(Texture const& t, v2s trg, b2s src, Color fg) { render_texture(t, trg, src, fg); }

		//void render_texture(Texture const& t, b2s trg, b2s src, Color fg);

		void render_fill(b2s box, Color c);
		
		Tick get_ticks();		
		inline uint8_t const* get_keyboard_state();

		void stop() { done = true; }

		void clear();

	private:
		void set_blend_font(Color c);
		void set_blend_norm();
		void render_subtexture(Texture const& t, v2s trg, b2s src);
		void _render_call_GL(GLenum tex_id, GLfloat * data, size_t size);

		void create_SDL(std::string const& title, v2s dim);
		void destroy_SDL();

		void create_GL();
		void destroy_GL();
	};

	inline Front::~Front() {
		destroy_GL();
		destroy_SDL();
	}

	inline void Front::init(std::string const& title, v2s dim) {
		create_SDL(title, dim);
		create_GL();
	}

	inline Tick Front::get_ticks() {
		#ifdef __EMSCRIPTEN__
			return emscripten_get_now();
		#else
			return SDL_GetTicks();
		#endif
	}

	inline bool Front::pool_event(Event & event) {
		return SDL_PollEvent(&event);
	}


	Image load_png(filesys::Path const& path);

}

#include "pixfont.hpp"

