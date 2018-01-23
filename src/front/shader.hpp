#pragma once

namespace shader
{


char const* vert0 = R"(
	#version 300 es
	precision mediump float;

	uniform mat4 u_proj;
		
	in vec2 a_xy;	
	in vec2 a_uv;

	out vec2 b_uv;
	
	void main()
	{
		gl_Position = u_proj * vec4(a_xy.x, a_xy.y, 0.0, 1.0);
		b_uv = a_uv;
	}
)";



char const* frag0 = R"(
	#version 300 es
	precision mediump float;

	uniform sampler2D u_base;
	uniform int u_mode;
	uniform vec4 u_rgba;
	
	in vec2 b_uv;
	
	out vec4 outcolor;
		
	void main()
	{
		switch (u_mode) // texture
		{
			case 0: // texture
				outcolor = texture(u_base, b_uv);
				break;
			case 1: // aamask
				outcolor = vec4(u_rgba.rgb, texture(u_base, b_uv).a * u_rgba.a);
				break;
			case 2: // fill
				outcolor = u_rgba;
				break;
			default: // all red
				outcolor = vec4(1,0,0,1);
				break;
		}		
	}
)";


}
