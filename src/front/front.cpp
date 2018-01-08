#include "front.hpp"

#include "../lodepng/lodepng.h"
#include "my.hpp"
#include "shader.hpp"

namespace front {


	using geo2::vdiv;

	void init_glew() {
		// GL_INVALUD_ENUM bug
		glewExperimental=GL_TRUE;
		
		auto x = glewInit();
		if (x != GLEW_OK) {
			print(std::cerr, "ERROR: GLEW: %||\n", glewGetErrorString(x));
			std::exit(-1);
		}

		// GL_INVALUD_ENUM bug
		auto err = glGetError();
		switch (err) {
			case GL_INVALID_ENUM:
				//print(std::cerr, "INFO: glewInit: GL_INVALID_ENUM\n");
				break;
			case GL_NO_ERROR:
				break;
			default:
				ext::fail("unexpected error\n");
		}
		
	}


	void check_sdl() {
		if (SDL_GetError()[0]) {
			print(std::cerr, "Error: SDL: %||\n", SDL_GetError());
			SDL_Quit();
			std::exit(1);		
		}
	}

	PixFont Front::make_font(filesys::Path const& path, int adv) {
		return PixFont(*this, path, adv);
	}

	

	Image load_png(filesys::Path const& path)
	{
		std::vector<uint8_t> pixels;
		unsigned width1, height1;
		unsigned err = lodepng::decode(pixels, width1, height1, path);
		if (err) {
			ext::fail("ERROR: lodepng: %||: %||\n", lodepng_error_text(err), path);		
		}
				
		// TODO: find way to decode directly to adress
		
		if (width1 > 16000) ext::fail("image too big");
		if (height1 > 16000) ext::fail("image too big");
		
		int16_t width = width1;
		int16_t height = height1;
		
		Image r({width, height});

		uint8_t * p = &pixels[0];
		
		for (int16_t j=0; j<height; ++j) {
			for (int16_t i=0; i<width; ++i) {
				Color c;
				c.r = *p;
				++p;
				c.g = *p;
				++p;
				c.b = *p;
				++p;
				c.a = *p;
				++p;

				r({i,j}) = c;
			}

		}

		return r;
	}

	Texture Front::make_texture(uint8_t const* rgba, v2s dim) {

		Texture t;
		t.create();
		
		t.dim = dim;
		
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		CHECK_GL();

		glBindTexture(GL_TEXTURE_2D, t.id);
		CHECK_GL();

			
		glTexImage2D(GL_TEXTURE_2D, 0, t.format, dim[0], dim[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
		CHECK_GL();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		CHECK_GL();

		return t;
	}


	


	Texture Front::make_texture(Image const& img) {
		auto data = (uint8_t*)&img({0,0});
		return make_texture(data, img.get_dim());
	}

	Texture Front::make_texture(filesys::Path const& path) {
		return make_texture(load_png(path));
	}

	void Texture::create() {
		assert(id == 0);
		glGenTextures(1, &id);
		CHECK_GL();		
	}

	void Texture::destroy() {
		glDeleteTextures(1, &id);	
		CHECK_GL();
		id = 0;  // ?	
	}





	glm::mat4 make_projection_matrix(int16_t width, int16_t height)
	{
		// 2D view
		// top-left pixel is (0,0)
		// +x goes right
		// +y goes down
		// bottom-right pixel is (width-1,height-1)

		// orto
		
		float tarr[] = {
			2.0f/width,  0.0f,         0.0f,     -1.0f,
			0.0f,       -2.0f/height,  0.0f,      1.0f,
			0.0f,        0.0f,        -2.0f/0.2,  0.0f,
			0.0f,        0.0f,         0.0f,      1.0f,
		};
		auto proj = glm::make_mat4x4(tarr);
		proj = glm::transpose(proj);	
		return proj;
	}


	glm::mat4 make_scale_matrix(v2f s)
	{
		float tarr[] = {
			s[0],    0.0f,  0.0f,  0.0f,
			0.0f,  s[1],    0.0f,  0.0f,
			0.0f,  0.0f,  1.0f,  0.0f,
			0.0f,  0.0f,  0.0f,  1.0f,
		};
		auto proj = glm::make_mat4x4(tarr);
		return proj;
	}




	void Front::render_texture(Texture const& t, v2s trg, b2s src, Color fg) {
		set_blend_font(fg);
		render_subtexture(t,trg,src);
	}


	void Front::set_blend_font(Color c) {
		auto v = ColorFloat(c);
		glBlendColor(v.r, v.g, v.b, v.a);
		glBlendFuncSeparate(GL_CONSTANT_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		glEnable(GL_BLEND);
	}

	void Front::set_blend_fill(Color c) {
		auto v = ColorFloat(c);
		glBlendColor(v.r, v.g, v.b, v.a);
		glBlendFuncSeparate(GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_ALPHA, GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		glEnable(GL_BLEND);
	}

	void Front::set_blend_norm() {
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		glEnable(GL_BLEND);
	}

	
	/*void Front::render_texture(Texture const& t, b2s trg, b2s src, Color fg) {
		set_blend_font(fg);
		
		
	}*/

	void Front::render_line(v2s a, v2s b, Color c, int8_t thick) 
	{
		// render textured quad
		
		set_blend_fill(c);
		
		if (a == b) {
			return;
		}
		
		auto aa = v2f(a);
		auto bb = v2f(b);
		
		auto x = bb - aa;
		auto y = v2f{-x[1], x[0]};
		auto n = (y * (1.0f / std::sqrt(y.norm2()))) * thick;
		
		// quad as triangle fan x,y + u,v + displacement
		GLfloat data[] = {
			aa[0], aa[1],  0.0f, 0.0f, -n[0], -n[1],
			aa[0], aa[1],  0.0f, 1.0f,  n[0],  n[1],
			bb[0], bb[1],  1.0f, 1.0f,  n[0],  n[1],
			bb[0], bb[1],  1.0f, 0.0f, -n[0], -n[1]
		};

		_render_call_GL(line_seg.id, data, sizeof(data));
	}


	void Front::render_fill(b2s box, Color c) {
		set_blend_fill(c);

		auto t_pos = v2f(box.pos);
		auto t_end = v2f(box.pos + box.dim);
		
		// quad as triangle fan x,y + u,v
		GLfloat data[] = {
			t_pos[0], t_pos[1],  0.0f, 0.0f,
			t_pos[0], t_end[1],  0.0f, 1.0f, 
			t_end[0], t_end[1],  1.0f, 1.0f,
			t_end[0], t_pos[1],  1.0f, 0.0f,
		};

		_render_call_GL(white1x1.id, data, sizeof(data));		
	}

	void Front::render_subtexture(Texture const& t, v2s trg, b2s src) {

		auto t_pos = v2f(trg);
		auto t_end = v2f(trg + src.dim);

		auto rs_pos = vdiv(v2f(src.pos), v2f(t.dim));
		auto rs_end = vdiv(v2f(src.pos + src.dim), v2f(t.dim));
		
		// quad as triangle fan x,y + u,v
		GLfloat data[] = {
			t_pos[0], t_pos[1],  rs_pos[0], rs_pos[1],
			t_pos[0], t_end[1],  rs_pos[0], rs_end[1], 
			t_end[0], t_end[1],  rs_end[0], rs_end[1],
			t_end[0], t_pos[1],  rs_end[0], rs_pos[1],
		};

		_render_call_GL(t.id, data, sizeof(data));
	}

	void Front::_render_call_GL(GLenum tex_id, GLfloat * data, size_t size) {
		// set texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_id);
		CHECK_GL();

		// update array
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
		CHECK_GL();

		glBindVertexArray(vao[0]);
		CHECK_GL();
		
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		CHECK_GL();
	}



	

	void Front::render_texture(Texture const& t, v2s trg, b2s src)
	{
		set_blend_norm();
		render_subtexture(t,trg,src);	
	}

	void Front::render_texture(Texture const& t, v2s pos_) {
		set_blend_norm();


		v2f pos = v2f(pos_);
		v2f end = pos + t.dim;
		
		// quad as triangle fan x,y + u,v
		GLfloat data[] = {
			pos[0], pos[1],  0.0f, 0.0f,
			pos[0], end[1],  0.0f, 1.0f, 
			end[0], end[1],  1.0f, 1.0f,
			end[0], pos[1],  1.0f, 0.0f,
		};

		_render_call_GL(t.id, data, sizeof(data));		
	}

	void Front::destroy_GL() {
		glDeleteBuffers(1, vbo);
		glDeleteVertexArrays(1, vao);
		glDeleteProgram(prog[0]);
		CHECK_GL();
	}

	void Front::create_GL() {
		init_glew();
		CHECK_GL();

		// render texture program
		prog[0] = glCreateProgram();
		myAttachShader(prog[0], GL_VERTEX_SHADER, shader::vert0);
		myAttachShader(prog[0], GL_FRAGMENT_SHADER, shader::frag0);
		myLinkProgram(prog[0]);
		CHECK_GL();	
		
		glUseProgram(prog[0]);
		CHECK_GL();

		
		glGenVertexArrays(1, vao);
		CHECK_GL();
		
		glGenBuffers(1, vbo);
		CHECK_GL();

		// vertex_array[0] => x y u v
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		CHECK_GL();
		
		glBindVertexArray(vao[0]);
		CHECK_GL();

		{
			auto loc = myGetAttribLocation(prog[0], "a_xy");		
			glEnableVertexAttribArray(loc);
			glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), 0);
			CHECK_GL();
		}
		
		{
			auto loc = myGetAttribLocation(prog[0], "a_uv");
			glEnableVertexAttribArray(loc);
			glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)));
			CHECK_GL();
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		
		
		set_ctx_dim(win_dim);
		CHECK_GL();
		
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		CHECK_GL();

		// activate prog[0] (render texture simple)

		glUniform1i(myGetUniformLocation(prog[0], "s_texture"), 0);
		
		{
			uint8_t rgba[] = {255,255,255,255};
			white1x1 = make_texture(rgba, {1,1});
		}
		
		{
			uint8_t rgba[] = {
				63,63,63,63,
				127,127,127,127,
				255,255,255,255,
				127,127,127,127,
				63,63,63,63
			};
			line_seg = make_texture(rgba, {1,5});
		}

		clear();
		
	}

	void Front::set_ctx_dim(v2s ctx_dim)
	{
		auto win_dim = get_win_dim();
			
		glViewport(0, 0, win_dim[0], win_dim[1]);	
	
		this->proj = make_projection_matrix(ctx_dim[0], ctx_dim[1]);

		glUniformMatrix4fv(
			myGetUniformLocation(prog[0], "m_proj"),
			1,
			GL_FALSE,
			glm::value_ptr(this->proj)
		);
		CHECK_GL();

		this->ctx_dim = ctx_dim;
	}

	void Front::set_ctx_dim() {
		set_ctx_dim(ctx_dim);
	}

	void Front::clear() {
		glClear(GL_COLOR_BUFFER_BIT);
		CHECK_GL();
	}

	void Front::create_SDL(std::string const& title, v2s dim) {
		#ifdef __EMSCRIPTEN__
			auto r = SDL_Init(SDL_INIT_VIDEO);
			check_sdl();
		#else
			auto r = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);
			check_sdl();
		#endif
				
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		check_sdl();
				
		this->win = SDL_CreateWindow(title.c_str(), 
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			dim[0], dim[1],
			SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE
		);
		check_sdl();
		
		this->ctx = SDL_GL_CreateContext(win);
		check_sdl();

		this->ctx_dim = dim;		
	}

	void Front::destroy_SDL() {
		SDL_GL_DeleteContext(ctx);
		SDL_DestroyWindow(win);
		SDL_Quit();
	}


}
