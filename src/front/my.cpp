#include "my.hpp"

#include "../ext/ext.hpp"

void check_gl(char const* fname, int line) {
	auto x = glGetError();
	if (x != GL_NO_ERROR) {
		do {
			print(std::cerr, "Error at [%||:%||]: %|| (code %||)\n", fname, line, get_gl_err_msg(x), x);
			x = glGetError();
		} while (x != GL_NO_ERROR);		
		assert(0);
		std::exit(-1);
	}
}

char const* get_gl_err_msg(GLuint x) {
	#define CASE_ERR(X) case X: return #X
	switch(x) {
		CASE_ERR(GL_NO_ERROR);
		CASE_ERR(GL_INVALID_ENUM);
		CASE_ERR(GL_INVALID_VALUE);
		CASE_ERR(GL_INVALID_OPERATION);
		CASE_ERR(GL_INVALID_FRAMEBUFFER_OPERATION);
		CASE_ERR(GL_OUT_OF_MEMORY);
		CASE_ERR(GL_STACK_UNDERFLOW);
		CASE_ERR(GL_STACK_OVERFLOW);
	}
	#undef CASE_ERR
	return "unknown opengl error code";
}


void myAttachShader(GLuint prog, GLenum shader_type, char const* shader_src)
{
	auto s = glCreateShader(shader_type);
	CHECK_GL();
	
	glAttachShader(prog, s);
	
	myLoadShader(s, shader_src);
	
	// will remain attached to program
	glDeleteShader(s);	
}


void myLoadShader(GLuint shader, char const* shader_src)
{
	glShaderSource(shader, 1, &shader_src, nullptr);
	CHECK_GL();	
	glCompileShader(shader);
	CHECK_GL();	
	
	GLint compiled;	
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	CHECK_GL();

	if (!compiled)
	{
		GLint len = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		CHECK_GL();

		char log[len];
		
		glGetShaderInfoLog(shader, len, nullptr, log);
		CHECK_GL();	

		print(std::cerr, shader_src);
		ext::fail("Error while compiling shader: %||\n", log);
	}
	
}


void myShowShader(GLuint shader) {
	GLint len = 0;
	glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &len);
	char * buf = new char[len];

	glGetShaderSource(shader, len, nullptr, buf);

	print(buf);

	delete buf;
	
}



void myShowGLInfo() {
	GLint v;
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &v);
	print("GL_MAX_VERTEX_UNIFORM_VECTORS = %||\n", v);
	
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &v);
	print("GL_MAX_FRAGMENT_UNIFORM_VECTORS = %||\n", v);

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &v);
	print("GL_MAX_VERTEX_ATTRIBS = %||\n", v);
	
	glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &v);
	print("GL_MAX_VERTEX_OUTPUT_COMPONENTS = %||\n", v);
	
	CHECK_GL();
}



void myLinkProgram(GLuint program) {
	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLint len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

		char log[len];

		glGetProgramInfoLog(program, len, nullptr, log);
		ext::fail("Error while linking program: %||\n", log);
	}
}

GLuint myGetAttribLocation(GLuint prog, char const* name) {
	auto loc = glGetAttribLocation(prog, name);
	if (loc == -1) {
		ext::fail("Error: GL: no such attribute: %||\n", name);
	}
	CHECK_GL();
	return loc;
}

GLuint myGetUniformLocation(GLuint prog, char const* name) {
	auto loc = glGetUniformLocation(prog, name);
	if (loc == -1) {
		ext::fail("Error: GL: no such uniform: %||\n", name);
	}
	CHECK_GL();
	return loc;
}
