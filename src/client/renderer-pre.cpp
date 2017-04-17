#include "renderer-pre.hpp"



namespace col {

	PixFont FontTiny;

	using ResMap = std::unordered_map<ResKey, Texture>;

	ResMap res_map;
	
	ResKey make_reskey(ResCat cat, ResNum num) {
		return (ResKey(cat) << 16) | ResKey(num);
	}

	ResCat get_rescat(ResKey k) {
		return k >> 16;
	}

	ResNum get_resnum(ResKey k) {
		return (k << 16) >> 16;
	}

	Path get_res_dir(ResCat cat) {
		// categories, *xxx* -- generated to memory image
		switch (cat) {
			case TERR: return "TERRAIN_SS";
			case ICON: return "ICONS_SS";
			case PHYS: return "PHYS0_SS";
			case ARCH: return "PARCH_SS";
			case COLONY: return "COLONY_PIK";
			case BUILD: return "BUILDING_SS";
			case WOODTILE: return "WOODTILE_SS";
			case DIFFUSE: return "*DIFFUSE*";
			case COAST: return "*COAST*";
			case ZZ: return "ZZ";
		}
		fail("unknown resource category: %||\n", cat);
	}

	Path get_res_path(ResCat cat, ResNum num) {
		return conf.tile_path / get_res_dir(cat) / format("%|03|.png", num);
	}

	Path get_res_path_key(ResKey key) {		
		return get_res_path(get_rescat(key), get_resnum(key));
	}


	// Get resource from global cache (load on demand)
	Texture const& res(Front & fr, ResCat cat, ResNum num)
	{
		auto key = make_reskey(cat, num);		
		auto p = res_map.find(key);		
		if (p != res_map.end()) {
			return (*p).second;
		}
		else {
			auto path = get_res_path(cat, num);
			auto & img = res_map[key];
			img = fr.make_texture(path);
			return img;
		}
	}
	
	void preload(ResCat cat, ResNum num, Texture && tex) {
		auto key = make_reskey(cat, num);		
		res_map[key] = std::move(tex);
	}

	Image replace_black(Image const& inn, Image const& tex, v2s toff) {
		/* Create new surface

		mask colors meaning:
		 black -> take from tex
		 else -> take from mask

		 toff -- tex offset vector

		*/

		//print("replace_black: %|| + %|| <- %||\n", inn.dim, toff, tex.dim);

		auto dim = inn.get_dim();
		auto out = Image(dim);

		for (int j = 0; j < dim[1]; ++j) {
			for (int i = 0; i < dim[0]; ++i) {
				auto c = inn({i,j});
				auto pos = v2s(i,j);

				//print("c = %||\n", c);

				if (c.a == 0) {
					out(pos) = Color(0,0,0,0);
				}
				else if (c == Color(0,0,0,255)) {

					out(pos) = tex(toff + pos);
				}
				else {
					out(pos) = inn(pos);
				}

			}
		}

		return out;
	}

	//void render_fields(Front &win, Env const& env, Console const& con,
	//	v2s pix, Coords const& pos);


	v2s get_loffset(int l) {
		switch(l){
			case 0: return v2s(0,0) * ly.scale;
			case 1: return v2s(8,0) * ly.scale;
			case 2: return v2s(8,8) * ly.scale;
			case 3: return v2s(0,8) * ly.scale;
		}
		fail("invalid 'l'");
	}

	

	Texture make_combined_texture(
		Front & fr,
		std::pair<ResCat, ResNum> p,
		std::pair<ResCat, ResNum> b,
		v2s off
	) {
		auto p_img = front::load_png(
			get_res_path(p.first, p.second)
		);

		auto b_img = front::load_png(
			get_res_path(b.first, b.second)
		);

		auto surf = replace_black(p_img, b_img, off);

		return fr.make_texture(surf);
	}

	


	void preload_coast(Front & fr, Biome bio, int start) {
		for (int k = 0; k < 8; ++k) {
			for (int l = 0; l < 4; ++l) {
				int ind = (k<<2) + l;

				preload(COAST, start + ind,
					make_combined_texture(fr,
						{PHYS, 109+ind},
						{TERR, bio},
						get_loffset(l)
					)
				);
			}
		}
	}

	int const TextureOceanId = 11;
	int const TextureSeaLaneId = 11;

	void preload_coast(Front & back) {
		// 109 - 140

		preload_coast(back, TextureOceanId, 0);
		preload_coast(back, TextureSeaLaneId, 50);
	}




	void preload_terrain(Front & fr) {
		for (int i=1; i<13; ++i) {
			preload(DIFFUSE,  0+i,
				make_combined_texture(fr, {PHYS, 105}, {TERR, i}, {0,0})
			);
			preload(DIFFUSE, 50+i,
				make_combined_texture(fr, {PHYS, 106}, {TERR, i}, {0,0})
			);
			preload(DIFFUSE, 100+i,
				make_combined_texture(fr, {PHYS, 107}, {TERR, i}, {0,0})
			);
			preload(DIFFUSE, 150+i,
				make_combined_texture(fr, {PHYS, 108}, {TERR, i}, {0,0})
			);
		}

		preload_coast(fr);
	}

	
	
	void preload_renderer(Front & fr) {
		preload_terrain(fr);
		FontTiny.load(fr, conf.font_tiny_path, -1 * ly.scale);
	}
	
}
