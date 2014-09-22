#include <string>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <array>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <boost/filesystem.hpp>
#include "format.hpp"
#include "error.h"
#include "lodepng/lodepng.h"
#include "osdep.h"

using int8 = std::int8_t;
using uint8 = std::uint8_t;
using int16 = std::int16_t;
using uint16 = std::uint16_t;
using int32 = std::int32_t;
using uint32 = std::uint32_t;



using std::array;
using std::vector;
using std::string;



namespace filesys = boost::filesystem;

using Path = filesys::path;


// --------------------------------------------------


using RGBA = array<uint8, 4>;
using Buffer = vector<uint8>;

struct Image{
	int w, h;
	vector<RGBA> data;
	Image(int w, int h): w(w), h(h) {
		data.resize(w*h);
	}

	RGBA & at(int i, int j) {
		assert(i < w);
		assert(j < h);
		return data.at(j * w + i);
	}
};


struct Pallete{
	int w;
	vector<RGBA> data;



	Pallete(int w): w(w) {
		data.resize(w);
	}

	RGBA & at(int i) {
		assert(i < w);
		return data.at(i);
	}


	RGBA const& at(int i) const {
		assert(i < w);
		return data.at(i);
	}
};




struct PikHeader{
	uint16 height;
	uint16 width;
	uint16 unk1;
	uint16 unk2;
};


// --------------------------------------------------



template<unsigned N, class T>
array<uint8,N> subarr(T const& data, size_t p) {
	array<uint8, N> r;
	for (size_t i=0; i<N; ++i) {
		r.at(i) = data.at(p+i);
	}
	return r;
}


struct imbuf
{
	uint8 const * const a;
	uint8 const * const b;

	uint8 const * c;

	imbuf(vector<uint8> const& xs): a(&*begin(xs)), b(&*end(xs)) {
		c = a;
    }

	void read(uint8 *buf, size_t n) {
		for (size_t i=0; i<n; ++i) {
			if (c == b) {
				throw Error("end of buffer");
			}
			buf[i] = *c;
			++c;
		}
	}

};

struct ifbuf
{
	std::ifstream & b;

	ifbuf(std::ifstream & b): b(b) {}

	void read(uint8 *buf, size_t n) {
		b.read(reinterpret_cast<char*>(buf), n);
	}

	void seekg(size_t n) {
		b.seekg(n);
	}

	size_t tellg() const {
		return b.tellg();
	}
};





Path get_dname(Path const& fname) {
	// fname -- path to Colonization madsfile (*.SS or *.PIK)
	// return -- destination dir path (unpack to ...)

	string tmp(fname.filename().c_str());
	std::replace( tmp.begin(), tmp.end(), '.', '_');

	Path ddirname(tmp);

	filesys::path dname;
	dname /= os::get_home_dir();
	dname /= ".cc94";
	dname /= "COLONIZE";
	dname /= ddirname;

	filesys::create_directories(dname);

	return dname;
}


// little endian

uint8 read_uint8(array<uint8,1> const& data) {
	return data.at(0);
}

uint16 read_uint16(array<uint8,2> const& data) {
	uint16 r = 0;
	r |= uint16(data.at(1)) << 8;
	r |= uint16(data.at(0)) << 0;
	return r;
}

uint32 read_uint32(array<uint8,4> const& data) {
	uint32 r = 0;
	r |= uint32(data.at(3)) << 24;
	r |= uint32(data.at(2)) << 16;
	r |= uint32(data.at(1)) << 8;
	r |= uint32(data.at(0)) << 0;
	return r;
}

int32 read_int32(array<uint8,4> const& data) {
	return int32(read_uint32(data));
}

template <unsigned N, class B>
array<uint8, N> read(B & f) {
	array<uint8, N> buffer;
	f.read(&buffer[0], N);
	return buffer;
}

template <class B>
string read_str(B & f, size_t n) {
	string buffer;
	buffer.resize(n);
	f.read(reinterpret_cast<uint8*>(&buffer[0]), n);
	return buffer;
}

template <class B>
Buffer read_buf(B & f, size_t n) {
	Buffer buffer;
	buffer.resize(n);
	f.read(&buffer[0], n);
	return buffer;
}



PikHeader read_pik_header(vector<uint8> const& data) {
	if (data.size() != 8) {
		throw Error("pik header has wrong size: %||", data.size());
	}

	PikHeader h;
	h.height = read_uint16(subarr<2>(data, offsetof(PikHeader, height)));
	h.width = read_uint16(subarr<2>(data, offsetof(PikHeader, width)));
	h.unk1 = read_uint16(subarr<2>(data, offsetof(PikHeader, unk1)));
	h.unk2 = read_uint16(subarr<2>(data, offsetof(PikHeader, unk2)));

	return h;
}


size_t fab_decompress(ifbuf & src, Buffer & dest) {
	// Decompress fab section
	// src -- array
	// dest -- array
	// return -- length of decoded data

	auto fab = read_str(src, 3);
	if (fab != "FAB") {
		throw Error("fab_decode: header not found");
	}

	uint8 shift_val = read_uint8(read<1>(src));
	if (!(10 <= shift_val and shift_val < 14)) {
		throw Error("fab_decode: invalid shift_val: %||", shift_val);
	}

	// uint8s
	uint8 copy_len = 0;
	uint8 copy_ofs_shift = 16 - shift_val;
	uint8 copy_ofs_mask = 0xFF << (shift_val - 8);
	uint8 copy_len_mask = (1 << copy_ofs_shift) - 1;

	// uint32
	uint32 copy_ofs = 0xFFFF0000;


	uint8 _bits_left = 16;
	uint32 _bit_buffer = read_uint16(read<2>(src));

	//print("bb = %||\n", _bit_buffer);

	// i = 6  # src pos
	size_t j = 0;  // dest pos

	auto get_bit = [&_bits_left, &_bit_buffer, &src]() -> decltype(_bit_buffer) {
		_bits_left -= 1;
		if (_bits_left == 0) {
			_bit_buffer = (read_uint16(read<2>(src)) << 1) | (_bit_buffer & 1);
			_bits_left = 16;
			//print("bb = %||\n", _bit_buffer);
		}
		auto bit = _bit_buffer & 1;
		_bit_buffer >>= 1;
		//print("bit=%||\n", int(bit));
		return bit;
	};


	while (1) {
		if (get_bit() == 0) {
			if (get_bit() == 0) {
				copy_len = ((get_bit() << 1) | get_bit()) + 2;
				copy_ofs = read_uint8(read<1>(src)) | 0xFFFFFF00;
			}
			else {
				uint8 a = read_uint8(read<1>(src));
				uint8 b = read_uint8(read<1>(src));

				copy_ofs = (((b >> copy_ofs_shift) | copy_ofs_mask) << 8) | a;
				copy_len = b & copy_len_mask;

				if (copy_len == 0) {
					copy_len = read_uint8(read<1>(src));

					if (copy_len == 0) {
						break;
					}
					else if (copy_len == 1) {
						continue;
					}
					else {
						copy_len += 1;
					}
				}
				else {
					copy_len += 2;
				}

				copy_ofs |= 0xFFFF0000;
			}

			while (copy_len > 0) {
				//print("j=%||\n", j);
				dest.at(j) = dest[j + int32(copy_ofs)];
				j += 1;
				copy_len -= 1;
			}

		}
		else {
			//print("j=%||\n", j);
			dest.at(j) = read_uint8(read<1>(src));
			j += 1;
		}

	}

	return j;

}



vector<Buffer> get_fabs(ifbuf & f) {
	// f -- input stream


	if (read_str(f, 8) != "MADSPACK") {
		throw Error("is not a madspack file");
	}

	f.seekg(14);  // absolute pos in stream

	auto _count = read_uint16(read<2>(f));

	vector<Buffer> _items;

	Buffer header = read_buf(f, 0xA0);
	auto iheader = imbuf(header);

	// read header
	for (int i = 0; i < _count; ++i) {

		auto hash_ = read_uint16(read<2>(iheader));
		auto size = read_uint32(read<4>(iheader));
		auto compressed_size = read_uint32(read<4>(iheader));

		vector<uint8> data;
		data.resize(size);

		//print("part=%||; pos=%||; size=%||; csize=%||; _count=%||\n", i, f.tellg(), size, compressed_size, _count);

		if (size == compressed_size) {
			// no compression on this entry
			data = read_buf(f, size);
		}
		else {
			// compressed

			auto dsize = fab_decompress(f, data);
			if (dsize != size) {
				throw Error("expecting %|| bytes of data, got %||", size, dsize);
			}
		}

		_items.push_back(data);
	}

	return _items;
}




void save_png(Path const& filename, Image & image, uint width, uint height, bool overwrite=false)
{

	bool exist = boost::filesystem::exists(filename);

	if (overwrite or !exist) {
		unsigned error = lodepng::encode(filename.c_str(), reinterpret_cast<uint8 const*>(&image.data[0]), width, height);
		if (error) {
			throw Error("lodepng encoder error %||: $||", error, lodepng_error_text(error));
		}
	}
}





void export_pallete(Pallete const& pal, Path const& dname) {
	// save pallete to 16x16 file (each color -> one pix)

	Image img(16,16);
	unsigned k = 0;

	for (int j = 0; j < img.h; ++j) {
		for (int i = 0; i < img.w; ++i) {
			img.at(i,j) = pal.at(k);
			++k;
		}
	}

	save_png(format("%||/pal.png", dname), img, 16, 16);
}



template <class B>
Pallete read_pallete(B & b, Path const& dname) {
	// Read pallete from fab part
	// return -- 256 * RGBA

	Pallete pal(256);

	for (size_t i = 0; i < 256; ++i) {
		// RGBA
		pal.at(i).at(0) = read_uint8(read<1>(b)) << 2;
		pal.at(i).at(1) = read_uint8(read<1>(b)) << 2;
		pal.at(i).at(2) = read_uint8(read<1>(b)) << 2;
		pal.at(i).at(3) = 255;

	}

	export_pallete(pal, dname);

	return pal;
}


Pallete get_default_pallete(Path const& fname) {

	auto dname = get_dname("VICEROY.PAL");

	auto dir = fname.parent_path();

	//print("pal_save_to = %||\n", (dir/"VICEROY.PAL").c_str());
	std::ifstream f((dir/"VICEROY.PAL").c_str());
	ifbuf wr(f);

	//imbuf wr2(read_buf(wr, 256*3));

	return read_pallete(wr, dname);
}



struct TileInfo{
	uint32 start_offset;
	uint32 length;
	uint16 width;
	uint16 height;
	uint16 width_padded;
	uint16 height_padded;
};







Image read_image_pik(TileInfo const& ti, Buffer const& data, Pallete const& pal) {
	Image img(ti.width, ti.height);

	auto k = ti.start_offset;
	for (int j = 0; j < img.h; ++j) {
		for (int i = 0; i < img.w; ++i) {
			auto col_ind = data.at(k);
			img.at(i,j) = pal.at(col_ind);
			k += 1;
		}
	}
	return img;
	//save_png(format("%||/pal.png", dname), buf, ti.width, ti.height);
}


Image read_image_ss(TileInfo const& ti, Buffer const& data, Pallete const& pal) {
	Image pix(ti.width, ti.height);

	unsigned i = 0;
	unsigned j = 0;
	unsigned k = ti.start_offset;
	uint8 bg = 0xFD;

	auto write_ind = [&i,&j,&pal,&pix,&bg](uint8 ci, uint32 l = 1) {
		auto c = pal.at(ci);

		if (ci == bg) {
			c[3] = 0;
		}

		while (l > 0) {
			pix.at(i,j) = c;
			i += 1;
			l -= 1;
		}
	};

	auto nextline = [&i,&j]() {
		i = 0;
		j += 1;
	};

	auto read = [&k, &data]() -> uint8 {
		uint8 r = data.at(k);
		k += 1;
		return r;
	};


	uint8 lm = read();
	while (1) {
		// line mode
		if (lm == 255) {
			write_ind(bg, ti.width - i);
			nextline();
			lm = read();
		}
		else if (lm == 252) {
			break;
		}
		else {
			uint8 x = read();
			if (x == 255) {
				write_ind(bg, ti.width - i);
				nextline();
				lm = read();
			}
			else {
				if (lm == 254) {
					// pix
					if (x == 254) {
						auto l = read();
						auto ci = read();
						write_ind(ci, l);
					}
					else {
						write_ind(x);
					}
				}
				else if (lm == 253) {
					auto ci = read();
					write_ind(ci, x);
				}
				else {
					throw Error("ERROR: unknown lm: %||\n", lm);
				}
			}
		}
	}

	return pix;
}








Image read_image(string const& fmt, TileInfo const& ti, Buffer const& data, Pallete const& pal) {
	if (fmt == ".PIK") {
		return read_image_pik(ti, data, pal);
	}
	else if (fmt == ".SS") {
		return read_image_ss(ti, data, pal);
	}
	else if (fmt == ".FF") {
		return read_image_ss(ti, data, pal);
	}
	else {
		throw Error("unknown format %||", fmt);
	}
}




vector<Buffer> unpack_mads(Path const& fname, Path const& dname) {
	std::ifstream f(fname.c_str(), std::ios::binary);

	ifbuf wr(f);
	auto parts = get_fabs(wr);

	//print("parts found in %||:\n", fname);

	int i = 0;
	for (auto & p: parts) {
		Path save_to = dname/Path(format("part%||.b", i));
		print("* part %||: size=%||\n", i, p.size());

		std::ofstream f(save_to.c_str(), std::ios::binary);
		f.write(reinterpret_cast<char*>(&p[0]), p.size());
		f.close();

		++i;
	}

	return parts;
}

template <class B>
TileInfo read_tileinfo(B & data) {
	TileInfo t;
	t.start_offset = read_uint32(read<4>(data));
	t.length = read_uint32(read<4>(data));
	t.width_padded = read_uint16(read<2>(data));
	t.height_padded = read_uint16(read<2>(data));
	t.width = read_uint16(read<2>(data));
	t.height = read_uint16(read<2>(data));
	return t;
}

vector<TileInfo> read_tileinfos(Buffer const& data) {
	auto n = size_t(data.size() / 16);

	vector<TileInfo> xs;
	xs.reserve(n);

	auto b = imbuf(data);
	for (size_t i = 0; i < n; ++i) {
		xs.push_back(
			read_tileinfo(b)
		);
	}
	return xs;
}

void unpack(Path const& fname) {

	print("Unpacking %||\n", fname);

	string ext = filesys::extension(fname);

	auto dname = get_dname(fname);

	auto parts = unpack_mads(fname, dname);

	// -------------------------------------------
	Pallete pal(256);

	vector<TileInfo> tis;

	Buffer * image_part = nullptr;

	if (ext == ".PIK") {
		auto head = read_pik_header(parts[0]);

		if (parts.size() > 2) {
			imbuf wr(parts.at(2));
			pal = read_pallete(wr, dname);
		}
		else {
			pal = get_default_pallete(fname);
		}

		image_part = &parts.at(1);

		TileInfo ti;
		ti.start_offset = 0;
		ti.length = parts.at(1).size();
		ti.width = head.width;
		ti.height = head.height;
		ti.width_padded = 0;
		ti.height_padded = 0;

		tis.push_back(ti);

		print("1 image found\n");
	}
	else if (ext == ".SS") {
		// header?

		tis = read_tileinfos(parts.at(1));

		imbuf ipart2(parts.at(2));
		pal = read_pallete(ipart2, dname);

		image_part = &parts.at(3);


		print("%|| image(s) found\n", tis.size());

	}
	else {
		throw Error("unknown extension %||", ext);
	}

	// ------------------------------

	for (size_t i = 0; i < tis.size(); ++i) {
		auto & ti = tis.at(i);

		Path xxx_png = format("%03i.png", i+1);
		Path ofile = dname / xxx_png;

		bool exist = boost::filesystem::exists(ofile);

		string status = exist ? "skip_exist" : "written";

		print("* tile %||; dim=(%||,%||); path=%||; %||\n", i+1, ti.width, ti.height, dname.filename() / xxx_png, status);

		if (!exist) {
			auto img = read_image(ext, ti, *image_part, pal);

			save_png(ofile.c_str(), img, img.w, img.h);
		}

	}

	print("\n");
}



int main(int argc, char *argv[]) {
	if (argc != 2) {
		print("Usage: %|| <path-to-colonize-dir>\n", argv[0]);
		return 1;
	}
	
	Path col_dir(argv[1]);

	vector<string> colnames = {
		"WOODPANL.PIK",
		"WOODTILE.SS",
		"WOODPAN2.PIK",
		"WOODFRAM.SS",
		"TERRAIN.SS",
		"PHYS0.SS",
		"PARCH.SS",
		"NAMEPLAT.SS",
		"ICONS.SS",
		"CURSOR.SS",
		"COLONY.PIK",
		"CCBKGD.PIK",
		"BUILDING.SS",
	};

	try {
		for (auto& cname: colnames) {
			unpack(col_dir / Path(cname));
		}

		filesys::path dname;
		dname /= os::get_home_dir();
		dname /= ".cc94";
		dname /= "COLONIZE";

		print("NOTE: unpacked data location: %||\n", dname);
	}
	catch (Error const& e) {
		print("ERROR: %||\n", e.what());
		return 1;
	}

	return 0;
}

