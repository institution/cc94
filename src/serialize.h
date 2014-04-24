#ifndef SERIALIZE32_H
#define SERIALIZE32_H

#include <boost/serialization/map.hpp>
#include "env.h"



//namespace boost {namespace serialization {

namespace col {
	
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
	void write(A & ar, T const& t) {
		ar << t;		
	}
	
	
	template<class Archive>
    void Env::save(Archive & ar, uint const& ver) const
	{
		auto& env = *this;
		
		// players
		cerr << "save players" << endl;
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
		cerr << "save terrain" << endl;
		{
			ar << env.w;
			ar << env.h;

			for(Coord j = 0; j < env.h; ++j) {
				for(Coord i = 0; i < env.w; ++i) {
					auto& x = env.get_terr(Coords(i,j));

					cerr << Coords(i,j) << endl;
					
					// terr value
					ar << x.biome;
					ar << x.phys;

				}
			}
		}
		
		// colonies
		cerr << "save colonies" << endl;
		{
			auto& ps = env.get_cont<Colony>();
			auto tmp = ps.size();
			ar << tmp;
			for (auto& p: ps) {
				auto& x = p.second;

				// unit
				ar << x.id;
				ar << x.name;
				ar << x.storage;

				// location
				auto cr = env.get_coords(env.get_terr(x));
				ar << cr;

			}
		}

		// builds
		cerr << "save builds" << endl;
		{
			auto& ps = env.get_cont<Build>();
			auto tmp = ps.size();
			ar << tmp;
			for (auto& p: ps) {
				auto& x = p.second;

				// unit
				ar << x.id;
				ar << x.type->id;
				ar << x.free_slots;			

				// location
				auto cr = env.get_coords(env.get_terr(x));
				ar << cr;

			}
		}
		
		// units
		cerr << "save units" << endl;
		{
			auto& ps = env.get_cont<Unit>();
			auto tmp = ps.size();
			ar << tmp;
			for (auto& p: ps) {
				auto& x = p.second;

				// unit
				ar << x.id;
				ar << x.type->id;
				ar << x.player->id;
				ar << x.time_left;
				ar << x.workitem;
				ar << x.workplace;
				
				// location
				auto cr = env.get_coords(env.get_terr(x));
				ar << cr;
			}
		}
		
		// turn info
		ar << env.turn_no;
		// next id
		ar << next_id;

		
		// current player
		//l += write(f, env.current_player_id);

	}
	
    template<class Archive>
    void Env::load(Archive & ar, uint const& ver) 
	{
		auto& env = *this;
		
		// players
		cerr << "load players" << endl;
		{
			auto& ps = env.get_cont<Player>();
			size_t tmp;
			ar >> tmp;
			for (int i=0; i<tmp; ++i) {
				
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
		cerr << "load terrain" << endl;
		{
			ar >> env.w;
			ar >> env.h;
			
			env.resize({env.w, env.h});
			
			for(Coord j = 0; j < env.h; ++j) {
				for(Coord i = 0; i < env.w; ++i) {
					
					auto& x = env.ref_terr(Coords(i,j));
					
					cerr << Coords(i,j) << endl;
					
					// terr value
					ar >> x.biome;
					ar >> x.phys;

				}
			}
		}
		
		// colonies
		cerr << "load colonies" << endl;
		{
			auto& ps = env.get_cont<Colony>();
			size_t tmp;
			ar >> tmp;
			for (int i=0; i<tmp; ++i) {
				
				Colony x;
				ar >> x.id;
				ar >> x.name;
				ar >> x.storage;

				auto key = x.id;
				auto p = ps.emplace(key, std::move(x)).first;
				
				// location
				env.move_in(env.ref_terr(read<Coords>(ar)), (*p).second);

			}
		}

		
		
		// builds
		cerr << "load builds" << endl;
		{
			auto& ps = env.get_cont<Build>();
			size_t tmp;
			ar >> tmp;
			for (int i=0; i<tmp; ++i) {
				
				Build x;
				ar >> x.id;
				x.type = & env.get<BuildType>( read<BuildType::Id>(ar) );
				ar >> x.free_slots;	
				
				auto key = x.id;
				ps.emplace(key, std::move(x));
				
				// location
				Coords cr;
				ar >> cr;
				//env.move_in(, x);

			}
		}
		
		
		// units
		cerr << "load units" << endl;
		{
			auto& ps = env.get_cont<Unit>();
			size_t tmp;
			ar >> tmp;
			for (int i=0; i<tmp; ++i) {

				
				Unit x;
				
				ar >> x.id;
				x.type = & env.get<UnitType> ( read<UnitType::Id> (ar) );
				x.player = & env.get<Player> ( read<Player::Id> (ar) );
				ar >> x.time_left;
				ar >> x.workplace;
				ar >> x.workitem;

				auto key = x.id;
				auto p = ps.emplace(key, std::move(x)).first;
								
				env.move_in( env.ref_terr( read<Coords>(ar) ), (*p).second);
			}
		}
		
		// turn info
		ar >> env.turn_no;

		// current player
		//l += write(f, env.current_player_id);
		
	}
	
	
}

#endif
