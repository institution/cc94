#pragma once

namespace col{

	using Dir = uint8_t;
	Dir const
		DirQ=0, DirW=1, DirE=2,
		DirA=3, DirS=4, DirD=5,
		DirZ=6, DirX=7, DirC=8;
	
	inline Coords dir2vec(Dir d) {
		return Coords((d % 3) - 1, (d / 3) - 1);
	}

	inline Dir vec2dir(Coords d) {
		return (d[0] + 1) + (d[1] + 1) * 3;
	}
	
	inline string get_dir_name(Dir dir) {
		switch (dir) {
			case DirQ: return "DirC";
			case DirW: return "DirX";
			case DirE: return "DirZ";
			case DirA: return "DirD";
			case DirS: return "DirS";
			case DirD: return "DirA";
			case DirZ: return "DirE";
			case DirX: return "DirW";
			case DirC: return "DirQ";
			default: 
				print(std::cerr, "WARINING: unknown dir code: %||\n", dir);
				return "DirError";
		}
	}
	
	
	inline Dir turnabout(Dir a) {
		switch (a) {
			case DirQ: return DirC;
			case DirW: return DirX;
			case DirE: return DirZ;
			case DirA: return DirD;
			case DirS: return DirS;
			case DirD: return DirA;
			case DirZ: return DirE;
			case DirX: return DirW;
			case DirC: return DirQ;
		}
		assert(false);
	}
	
	inline Dir rotate_cw(Dir a) {
		switch (a) {
			case DirQ: return DirW;
			case DirW: return DirE;
			case DirE: return DirD;
			case DirA: return DirQ;
			case DirS: return DirS;
			case DirD: return DirC;
			case DirZ: return DirA;
			case DirX: return DirZ;
			case DirC: return DirX;
		}
		assert(false);
	}

	inline Dir rotate_ccw(Dir a) {
		switch (a) {
			case DirQ: return DirA;
			case DirW: return DirQ;
			case DirE: return DirW;
			case DirA: return DirZ;
			case DirS: return DirS;
			case DirD: return DirE;
			case DirZ: return DirX;
			case DirX: return DirC;
			case DirC: return DirD;
		}
		assert(false);
	}

	array<Dir, 9> const AllDirs = {
		DirQ,DirW,DirE,
		DirA,DirS,DirD,
		DirZ,DirX,DirC
	};
	
	std::array<Dir, 8> const Neighs = {
		DirQ,DirW,DirE,
		DirA,     DirD,
		DirZ,DirX,DirC
	};

	
	/* Dir code is yx in mod 3-1 (2 -> -1)
	00 0  -1,-1
	01 1  -1,0
	02 2  -1,1
	10 3  0,-1
	11 4  0,0
	12 5  0,1
	20 6  1,-1
	21 7  1,0
	22 8  1,1
	*/


}
