#pragma once

#include <list>

#include "col.hpp"
#include "base.hpp"
#include "dir.hpp"
#include "objs.hpp"
#include "csv.hpp"
#include "terr.hpp"
#include "build.hpp"
#include "unit.hpp"
#include "roll.hpp"
#include "error.hpp"
#include "field.hpp"
#include "type_name.hpp"
#include "colony.hpp"
#include "faction.hpp"

namespace col {

	ENABLE_TYPENAME(Unit);
	ENABLE_TYPENAME(UnitType);
	ENABLE_TYPENAME(Build);
	ENABLE_TYPENAME(BuildType);
	ENABLE_TYPENAME(Faction);
	ENABLE_TYPENAME(Colony);
	ENABLE_TYPENAME(Terr);


	using BuildTypes = ext::darray1<BuildType, BuildType::Id>;
	using UnitTypes = ext::darray1<UnitType, UnitType::Id>;

	using Terrs = ext::darray2<Terr, Coord>;
	
	
	
	using Units = unordered_map<Unit::Id, Unit>;
	//using Builds = unordered_map<Build::Id, Build>;
	using Colonies = unordered_map<Colony::Id, Colony>;
	// using Nations = unordered_map<Nation::Id, Nation>;
	using Factions = unordered_map<Faction::Id, Faction>;

	
	template<class T, class K>
	K get_free_id(unordered_map<K,T> const& xs) 
	{
		K i = 0;		
		auto p = xs.cbegin();
		auto end = xs.cend();
		
		while (p != end and i == p->first)
        { 
			++p;
			++i;
		}
		
		return i;
	}
	
	template<class T, class K>
	T & get_memory(unordered_map<K,T> & xs, K i) 
	{
		// assert i is unused
		return &xs[i];
	}
	



	struct Core {
		// const
		shared_ptr<BuildTypes> bts;
		shared_ptr<UnitTypes> uts;

		UnitType & get_unittype(UnitType::Id id) { return uts->at(id); }
		BuildType & get_buildtype(BuildType::Id id) { return bts->at(id); }

		UnitType const& get_unittype(UnitType::Id id) const { return uts->at(id); }
		BuildType const& get_buildtype(BuildType::Id id) const { return bts->at(id); }
		

		
		// state
		Factions factions;
		Colonies colonies;
		Units units;

		// map state
		Coord w{0};
		Coord h{0};
		Terrs terrs;

		// detail
		uint32_t next_id{0};

		explicit
		Core():
			terrs(Coords(0,0))
		{
			bts = make_shared<BuildTypes>();
			uts = make_shared<UnitTypes>();
			w = h = 0;
			next_id = 0;
		}

		~Core() {
			clear_units();
			clear_colonies();
			clear_factions();
		}


		Unit::Id get_id(Unit const& u) const { return u.id; }
		Unit::Id get_id(Unit const* u) const { return (u) ? (u->id) : (Unit::Id(0)); }
		UnitType::Id get_id(UnitType const& ut) const { return ut.id; }
		Faction::Id get_id(Faction const& n) { return n.id; }

		
		template<class Type>
		void loads(string const& fn);

		template <typename T>
		unordered_map<typename T::Id, T>& get_cont();

		template <typename T>
		unordered_map<typename T::Id, T> const& get_cont() const;

		//template<class T>
		uint32 get_next_id() { return ++next_id; }


		template<class T, class... A>
		T& create(A&&... args);

		template <typename T>
		void remove(typename T::Id const& id);

		template <typename T>
		T const& get(typename T::Id const& id) const;

		template <typename T>
		T & get(typename T::Id const& id);

		template <typename T>
		bool exist(typename T::Id const& id);

		Coords get_coords(Terr const& t) const;
		Terr::Id get_id(Terr const& t) const { return get_coords(t); }

		bool has_coords(Coords c) const { return terrs.has(c); }
		Coords get_coords(Unit const& u) const;

		Terr & get_terr(Coords const& z);
		Terr const& get_terr(Coords const& z) const;

		template <typename T> inline
		Terr & get_terr(T const& u);

		template <typename T>
		Terr const& get_terr(T const& u) const;

		template <typename T> inline
		T * opt(typename T::Id const& id);
	
		template <typename T> inline
		T const* opt(typename T::Id const& id) const;
	


		void resize(Coords const& dim);
		void set_terr(Coords const& z, Terr const& t) {	terrs(z) = t; }

		template <Dir D>	bool in_bound(Coords const& p) const;
		template <Dir D>	bool on_bound(Coords const& p) const;

		bool in_bounds(Coords const& p) const;


		void free(Unit & u);
		void free(Colony & c);
		void free(Faction & c);

		void clear_units();
		void clear_colonies();
		void clear_factions();


	};

	inline Coords Core::get_coords(Terr const& t) const {
		// offset = x*w + y
		assert(terrs.size() > 0);
		
		auto offset = size_t(&t - &terrs[0]);
		
		assert(offset < terrs.size());
		
		return Coords(offset % w, offset / w);
	}

	inline Coords Core::get_coords(Unit const& u) const {
		return get_coords(get_terr(u));
	}

	inline Terr & Core::get_terr(Coords const& z) {
		return terrs(z);
	}

	inline Terr const& Core::get_terr(Coords const& z) const {
		return terrs(z);
	}

	template <typename T> inline
	Terr const& Core::get_terr(T const& u) const {
		if (u.terr) {
			return *u.terr;
		}
		else {
			throw Error("not in field");
		}
	}

	template <typename T> inline
	Terr & Core::get_terr(T const& u) {
		if (u.terr) {
			return *u.terr;
		}
		else {
			throw Error("not in field");
		}
	}



	template<class Type> inline
	void Core::loads(string const& fn) {

		auto& ts = get_cont<Type>();

		auto ls = read_conf(fn);
		auto p = ls.begin();
		auto e = ls.end();

		++p; // skip header
		// load line by line
		while (p != e) {
			auto& xs = *p;
			if (xs.size() > 1) {
				// id must be in second column
				auto id = stoi(xs[1]);
				ts.emplace(piecewise_construct,
					forward_as_tuple(id),
					forward_as_tuple(xs)
				);
			}
			++p;
		}

	}

	
	
	template<class T, class... A> inline
	T& Core::create(A&&... args) {
		uint32 id = get_next_id();
		auto & ts = get_cont<T>();
		
		auto p = ts.emplace(piecewise_construct,
			forward_as_tuple(id),
			forward_as_tuple(id, std::forward<A>(args)...)
		).first;

		return (*p).second;
	}

	template <typename T> inline
	void Core::remove(typename T::Id const& id) {
		get_cont<T>().erase(id);
	}

	template <typename T> inline
	T * Core::opt(typename T::Id const& id) {
		auto & ts = get_cont<T>();
		auto it = ts.find(id);
		if (it == ts.end()) {
			return nullptr;
		}
		return &(*it).second;
	}


	template <typename T> inline
	T const* Core::opt(typename T::Id const& id) const {
		auto & ts = get_cont<T>();
		auto it = ts.find(id);
		if (it == ts.end()) {
			return nullptr;
		}
		return &(*it).second;
	}


	template <typename T> inline
	T const& Core::get(typename T::Id const& id) const {
		auto it = get_cont<T>().find(id);

		if (it == get_cont<T>().end()) {
			print("ERROR: no %|| with id=%||\n", type_name<T>::get(), int(id));
			assert(false);
		}

		return (*it).second;
	}

	template <typename T> inline
	T & Core::get(typename T::Id const& id) {
		return const_cast<T&> (
			static_cast<Core const*> (this) -> get<T>(id) );
	}

	template <typename T> inline
	bool Core::exist(typename T::Id const& id) {
		return get_cont<T>().count(id);
	}



	inline void Core::resize(Coords const& dim) {
		//terrs.resize(boost::extents[dim[1]][dim[0]]);
		w = dim[0]; h = dim[1];
		terrs.resize(Coords(w,h));
	}



	inline void Core::free(Unit & u) {
		if (u.terr) {
			u.terr->sub(u);
			u.terr = nullptr;
		}
		if (u.workplace) {
			u.workplace->sub(u);
			u.workplace = nullptr;
		}
		u.type = nullptr;
	}

	inline void Core::free(Colony & c) {
		if (c.terr) {
			c.terr->colony = nullptr;
			c.terr = nullptr;
		}
	}

	inline void Core::free(Faction & c) {

	}


	inline void Core::clear_units() {
		// remove all units
		for (auto& u: units) {
			free(u.second);
		}
		units.clear();
	}

	inline void Core::clear_colonies()  {
		// remove all colonies; first remove units
		for (auto& c: colonies) {
			free(c.second);
		}
		colonies.clear();
	}

	inline void Core::clear_factions() {
		// remove all nations; first remove colonies
		for (auto& p: factions) {
			free(p.second);
		}
		factions.clear();
	}


	template <> inline Terr & Core::get<Terr>(Terr::Id const& id) { return get_terr(id); }
	template <> inline Terr const& Core::get(Terr::Id const& id) const { return get_terr(id); }

	template <>	inline Units & Core::get_cont<Unit>() { return units; }
	template <>	inline Units const& Core::get_cont<Unit>() const { return units; }
	template <>	inline Colonies & Core::get_cont<Colony>() { return colonies; }
	template <>	inline Colonies const& Core::get_cont<Colony>() const { return colonies; }
	template <>	inline Factions & Core::get_cont<Faction>() { return factions; }
	template <>	inline Factions const& Core::get_cont<Faction>() const { return factions; }

	//template <>	inline UnitTypes & Core::get_cont<UnitType>() { return *uts; }
	//template <>	inline UnitTypes const& Core::get_cont<UnitType>() const { return *uts; }
	//template <>	inline BuildTypes & Core::get_cont<BuildType>() { return *bts; }
	//template <>	inline BuildTypes const& Core::get_cont<BuildType>() const { return *bts; }



	template <> inline bool Core::in_bound<DirA>(Coords const& p) const { return 0 <= p[0]; }
	template <> inline bool Core::in_bound<DirD>(Coords const& p) const { return p[0] < w; }
	template <> inline bool Core::in_bound<DirW>(Coords const& p) const { return 0 <= p[1]; }
	template <> inline bool Core::in_bound<DirX>(Coords const& p) const {	return p[1] < h; }

	template <> inline bool Core::on_bound<DirA>(Coords const& p) const {	return 0 == p[0]; }
	template <> inline bool Core::on_bound<DirD>(Coords const& p) const { return p[0] == w - 1; }
	template <> inline bool Core::on_bound<DirW>(Coords const& p) const {	return 0 == p[1]; }
	template <> inline bool Core::on_bound<DirX>(Coords const& p) const { return p[1] == h - 1; }

	inline bool Core::in_bounds(Coords const& p) const {
		return in_bound<DirA>(p) and in_bound<DirD>(p)
		   and in_bound<DirW>(p) and in_bound<DirX>(p);
	}




}

