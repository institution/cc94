#ifndef BOX2_H
#define BOX2_H

#include "col.hpp"

namespace col{



	struct Box2 {

		v2i pos, dim, end;

		Box2() = default;

		Box2(v2i const& pos, v2i const& dim):
			pos(pos), dim(dim), end(pos+dim)
		{}

		Box2(int x, int y, int w, int h):
			pos(x,y), dim(w,h), end(x+w,y+h)
		{}


	};

	struct Box {

		v2i pos, dim;

		Box() = default;

		Box(v2i const& pos, v2i const& dim):
			pos(pos), dim(dim)
		{}

		Box(int x, int y, int w, int h):
			pos(x,y), dim(w,h)
		{}

		Box(Box2 const& box):
			pos(box.pos), dim(box.dim)
		{}


	};
}


#endif
