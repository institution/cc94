#ifndef BOX2_H
#define BOX2_H

#include "col.hpp"

namespace col{

	struct Box {

		v2i pos, dim, end;

		Box() = default;

		Box(v2i const& pos, v2i const& dim):
			pos(pos), dim(dim), end(pos+dim)
		{}

		Box(int x, int y, int w, int h):
			pos(x,y), dim(w,h), end(x+w,y+h)
		{}


	};




}


#endif
