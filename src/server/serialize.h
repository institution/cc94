#ifndef SERIALIZE32_H
#define SERIALIZE32_H

#include <boost/serialization/map.hpp>
#include "env.h"



//namespace boost {namespace serialization {



namespace boost {
	namespace serialization {

		template<class A>
		void serialize(A & ar, ::col::Coords & c, uint const version)
		{
			ar & c[0];
			ar & c[1];
		}

	} // namespace serialization
} // namespace boost



namespace col {

	uint const current_version = 1;





	template<typename T, typename Archive>
	void save_cont(Archive & ar, Env const& env) {
		auto& xs = env.get_cont<T>();
		ar << xs.size();
		for (auto& x: xs) {
			ar << x.first;
			ar << x.second;
		}
	}

	template<typename T, typename Archive>
	void load_cont(Archive & ar, Env & env) {
		auto& xs = env.get_cont<T>();
		using size_t = decltype(xs.size());
		size_t l;
		ar & l;
		for (size_t i=0; i<l; ++i) {
			//typename std::remove_reference<decltype(xs)>::type::key_type key;
			typename T::Id key;
			ar & key;
			/*xs.emplace(piecewise_construct,
				forward_as_tuple(key),
				forward_as_tuple(Reserve())
			);*/
			Nation p(key);
			ar & p;
			xs.emplace(key,
					std::move(p)
			);
		}
	}

	template<typename T, typename A>
	T read(A & ar) {
		T tmp;
		ar >> tmp;
		return tmp;
	}

	template<typename T, typename A>
	void read(A & ar, T & t) {
		ar >> t;
	}

	template<typename T, typename A>
	void write(A & ar, T const& t) {
		ar << t;
	}

	template<typename T, typename Archive>
	void write_map(Archive & ar, T const& xs) {
		write(ar, xs.size());
		for (auto & x: xs) {
			ar << x.first;
			ar << x.second;
		}
	}

	template<typename T, typename Archive>
	void read_map(Archive & ar, T & xs) {
		using Size = decltype(xs.size());
		using Key = typename T::key_type;
		using Val = typename T::mapped_type;

		auto size = read<Size>(ar);
		for (Size i = 0; i < size; ++i) {
			auto key = read<Key>(ar);
			auto val = read<Val>(ar);
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
		ar << x.biome;
		ar << x.phys;
		ar << x.alt;
	}

	template<typename A>
	void read(A & ar, Env const& env, Terr const& x) {
		ar >> x.biome;
		ar >> x.phys;
		ar >> x.alt;
	}





	template<class Archive>
    void Env::save(Archive & ar, uint const& ver_) const
	{
		auto ver = current_version;

		auto& env = *this;
		auto verbose = 0;


		// save version
		ar << ver;

		// nations
		if (verbose) cerr << "save nations" << endl;
		{
			auto& ps = env.get_cont<Nation>();
			auto tmp = ps.size();

			// num of nations
			write(ar, tmp);
			for (auto& p: ps) {
				auto& x = p.second;
				auto& id = p.first;

				// nation id
				write(ar, id);

				// nation data
				write(ar, env, x);
			}
		}

		// terrain
		if (verbose) cerr << "save terrain" << endl;
		{
			ar << env.w;
			ar << env.h;

			for(Coord j = 0; j < env.h; ++j) {
				for(Coord i = 0; i < env.w; ++i) {
					auto& x = env.get_terr(Coords(i,j));

					//cerr << Coords(i,j) << endl;

					// terr value
					write(ar, env, x);

				}
			}
		}

		// colonies
		if (verbose) cerr << "save colonies" << endl;
		{
			auto& ps = env.get_cont<Colony>();
			auto tmp = ps.size();
			ar << tmp;
			for (auto& p: ps) {
				auto& x = p.second;

				if (verbose) cerr << "save colony name = " << x.name << endl;

				// colony
				ar << x.id;
				ar << x.name;
				write(ar, x.store.cargos);

				// buildings
				for (auto& b: x.builds) {
					if (verbose) cerr << "save building type id = " << b.type->id << endl;
					// build
					ar << b.type->id;
					ar << b.free_slots;
					if (ver > 0) ar << b.hammers;

				}

				// fields
				write(ar, uint(x.fields.size()));
				for (auto& f: x.fields) {
					// field
					write(ar, env.get_coords(f.get_terr()));
					write(ar, f.get_proditem());

				}

				// location
				auto cr = env.get_coords(env.get_terr(x));
				ar << cr;

			}
		}

		// units
		if (verbose) cerr << "save units" << endl;
		{
			auto& ps = env.get_cont<Unit>();
			auto tmp = ps.size();
			ar << tmp;
			for (auto& p: ps) {
				auto& x = p.second;
				write(ar, p.first);
				write(ar, env, x);

			}
		}

		if (verbose) cerr << "save misc" << endl;
		// turn info
		ar << env.turn_no;
		// next id
		ar << next_id;
		// state
		ar << state;
		// current nation
		if (env.state == 1) {
			if (verbose) cerr << "save current nation" << env.get_current_nation().id << endl;
			ar << env.get_current_nation().id;
		}
		else {
			if (verbose) cerr << "save current nation INV STATE: " << env.state << endl;
			write<Nation::Id>(ar, -1);
		}



	}

    template<class Archive>
    void Env::load(Archive & ar, uint const& ver_)
	{
		auto& env = *this;
		auto verbose = env.verbose;
		auto ver = current_version;

		// version
		ar >> ver;

		// nations
		if (verbose) {
			cerr << "load nations" << endl;
		}

		{
			auto& ps = env.get_cont<Nation>();
			size_t tmp;
			ar >> tmp;
			for (size_t i=0; i<tmp; ++i) {

				Nation::Id id;
				read(ar, id);

				//Nation x;
				read(ar, env, ps[id]);

				//ps.emplace(id, std::move(x));
			}
		}

		// terrain
		if (verbose) {
			cerr << "load terrain" << endl;
		}

		{
			ar >> env.w;
			ar >> env.h;

			env.resize({env.w, env.h});

			for(Coord j = 0; j < env.h; ++j) {
				for(Coord i = 0; i < env.w; ++i) {

					auto& x = env.get_terr(Coords(i,j));

					//cerr << Coords(i,j) << endl;

					// terr value
					ar >> x.biome;
					ar >> x.phys;
					ar >> x.alt;

				}
			}
		}

		// colonies
		if (verbose) {
			cerr << "load colonies" << endl;
		}

		{
			auto& ps = env.get_cont<Colony>();
			size_t tmp;
			ar >> tmp;
			for (size_t i=0; i<tmp; ++i) {

				Colony x;
				ar >> x.id;
				ar >> x.name;
				read(ar, x.store.cargos);

				auto key = x.id;
				auto p = ps.emplace(key, std::move(x)).first;

				auto& col = (*p).second;

				cerr << "loaded colony name = " << col.name << endl;

				// buildings
				for (auto& b: col.builds) {
					auto build_type_id = read<BuildType::Id>(ar);
					cerr << "load building type id = " << build_type_id << endl;

					b.type = & env.get<BuildType>( build_type_id );
					ar >> b.free_slots;
					if (ver > 0) ar >> b.hammers;
				}

				// fields
				auto nfields = read<size_t>(ar);
				for (size_t i = 0; i < nfields; ++i) {
					// field
					col.add_field(
						Field(env.get_terr(read<Coords>(ar))));

					col.fields[i].set_proditem(read<Item>(ar));

				}


				// location
				env.init(env.get_terr(read<Coords>(ar)), (*p).second);

			}
		}


		// units
		if (verbose) {
			cerr << "load units" << endl;
		}

		{
			auto& units = env.get_cont<Unit>();
			size_t tmp;
			ar >> tmp;
			for (size_t i = 0; i < tmp; ++i) {
				auto unit_id = read<Unit::Id>(ar);
				read_unit(ar, env, units[unit_id]);
			}
		}

		if (verbose) {
			cerr << "load misc" << endl;
		}
		// turn info
		ar >> env.turn_no;
		// next id
		ar >> env.next_id;
		// state
		ar >> env.state;
		// current nation
		if (env.state == 1) {
			env.set_current_nation(
				env.get<Nation>(
					read<Nation::Id>(ar) ) );
		}
		else {
			read<Nation::Id>(ar);
		}

		// current nation
		//l += write(f, env.current_nation_id);

	}





}

#endif
