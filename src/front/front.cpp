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



	Texture Front::make_texture(GLenum format, uint8_t const* data, v2s dim) 
	{
		if (format == GL_ALPHA) {
			
		}		
		else if (format == GL_RGBA) 
		{
			
		}
		else {
			ext::fail("make_texture: bad format");
		}
		
	
		Texture t;
		t.create();
		t.format = format;		
		t.dim = dim;
		
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		CHECK_GL();

		glBindTexture(GL_TEXTURE_2D, t.id);
		CHECK_GL();
			
		glTexImage2D(GL_TEXTURE_2D, 0, format, dim[0], dim[1], 0, format, GL_UNSIGNED_BYTE, data);
		CHECK_GL();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		CHECK_GL();

		return t;
	}


	Texture Front::make_texture(ImageRGBA8 const& img) {
		auto data = (uint8_t*)&img({0,0});
		return make_texture(GL_RGBA, data, img.get_dim());
	}


	Texture Front::make_texture(ImageA8 const& img) {
		auto data = (uint8_t*)&img({0,0});
		return make_texture(GL_ALPHA, data, img.get_dim());
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


	/*
	void Front::set_blend_font(RGBA8 c) {
		auto v = make_RGBAf(c);
		glBlendColor(v.r, v.g, v.b, v.a);
		glBlendFuncSeparate(GL_CONSTANT_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		glEnable(GL_BLEND);
	}

	void Front::set_blend_fill(RGBA8 c) {
		auto v = make_RGBAf(c);
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
	*/

	
	/*void Front::render_texture(Texture const& t, b2s trg, b2s src, Color fg) {
		set_blend_font(fg);
		
		
	}*/



	/*
		0 texture texture       pos
		0 texture texture       pos src
		1 aamask  texture color pos src
		2 fill            color dst
	*/
	
	void Front::render_texture(Texture const& tex, v2s pos)
	{
		// MODE_TEXTURE, tex_unit, dst, src
		myBindTexture(GL_TEXTURE0, tex.id);
		render_call(0, 0, RGBA8(0,0,0,0), {pos, tex.dim}, v2f(0,0), v2f(1,1));
	}
	
	void Front::render_texture(Texture const& tex, v2s pos, b2s src)
	{
		// MODE_TEXTURE, tex_unit, dst, src
		auto uva = vdiv(v2f(src.pos), v2f(tex.dim));
		auto uvb = vdiv(v2f(src.pos + src.dim), v2f(tex.dim));
		myBindTexture(GL_TEXTURE0, tex.id);
		render_call(0, 0, RGBA8(0,0,0,0), {pos, src.dim}, uva, uvb);		
	}
	
	void Front::render_aamask(Texture const& tex, v2s pos, b2s src, RGBA8 color)
	{		
		// MODE_AAMASK, tex_unit, color, dst, src
		auto uva = vdiv(v2f(src.pos), v2f(tex.dim));
		auto uvb = vdiv(v2f(src.pos + src.dim), v2f(tex.dim));
		myBindTexture(GL_TEXTURE0, tex.id);
		render_call(1, 0, color, {pos, src.dim}, uva, uvb);		
	}
	 
	void Front::render_fill(b2s dst, RGBA8 color)
	{
		//  MODE_FILL,    color, dst		
		render_call(2, 0, color, dst, v2f(0,0), v2f(0,0));		
	}

	void Front::render_call(int mode, GLint tex_unit, RGBA8 fg, b2s dst, v2f uva, v2f uvb)
	{		
		// uniforms
		auto ff = make_RGBAf(fg);
		
		myUniform1i(prog[0], "u_base", tex_unit);
		myUniform1i(prog[0], "u_mode", mode);
		myUniform4f(prog[0], "u_rgba", ff.r, ff.g, ff.b, ff.a);

		// vertex array
		auto d_pos = v2f(dst.pos);
		auto d_end = v2f(dst.pos + dst.dim);

		GLfloat data[] = {
			d_pos[0], d_pos[1], uva[0], uva[1],
			d_pos[0], d_end[1], uva[0], uvb[1], 
			d_end[0], d_end[1], uvb[0], uvb[1],
			d_end[0], d_pos[1], uvb[0], uva[1],
		};
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_DYNAMIC_DRAW);
		CHECK_GL();

		glBindVertexArray(vao[0]);
		CHECK_GL();

		// draw call		
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		CHECK_GL();
	}

	void Front::render_aaline_test(v2s a, v2s b, RGBA8 c, int8_t thick) 
	{
		// render textured quad
		
		/*set_blend_fill(c);
		
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
		*/
		//_render_call_GL(line_seg.id, data, sizeof(data));
	}


	




	

	void Front::destroy_GL()
	{
		glDeleteBuffers(1, vbo);
		glDeleteVertexArrays(1, vao);
		glDeleteProgram(prog[0]);
		glDeleteProgram(prog[1]);
		CHECK_GL();
	}

	void Front::create_GL()
	{
		init_glew();
		CHECK_GL();

		// program
		prog[0] = glCreateProgram();
		myAttachShader(prog[0], GL_VERTEX_SHADER, shader::vert0);
		myAttachShader(prog[0], GL_FRAGMENT_SHADER, shader::frag0);
		myLinkProgram(prog[0]);
		CHECK_GL();	
		
		glUseProgram(prog[0]);
		CHECK_GL();

		// vertex arrays
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

		// enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,  GL_ONE_MINUS_SRC_ALPHA);

		// clear color
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		CHECK_GL();

		// opengl resolution
		set_ctx_dim(win_dim);
				
		// technical textures
		{
			uint8_t rgba[] = {255,255,255,255};
			white1x1 = make_texture(GL_RGBA, rgba, {1,1});
		}
		
		{
			uint8_t rgba[] = {
				63,63,63,63,
				127,127,127,127,
				255,255,255,255,
				127,127,127,127,
				63,63,63,63
			};
			line_seg = make_texture(GL_RGBA, rgba, {1,5});
		}

		// clear screen
		clear();
		
	}

	void Front::set_ctx_dim(v2s ctx_dim)
	{
		auto win_dim = get_win_dim();
			
		glViewport(0, 0, win_dim[0], win_dim[1]);	
	
		this->proj = make_projection_matrix(ctx_dim[0], ctx_dim[1]);

		glUniformMatrix4fv(
			myGetUniformLocation(prog[0], "u_proj"),
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
