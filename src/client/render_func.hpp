#pragma once

#include "base.hpp"
#include "conf.hpp"
#include "res.hpp"
#include <string>
#include "align.hpp"
#include "../front/front.hpp"

namespace col
{
	using front::RenderCall;


	inline void render_texture(b2s dst, TexUnit texu)
	{
		RenderCall rc;
		rc.win = &win;
		rc.mode = front::MODE_TEXTURE;
		rc.texu = texu;
		rc.dst = dst;
		rc.uv0 = aabb2f(0,0,1,1);
		rc();
	}
	
	inline void render_sprite(v2s pos, Sprite const& s0)
	{
		RenderCall rc;
		rc.win = &win;
		rc.mode = front::MODE_TEXTURE;
		rc.texu = TEXU_SPRITES;
		rc.dst = b2s(pos, s0.dim);
		rc.uv0 = s0.box;
		rc();
	}

	inline void render_sprite_replace(v2s pos, Sprite const& s0, Sprite const& s1)
	{
		RenderCall rc;
		rc.win = &win;
		rc.mode = front::MODE_REPLACE;
		rc.texu = TEXU_SPRITES;
		rc.dst = b2s(pos, s0.dim);
		rc.uv0 = s0.box;
		rc.uv1 = s1.box;
		rc();
	}

	inline void render_fill(b2s box, RGBA8 color)
	{
		RenderCall rc;
		rc.win = &win;
		rc.mode = front::MODE_FILL;
		rc.color = color;
		rc.dst = box;
		rc();
	}

	inline void render_aamask(TexUnit texu, v2s pos, Sprite const& s0, RGBA8 color)
	{
		RenderCall rc;
		rc.win = &win;
		rc.mode = front::MODE_AAMASK;
		rc.texu = texu;
		rc.dst = b2s(pos, s0.dim);
		rc.uv0 = s0.box;
		rc.color = color;
		rc();
	}

	inline void render_aaline_test(v2s a, v2s b, RGBA8 c, int8_t thick) 
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
	
	/// Return text dim
	v2s get_text_dim(Font const& font, string const& text);
	
	
	/// Render text at pos (top-left)
	b2s render_text(
		v2s pos,
		Font const& font, RGBA8 fg,
		string const& text		
	);
		
	/// Render text aligned inside box with background
	b2s render_text(
		v2s pos, v2s dim, v2f align,
		Font const& font, RGBA8 fg, RGBA8 bg,
		string const& text		
	);

}
