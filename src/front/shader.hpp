#pragma once

namespace shader
{


char const* vert0 = R"(
	#version 300 es
	precision mediump float;
	
	uniform mat4 u_proj;
	uniform lowp int u_mode;

			
	in vec2 a_xy;	
	in vec2 a_uv0;
	in vec2 a_uv1;

	out vec2 b_uv0;
	out vec2 b_uv1;
	
	void main()
	{		
		gl_Position = u_proj * vec4(a_xy.x, a_xy.y, 0.0, 1.0);		
		b_uv0 = a_uv0;
		b_uv1 = a_uv1;
	}
)";



char const* frag0 = R"(
	#version 300 es
	precision mediump float;

	uniform sampler2D u_base;
	uniform lowp int u_mode;
	uniform vec4 u_rgba;
	
	in vec2 b_uv0;
	in vec2 b_uv1;
	
	out vec4 outcolor;
		
	void main()
	{
		switch (u_mode) // texture
		{
			case 0: // debug
				outcolor = vec4(1,0,0,1);
				break;
			case 1: // texture
				outcolor = texture(u_base, b_uv0);
				break;
			case 2: // aamask
				outcolor = vec4(u_rgba.rgb, texture(u_base, b_uv0).a * u_rgba.a);
				break;
			case 3: // fill
				outcolor = u_rgba;
				break;
			case 4: // replace black with second image
				vec4 rgba0 = texture(u_base, b_uv0);
				if (rgba0 == vec4(0,0,0,1)) {
					outcolor = texture(u_base, b_uv1);
				}
				else {
					outcolor = rgba0;
				}
				break;				
			default: // all red
				outcolor = vec4(1,0,0,1);
				break;
		}		
	}
)";



}
