#include "front.hpp"

#include "../lodepng/lodepng.h"
#include "my.hpp"
#include "shader.hpp"

namespace front {

	using geo2::vdiv;

	void init_glew() {
		// GL_INVALUD_ENUM bug
		glewExperimental = GL_TRUE;
		
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



	Texture make_texture(GLenum format, GLenum data_format, uint8_t const* data, v2s dim) 
	{
		GLenum data_type = GL_UNSIGNED_BYTE;
		
		if (format == GL_R8) 
		{
			
		}
		else if (format == GL_RGBA8) 
		{
			
		}
		else {
			ext::fail("make_texture: bad format\n");
		}
		
	
		Texture t;
		t.create();
		t.format = format;		
		t.dim = dim;
		
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		CHECK_GL();

		glActiveTexture(GL_TEXTURE0);   // use this texu as tmp
		glBindTexture(GL_TEXTURE_2D, t.id);
		CHECK_GL();
			
		glTexImage2D(GL_TEXTURE_2D, 0, format, dim[0], dim[1], 0, data_format, data_type, data);
		CHECK_GL();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		CHECK_GL();
		
		glBindTexture(GL_TEXTURE_2D, 0);
		CHECK_GL();		

		return t;
	}


	Texture make_texture(ImageRGBA8 const& img) {
		auto data = (uint8_t*)&img({0,0});
		return make_texture(GL_RGBA8, GL_RGBA, data, img.get_dim());
	}


	Texture make_texture(ImageR8 const& img) {
		auto data = (uint8_t*)&img({0,0});
		return make_texture(GL_R8, GL_RED, data, img.get_dim());
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
		0 texture texture       pos
		0 texture texture       pos src0
		1 aamask  texture color pos src0
		2 fill            color dst
		3 replace texture       dst src0 src1
	*/



	
	void RenderCall::operator()()
	{
		//print("INFO: render_call &win=%||\n", win);
			
		// uniforms
		auto ff = make_RGBAf(color);

		myUniform1i(win->prog[0], "u_base", texu);
		myUniform1i(win->prog[0], "u_mode", mode);
		myUniform4f(win->prog[0], "u_rgba", ff.r, ff.g, ff.b, ff.a);

		// vertex array
		auto d_pos = v2f(dst.pos);
		auto d_end = v2f(dst.end());

		GLfloat data[] = {
			d_pos[0], d_pos[1], uv0.pos[0], uv0.pos[1], uv1.pos[0], uv1.pos[1],
			d_pos[0], d_end[1], uv0.pos[0], uv0.end[1], uv1.pos[0], uv1.end[1],
			d_end[0], d_end[1], uv0.end[0], uv0.end[1], uv1.end[0], uv1.end[1],
			d_end[0], d_pos[1], uv0.end[0], uv0.pos[1], uv1.end[0], uv1.pos[1],
		};
		
		glBindBuffer(GL_ARRAY_BUFFER, win->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_DYNAMIC_DRAW);
		CHECK_GL();

		glBindVertexArray(win->vao[0]);
		CHECK_GL();

		// draw call		
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		CHECK_GL();
	}




	




	

	void Front::destroy_GL()
	{
		glDeleteBuffers(1, vbo);
		glDeleteVertexArrays(1, vao);
		glDeleteProgram(prog[0]);
		CHECK_GL();
	}

	void Front::create_GL()
	{
		init_glew();
		CHECK_GL();
		
		// set clear color
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		// first thing to do: clear screen to black
		//glClear(GL_COLOR_BUFFER_BIT);
		//SDL_GL_SwapWindow(win);
		CHECK_GL();

		// program
		prog[0] = glCreateProgram();
		myAttachShader(prog[0], GL_VERTEX_SHADER, shader::vert0);
		myAttachShader(prog[0], GL_FRAGMENT_SHADER, shader::frag0);
		myLinkProgram(prog[0]);
		CHECK_GL();
		print("INFO: prog[0]=%||\n", prog[0]);
		
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
			glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), 0);
			CHECK_GL();
		}
		
		{
			auto loc = myGetAttribLocation(prog[0], "a_uv0");
			glEnableVertexAttribArray(loc);
			glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)));
			CHECK_GL();
		}

		{
			auto loc = myGetAttribLocation(prog[0], "a_uv1");
			glEnableVertexAttribArray(loc);
			glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)(4*sizeof(GLfloat)));
			CHECK_GL();
		}


		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,  GL_ONE_MINUS_SRC_ALPHA);


		// projection matrix resolution
		resize_view(win_dim, ctx_dim);		
	}

	v2s Front::get_real_win_dim() const { 
		int w,h;
		SDL_GetWindowSize(win, &w, &h);
		return v2s(w,h);
	}			


		
	void Front::resize_view(v2s win_dim, v2s ctx_dim)
	{
		this->win_dim = win_dim;
		this->ctx_dim = ctx_dim;
		
		glViewport(0, 0, win_dim[0], win_dim[1]);	
		this->proj = make_projection_matrix(ctx_dim[0], ctx_dim[1]);
		glUniformMatrix4fv(
			myGetUniformLocation(prog[0], "u_proj"),
			1,
			GL_FALSE,
			glm::value_ptr(this->proj)
		);
		CHECK_GL();		
	}




	void Front::create_SDL(std::string const& title, v2s dim)
	{
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
		
		this->ctx = SDL_GL_CreateContext(this->win);
		check_sdl();

		this->ctx_dim = dim;
		this->win_dim = dim;
	}

	void Front::destroy_SDL() {
		SDL_GL_DeleteContext(this->ctx);
		SDL_DestroyWindow(this->win);
		SDL_Quit();
	}


}
