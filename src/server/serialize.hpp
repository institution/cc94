#pragma once

#include <fstream>

#include "col.hpp"
#include "env.hpp"
#include "nation.hpp"
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
	
	
	
	
		
	
	
	template<class T, typename A>
	void read(A & ar, T & r) {
		T t;
		r = 0;
		unsigned i = 0;
		
		// read size, int has diffrent size on diffrent platforms
		auto size = read_byte(ar);  
		
		while (i < size*8) {
			t = read_byte(ar);
			r = r | (t << i);
			i += 8;
		}
	}

	template<class T, typename A>
	void write(A & ar, T const& t) {		
		unsigned i = 0;
		
		// write size, int has diffrent size on diffrent platforms
		write_byte(ar, uint8_t(sizeof(T)));
		
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
				t.what = &env.get<BuildType>(id);
				break;
			case ClassUnitType:
				t.what = &env.get<UnitType>(id);
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
				


	template<typename A>
	void read(A & ar, col::NationColor & c) {
		read(ar, c.r);
		read(ar, c.g);
		read(ar, c.b);
		read(ar, c.a);
	}

	template<typename A>
	void write(A & ar, col::NationColor const& c) {
		write(ar, c.r);
		write(ar, c.g);
		write(ar, c.b);
		write(ar, c.a);
	}


	template<typename T, typename A>
	T read(A & ar) {
		T tmp;		
		read(ar, tmp);
		return tmp;
	}

	

	template<typename K, typename T, typename Archive>
	void write_map(Archive & ar, std::unordered_map<K, T> const& xs) {
		write(ar, xs.size());
		for (auto & x: xs) {
			write(ar, x.first);
			write(ar, x.second);
		}
	}

	template<typename K, typename T, typename Archive>
	void read_map(Archive & ar, std::unordered_map<K, T> & xs) {
		using Size = decltype(xs.size());
		
		auto size = read<Size>(ar);
		for (Size i = 0; i < size; ++i) {			
			auto key = read<K>(ar);
			auto val = read<T>(ar);
			xs[key] = val;
		}
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
	void write(A & ar, col::Env const& env, col::Nation const& x) {
		write(ar, x.id);
		write(ar, x.name);
		write(ar, x.color);
		write(ar, x.flag_id);
	}

	template<typename A>
	void write(A & ar, col::Env const& env, col::Unit const& x) {
		// unit
		write(ar, x.id);
		write(ar, x.type->id);
		write(ar, x.nation->id);
		//write(ar, x.space_left);
		write(ar, x.time_left);
		write(ar, x.transported);		
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
	void read(A & ar, col::Env & env, col::Nation & x) {
		read(ar, x.id);
		read(ar, x.name);
		read(ar, x.color);
		read(ar, x.flag_id);
	}



	template<typename A>
	void read_unit(A & ar, col::Env & env, col::Unit & unit) {

		read(ar, unit.id);
		unit.type = & env.get<UnitType> ( read<col::UnitType::Id> (ar) );
		unit.nation = & env.get<Nation> ( read<col::Nation::Id> (ar) );
		read(ar, unit.time_left);
		read(ar, unit.transported);
		read(ar, unit.prof_dir);
		read(ar, unit.prof_num);

		// terr
		env.init(env.get_terr(read<col::Coords>(ar)), unit);

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
	void read(A & ar, Env & env, Unit & x) {

		read(ar, x.id);
		x.type = & env.get<UnitType> ( read<UnitType::Id> (ar) );
		x.nation = & env.get<Nation> ( read<Nation::Id> (ar) );
		read(ar, x.space_left);
		read(ar, x.time_left);
		read(ar, x.transported);

		auto& unit = x;

		// terr
		env.init(env.get_terr(read<Coords>(ar)), unit);

		auto build_id = read<int>(ar);
		auto field_id = read<int>(ar);

		if (build_id) {
			env.work_build(build_id - 1, unit);
		}

		if (field_id) {
			env.work_field(field_id - 1, unit);
		}

		assert(unit.type != nullptr);
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
		{
			if (verbose) print("save nations\n");
			
			auto & ps = env.get_cont<Nation>();
			
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
		{
			if (verbose) print( "save terrain\n");
			
			
			write(ar, env.w);
			write(ar, env.h);

			for(Coord j = 0; j < env.h; ++j) {
				for(Coord i = 0; i < env.w; ++i) {
					auto& x = env.get_terr({i,j});
					// terr value
					write(ar, env, x);

				}
			}
		}

		// colonies		
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

		if (verbose) print("save misc\n");
		// turn info
		write(ar, env.turn_no);
		// next id
		write(ar, env.next_id);
		// state
		write(ar, env.state);
		// current nation
		if (env.state == 1) {
			if (verbose) print("save current nation id = %||\n", env.get_current_nation().id);			
			write(ar, env.get_current_nation().id);
		}
		else {
			if (verbose) print("save current nation INV STATE: %||\n", env.state);
			write<Nation::Id>(ar, -1);
		}

	}

    template<class Archive>
    void read(Archive & ar, Env & env)
	{
		auto verbose = env.verbose;		
		auto ver = current_version;

		// version
		read(ar, ver);

		// nations
		{
			if (verbose) print("load nations\n");
			
			auto& ps = env.get_cont<Nation>();
			
			auto num = read<size_t>(ar);
			for (size_t i=0; i<num; ++i) {
				auto id = read<Nation::Id>(ar);
				read(ar, env, ps[id]);
			}
		}

		// terrain
		{
			if (verbose) print("load terrain\n");
			
			print("offset = %||\n", ar.tellg());
			read(ar, env.w);
			read(ar, env.h);
			
			print("map dim = %||, %||\n", env.w, env.h);

			env.resize({env.w, env.h});

			for(Coord j = 0; j < env.h; ++j) {
				for(Coord i = 0; i < env.w; ++i) {

					auto& x = env.get_terr(Coords(i,j));

					// terr value
					read(ar, env, x);
				}
			}
		}

		

		// colonies
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

					b.type = & env.get<BuildType>( build_type_id );
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
				env.init(env.get_terr(read<Coords>(ar)), (*p).second);

			}
		}


		// units
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
		
		// turn info
		read(ar, env.turn_no);
		// next id
		read(ar, env.next_id);
		// state
		read(ar, env.state);
		// current nation
		if (env.state == 1) {
			env.set_current_nation(
				env.get<Nation>(
					read<Nation::Id>(ar) 
				) 
			);
		}
		else {
			read<Nation::Id>(ar);
		}

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
