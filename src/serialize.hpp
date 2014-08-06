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
			Player p(key);
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

	template<typename A>
	void write(A & ar, Env const& env, Unit const& x) {
		// unit
		write(ar, x.id);
		write(ar, x.type->id);
		write(ar, x.player->id);
		write(ar, x.space_left);
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
	}


	template<typename A>
	void read_unit(A & ar, Env & env) {
		auto& ps = env.get_cont<Unit>();


		Unit x;
		read(ar, x.id);
		x.type = & env.get<UnitType> ( read<UnitType::Id> (ar) );
		x.player = & env.get<Player> ( read<Player::Id> (ar) );
		read(ar, x.space_left);
		read(ar, x.time_left);
		read(ar, x.transported);

		auto key = x.id;
		auto p = ps.emplace(key, std::move(x)).first;


		auto& unit = (*p).second;

		// terr
		env.init(unit, env.get_terr(read<Coords>(ar)));

		auto build_id = read<int>(ar);
		auto field_id = read<int>(ar);

		if (build_id) {
			env.work_build(build_id - 1, unit);
		}

		if (field_id) {
			env.work_field(field_id - 1, unit);
		}
	}





	template<class Archive>
    void Env::save(Archive & ar, uint const& ver_) const
	{
		auto ver = current_version;

		auto& env = *this;
		auto verbose = 0;


		// save version
		ar << ver;

		// players
		if (verbose) cerr << "save players" << endl;
		{
			auto& ps = env.get_cont<Player>();
			auto tmp = ps.size();
			ar << tmp;
			for (auto& p: ps) {
				auto& x = p.second;

				// player
				ar << x.id;
				ar << x.name;
				ar << x.color;
				ar << x.flag_id;
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
					ar << x.biome;
					ar << x.phys;
					ar << x.alt;

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
				ar << x.storage;

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
		// current player
		if (env.state == 1) {
			if (verbose) cerr << "save current player" << env.get_current_player().id << endl;
			ar << env.get_current_player().id;
		}
		else {
			if (verbose) cerr << "save current player INV STATE: " << env.state << endl;
			write<Player::Id>(ar, -1);
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

		// players
		if (verbose) {
			cerr << "load players" << endl;
		}

		{
			auto& ps = env.get_cont<Player>();
			size_t tmp;
			ar >> tmp;
			for (size_t i=0; i<tmp; ++i) {

				Player x;
				ar >> x.id;
				ar >> x.name;
				ar >> x.color;
				ar >> x.flag_id;

				auto key = x.id;
				ps.emplace(key, std::move(x));
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
				ar >> x.storage;

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
				env.init((*p).second, env.get_terr(read<Coords>(ar)));

			}
		}


		// units
		if (verbose) {
			cerr << "load units" << endl;
		}

		{

			size_t tmp;
			ar >> tmp;
			for (size_t i=0; i<tmp; ++i) {

				read_unit(ar, env);

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
		// current player
		if (env.state == 1) {
			env.set_current_player(
				env.get<Player>(
					read<Player::Id>(ar) ) );
		}
		else {
			read<Player::Id>(ar);
		}

		// current player
		//l += write(f, env.current_player_id);

	}





}

#endif
