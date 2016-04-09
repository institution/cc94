#pragma once

namespace shader
{

char const* vert0 = R"(
	#version 300 es

	precision mediump float;

	uniform mat4 m_proj;
	
	layout(location = 0) in vec2 a_xy;	
	layout(location = 1) in vec2 a_uv;	
	out vec2 v_uv;
	
	void main()
	{
		gl_Position = m_proj * vec4(a_xy.x, a_xy.y, 0.0, 1.0);
		v_uv = a_uv;
	}
)";



char const* frag0 = R"(
	#version 300 es

	precision mediump float;

	in vec2 v_uv;

	layout(location = 0) out vec4 outcolor;
	
	uniform sampler2D s_texture;
	
	void main()
	{
		outcolor = texture(s_texture, v_uv);
	}
)";



}
