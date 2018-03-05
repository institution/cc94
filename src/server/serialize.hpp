#pragma once

#include <fstream>

#include "col.hpp"
#include "env.hpp"
#include "control.hpp"
#include "objs.hpp"


namespace col {

	template <class E>
	auto enum_to_integer(E e) -> typename std::underlying_type<E>::type {
		return static_cast<typename std::underlying_type<E>::type>(e);
	}
	 
	template <class E, class T>
	auto integer_to_enum(T t) -> E {
		return static_cast<E>(t);
	}

	uint32_t const current_version = 1;
	
	
	using Byte = uint8_t;
	
	template<typename A>
	Byte read_byte(A & ar) {
		Byte b;
		ar.read((char *)&b, sizeof(Byte));
		return b;
	}

	template<typename A>
	void write_byte(A & ar, Byte const& t) {		
		ar.write((char const*)&t, sizeof(Byte));
	}
	
	

	
	/*template<class T, class A>
	void write_block(A & f, Mark mark, T const& obj) 
	{
		write(f, mark);
    
		uint32_t p0 = f.tell();
		write_uint32(f, 0);
		
		write(f, obj);
    
		uint32_t p1 = f.tell()
		f.seek(p0);
		write_uint32(p1 - p0 - 4);
		f.seek(p1);    
	}
	
	
	template<class T, class A>
	void read_block(A & f, char const* block_id, T & obj) 
	{
		auto id = read_string(f);
		if (id != block_id) {
			fail("bad block id; got='%||'; expected='%||'\n", id, block_id);
		}
    
		uint32_t len = read_uint32(f);
		uint32_t p0 = f.tell();
		
		read(f, obj);
    
		uint32_t p1 = f.tell()
		if (p1 - p0 != len) {
			fail("block length incorrect; got='%||'; expected='%||'\n", p1 - p0, len);
		}
	}*/
	
	
	template<class T, typename A>
	void read(A & ar, T & r) 
	{
		T t;
		r = 0;
		uint16_t i = 0;
		
		// use only for fixed size types acr platforms like: uint16_t
		uint16_t size = sizeof(T);  
		
		//print("aaa = %||\n", ar.tellg());
		
		while (i < size*8) {
			t = read_byte(ar);
			r = r | (t << i);
			i += 8;
		}
	}

	template<class T, typename A>
	void write(A & ar, T const& t) 
	{
		// use with: int16_t uint32_t -- only types with same size on all platforms
		unsigned i = 0;
		
		while (i < sizeof(T)*8) {
			write_byte(ar, (t >> i) & 0xFF);
			i += 8;
		}
		
	}
	
		
	
	
	
	
		
	
		
	template<typename A>
	void read(A & ar, std::string & t) {
		std::string::size_type n;
		read(ar, n);
		t.resize(n);
		for (std::string::size_type i=0; i<n; ++i) {
			read(ar, t[i]);
		}
	}

	template<typename A>
	void write(A & ar, std::string const& t) {		
		write(ar, t.size());
		for (auto c: t) {
			write(ar, c);
		}
	}

	template<typename E, typename A>
	void read_enum(A & ar, E & e) {
		using Integer = typename std::underlying_type<E>::type;
		Integer k;
		read(ar, k);		
		e = integer_to_enum<Class>(k);		
	}

	template<typename E, typename A>
	void write_enum(A & ar, E const& e) {
		write(ar, enum_to_integer(e));		
	}


	template <class T, class A>
	void write(A & ar, aga2::Mv1<T> const& c)
	{
		write(ar, c[0]);
		write(ar, c[1]);		
	}


	template <class T, class A>
	void read(A & ar, aga2::Mv1<T> & c)
	{		
		read(ar, c[0]);
		read(ar, c[1]);
	}



	template<typename A>
	void read(A & ar, Env const& env, col::Task & t) {
		read(ar, t.progress);
		
		Class cls;       read(ar, cls);
		Makeable::Id id; read(ar, id);
				
		switch (cls) {
			case ClassNone:
				t.what = nullptr;
				break;
			case ClassBuildType:
				t.what = &env.get_buildtype(id);
				break;
			case ClassUnitType:
				t.what = &env.get_buildtype(id);
				break;
			default:
				assert(0);
				break;
		}		
	}

	template<typename A>
	void write(A & ar, Env const& env, col::Task const& t) {
		write(ar, t.progress);
		if (t) {			
			write(ar, t.what->get_class());
			write(ar, t.what->get_id());
		}
		else {
			write(ar, ClassNone);
			write(ar, Makeable::Id(0));
		}
	}
				



	template<typename T, typename A>
	T read(A & ar) {
		T tmp;		
		read(ar, tmp);
		return tmp;
	}

	template<typename A>
	void read_mark(A & f, char const* expect)
	{
		auto m = read<Mark>(f);
		if (m != make_mark(expect)) {
			fail("read_mark: expected %||; fpos=%||\n", expect, f.tellg());
		}
		else {
			print("read_mark: mark=%|| at fpos=%||\n", expect, f.tellg());
		}
	}
	
	template<typename A>
	void write_mark(A & f, char const* c)
	{
		write<Mark>(f, make_mark(c));		
	}

	

	template<typename K, typename T, typename Archive>
	void write_map(Archive & ar, std::unordered_map<K, T> const& xs) {
		write_mark(ar, "MAP=");
		write(ar, xs.size());
		for (auto & x: xs) {
			write(ar, x.first);
			write(ar, x.second);
		}		
		write_mark(ar, "END=");
	}

	template<typename K, typename T, typename Archive>
	void read_map(Archive & ar, std::unordered_map<K, T> & xs) {
		using Size = decltype(xs.size());
		
		read_mark(ar, "MAP=");
		auto size = read<Size>(ar);
		for (Size i = 0; i < size; ++i) {			
			auto key = read<K>(ar);
			auto val = read<T>(ar);
			xs[key] = val;
		}
		read_mark(ar, "END=");
	}
	
	
	// Register/Storage
	
	template<typename T, typename Archive>
	void write_reg(Archive & ar, T const& r) {
		write(ar, r.cas.size());
		for (auto & ca: r.cas) {
			write(ar, ca.item);
			write(ar, ca.amount);
		}
	}

	template<typename T, typename Archive>
	void read_reg(Archive & ar, T & r) {
		using Size = decltype(r.size());
				
		auto size = read<Size>(ar);
		for (Size i = 0; i < size; ++i) {
			Item item;
			Amount amount;
			read(ar, item);
			read(ar, amount);
						
			r.find_mod(item).amount = amount;			
		}
	}

	template<typename A>
	void write(A & ar, col::Env const& env, col::Faction const& x) {
		write(ar, x.id);
		write(ar, x.name);
	}

	template<typename A>
	void write(A & ar, col::Env const& env, col::Unit const& x) {
		// unit
		write(ar, x.id);
		write(ar, x.type->id);
		write(ar, x.control);
		//write(ar, x.space_left);
		write(ar, x.time_left);
		write(ar, x.prof_dir);
		write(ar, x.prof_num);
		

		// unit terr
		write(ar, env.get_coords(*x.terr));

		// unit build
		auto wp = x.workplace;
		if (x.terr and wp and wp->place_type() == PlaceType::Build) {
			write<int>(ar, x.terr->get_colony().get_build_index(*(Build*)x.workplace) + 1);
		}
		else {
			write<int>(ar, 0);
		}

		// unit field
		if (x.terr and wp and wp->place_type() == PlaceType::Field) {
			write<int>(ar, x.terr->get_colony().get_field_index(*(Field*)x.workplace) + 1);
		}
		else {
			write<int>(ar, 0);
		}


		// unit cargo
		write_reg(ar, x.store);

	}



	template<typename A>
	void read(A & ar, col::Env & env, col::Faction & x) {
		read(ar, x.id);
		read(ar, x.name);		
	}



	template<typename A>
	void read_unit(A & ar, col::Env & env, col::Unit & unit) {

		read(ar, unit.id);
		unit.type = & env.get_unittype( read<col::UnitType::Id> (ar) );
		read(ar, unit.control);
		read(ar, unit.time_left);
		read(ar, unit.prof_dir);
		read(ar, unit.prof_num);

		// terr
		env.put(env.get_terr(read<col::Coords>(ar)), unit);

		auto build_id = read<int>(ar);
		auto field_id = read<int>(ar);

		if (build_id) {
			env.work_build(build_id - 1, unit);
		}

		if (field_id) {
			env.work_field(field_id - 1, unit);
		}

		// unit cargo
		read_reg(ar, unit.store);

		assert(unit.type != nullptr);

		unit.space_left = unit.get_space();		
		unit.space_left -= unit.store.total();		
	}


	template<typename A>
	void write(A & ar, Env const& env, Terr const& x) {
		write(ar, x.biome);
		write(ar, x.phys);
		write(ar, x.alt);
		write(ar, x.discovered);
	}

	template<typename A>
	void read(A & ar, Env const& env, Terr & x) {
		read(ar, x.biome);
		read(ar, x.phys);
		read(ar, x.alt);
		read(ar, x.discovered);
	}





	template<class Archive>
    void write(Archive & ar, Env const& env)
	{
		auto ver = current_version;

		auto verbose = 0;

		// version
		write(ar, ver);

		// nations
		write_mark(ar, "FACS");
		{
			if (verbose) print("save nations\n");
			
			auto & ps = env.get_cont<Faction>();
			
			// num of nations
			write(ar, ps.size());
			for (auto& p: ps) {
				// nation id
				write(ar, p.first);
				// nation data
				write(ar, env, p.second);
			}
		}

		// terrain
		write_mark(ar, "TERS");
		{
			if (verbose) print( "save terrain\n");
			
			
			write(ar, env.dim);
			

			for(Coord j = 0; j < env.dim[1]; ++j) {
				for(Coord i = 0; i < env.dim[0]; ++i) {
					auto& x = env.get_terr({i,j});
					// terr value
					write(ar, env, x);

				}
			}
		}

		// colonies
		write_mark(ar, "COLS");
		{
			if (verbose) print("save colonies\n");
						
			// write_byte(ar, 'C');
			
			auto& ps = env.get_cont<Colony>();
			
			write(ar, ps.size());
			for (auto& p: ps) {
				auto& x = p.second;

				if (verbose) print("save colony name = %||\n", x.name);

				// colony
				write(ar, x.id);
				write(ar, x.name);
				write_reg(ar, x.store);

				// buildings
				for (auto& b: x.builds) {
					if (verbose) print("save building type id = %||\n", b.type->id);
					
					// build
					write(ar, b.type->id);
					write(ar, b.free_slots);
					
					// task
					write(ar, env, b.task);

				}

				// fields
				write(ar, x.fields.size());
				for (auto& f: x.fields) {
					// field
					write(ar, env.get_coords(f.get_terr()));
					write(ar, f.get_proditem());
				}

				// location
				auto cr = env.get_coords(env.get_terr(x));
				write(ar, cr);

			}
		}

		// units	
		write_mark(ar, "UNIS");	
		{
			if (verbose) print("save units\n");
			
			auto& ps = env.get_cont<Unit>();
			write(ar, ps.size());
			for (auto& p: ps) {
				auto& x = p.second;
				write(ar, p.first);
				write(ar, env, x);
			}
		}

		write_mark(ar, "MISC");
		if (verbose) print("save misc\n");
		// turn info
		write(ar, env.turn_no);
		// next id
		write(ar, env.next_id);
		// state
		write(ar, env.state);
		// current control
		write(ar, env.get_current_control());
		
	}

    template<class Archive>
    void read(Archive & ar, Env & env)
	{
		auto verbose = env.verbose;		
		auto ver = current_version;

		// version
		read(ar, ver);

		// factions
		read_mark(ar, "FACS");
		{
			if (verbose) print("load factions\n");
			
			auto& ps = env.get_cont<Faction>();
			
			auto num = read<size_t>(ar);
			for (size_t i=0; i<num; ++i) {
				auto id = read<Faction::Id>(ar);
				read(ar, env, ps[id]);
			}
		}

		// terrain
		read_mark(ar, "TERS");
		{
			if (verbose) print("load terrain\n");
			
			print("offset = %||\n", ar.tellg());
			read(ar, env.dim);

			
			print("map dim = %||, %||\n", env.dim[0], env.dim[1]);

			env.resize(env.dim);

			for(Coord j = 0; j < env.dim[1]; ++j) {
				for(Coord i = 0; i < env.dim[0]; ++i) {

					auto& x = env.get_terr(Coords(i,j));

					// terr value
					read(ar, env, x);
				}
			}
		}

		

		// colonies
		read_mark(ar, "COLS");
		{
			// section mark
			/*if (read_byte(ar) != 'C') {
				fail('invalid section mark: expected C (Colonies)');
			}*/
		
			if (verbose) print("load colonies\n");
			
			auto& ps = env.get_cont<Colony>();
			
			auto tmp = read<decltype(ps.size())>(ar);
			for (decltype(ps.size()) i=0; i<tmp; ++i) {

				Colony x;
				read(ar, x.id);
				read(ar, x.name);
				read_reg(ar, x.store);

				auto key = x.id;
				auto p = ps.emplace(key, std::move(x)).first;

				auto& col = (*p).second;

				print("loaded colony name = %||\n", col.name);

				// buildings
				for (auto& b: col.builds) {
					auto build_type_id = read<BuildType::Id>(ar);
					//cerr << "load building type id = " << build_type_id << endl;

					b.type = & env.get_buildtype( build_type_id );
					read(ar, b.free_slots);

					// task
					read(ar, env, b.task);
					
				}

				// fields
				auto nfields = read<Colony::Fields::size_type>(ar);
				for (Colony::Fields::size_type i = 0; i < nfields; ++i) {
					// field
					col.add_field(
						Field(env.get_terr(read<Coords>(ar)))
					);

					col.fields[i].set_proditem(read<Item>(ar));

				}


				// location
				env.put(env.get_terr(read<Coords>(ar)), (*p).second);

			}
		}


		// units
		read_mark(ar, "UNIS");
		{
			if (verbose) print("load units\n");
			
			auto& units = env.get_cont<Unit>();
			size_t tmp;
			read(ar, tmp);
			for (size_t i = 0; i < tmp; ++i) {
				auto unit_id = read<Unit::Id>(ar);
				read_unit(ar, env, units[unit_id]);
			}
		}

		if (verbose) print("load misc\n");
		
		read_mark(ar, "MISC");
		// turn info
		read(ar, env.turn_no);
		// next id
		read(ar, env.next_id);
		// state
		read(ar, env.state);
		// current control
		env.set_current_control(read<Control>(ar));
	
		// current nation
		//l += write(f, env.current_nation_id);

	}



	inline void write_file(std::string const& fname, Env & env) {
		std::ofstream f(fname, std::ios::binary);
		write(f, env);
	}
    
    inline void read_file(std::string const& fname, Env & env) {
		std::ifstream f(fname, std::ios::binary);
		read(f, env);
	}

}
