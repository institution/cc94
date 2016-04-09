#pragma once


#include <GL/glew.h>




#define CHECK_GL() \
	check_gl(__FILE__, __LINE__) 

void check_gl(char const* fname, int line);

char const* get_gl_err_msg(GLuint x);

void myAttachShader(GLuint prog, GLenum shader_type, char const* shader_src);

void myLoadShader(GLuint shader, char const* shader_src);

void myShowShader(GLuint shader);

void myLinkProgram(GLuint program);

GLuint myGetAttribLocation(GLuint prog, char const* name);

GLuint myGetUniformLocation(GLuint prog, char const* name);


void myShowGLInfo();
