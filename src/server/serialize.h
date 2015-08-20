#ifndef SERIALIZE_H_3264367
#define SERIALIZE_H_3264367

#include <fstream>
#include "env.h"



namespace col {

	uint const current_version = 1;
	
	template<typename T, typename A,
		typename std::enable_if<!std::is_pointer<T>::value>::type* = nullptr
	>
	void read(A & ar, T & t) {
		ar.read(reinterpret_cast<char*>(&t), sizeof(t));
	}

	template<typename T, typename A,
		typename std::enable_if<!std::is_pointer<T>::value>::type* = nullptr	
	>
	void write(A & ar, T const& t) {		
		ar.write(reinterpret_cast<char const*>(&t), sizeof(t));
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

	template<typename D, typename T, typename A>
	void read(A & ar, distinct::Enum<D, T> & e) {
		T id;
		read(ar, id);
		e = distinct::Enum<D, T>(id);		
	}

	template<typename D, typename T, typename A>
	void write(A & ar, distinct::Enum<D, T> const& e) {
		write(ar, T(e));		
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
	void read(A & ar, Color & c) {
		read(ar, c.r);
		read(ar, c.g);
		read(ar, c.b);
		read(ar, c.a);
	}

	template<typename A>
	void write(A & ar, Color const& c) {
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
	

	template<typename A>
	void write(A & ar, Env const& env, Nation const& x) {
		write(ar, x.id);
		write(ar, x.name);
		write(ar, x.color);
		write(ar, x.flag_id);
	}

	template<typename A>
	void write(A & ar, Env const& env, Unit const& x) {
		// unit
		write(ar, x.id);
		write(ar, x.type->id);
		write(ar, x.nation->id);
		//write(ar, x.space_left);
		write(ar, x.time_left);
		write(ar, x.transported);

		// unit terr
		write(ar, env.get_coords(*x.terr));

		// unit build
		if (x.terr and x.build) {
			write<int>(ar, x.terr->get_colony().get_build_index(*x.build) + 1);
		}
		else {
			write<int>(ar, 0);
		}

		// unit field
		if (x.terr and x.field) {
			write<int>(ar, x.terr->get_colony().get_field_index(*x.field) + 1);
		}
		else {
			write<int>(ar, 0);
		}

		// unit cargo
		write_map(ar, x.get_cargos());

	}



	template<typename A>
	void read(A & ar, Env & env, Nation & x) {
		read(ar, x.id);
		read(ar, x.name);
		read(ar, x.color);
		read(ar, x.flag_id);
	}



	template<typename A>
	void read_unit(A & ar, Env & env, Unit & unit) {

		read(ar, unit.id);
		unit.type = & env.get<UnitType> ( read<UnitType::Id> (ar) );
		unit.nation = & env.get<Nation> ( read<Nation::Id> (ar) );
		read(ar, unit.time_left);
		read(ar, unit.transported);

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

		// unit cargo
		read_map(ar, unit.get_cargos());

		assert(unit.type != nullptr);

		unit.space_left = unit.get_space();
		for (auto& p: unit.get_cargos()) {
			unit.space_left -= p.second;
		}

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
	}

	template<typename A>
	void read(A & ar, Env const& env, Terr const& x) {
		read(ar, x.biome);
		read(ar, x.phys);
		read(ar, x.alt);
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
			
			auto& ps = env.get_cont<Colony>();
			
			write(ar, ps.size());
			for (auto& p: ps) {
				auto& x = p.second;

				if (verbose) print("save colony name = %||\n", x.name);

				// colony
				write(ar, x.id);
				write(ar, x.name);
				write_map(ar, x.store.cargos);

				// buildings
				for (auto& b: x.builds) {
					if (verbose) print("save building type id = %||\n", b.type->id);
					
					// build
					write(ar, b.type->id);
					write(ar, b.free_slots);
					if (ver > 0) write(ar, b.hammers);

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
			
			read(ar, env.w);
			read(ar, env.h);

			env.resize({env.w, env.h});

			for(Coord j = 0; j < env.h; ++j) {
				for(Coord i = 0; i < env.w; ++i) {

					auto& x = env.get_terr(Coords(i,j));

					// terr value
					read(ar, x.biome);
					read(ar, x.phys);
					read(ar, x.alt);

				}
			}
		}

		// colonies
		{
			if (verbose) print("load colonies\n");
			
			auto& ps = env.get_cont<Colony>();
			
			auto tmp = read<decltype(ps.size())>(ar);
			for (decltype(ps.size()) i=0; i<tmp; ++i) {

				Colony x;
				read(ar, x.id);
				read(ar, x.name);
				read_map(ar, x.store.cargos);

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
					if (ver > 0) read(ar, b.hammers);
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

#endif
