#ifndef CORE5938092735_H
#define CORE5938092735_H

#include <list>

#include "col.hpp"
#include "base.h"
#include "objs.h"
#include "nation.h"
#include "csv.h"
#include "terr.h"
#include "build.h"
#include "unit.h"
#include "roll.h"
#include "error.h"
#include "field.h"

namespace col {

	using BuildTypes = unordered_map<BuildType::Id, BuildType>;
	using UnitTypes = unordered_map<UnitType::Id, UnitType>;

	using Terrs = boost::multi_array<Terr, 2>;
	using Units = unordered_map<Unit::Id, Unit>;
	//using Builds = unordered_map<Build::Id, Build>;
	using Colonies = unordered_map<Colony::Id, Colony>;
	// using Nations = unordered_map<Nation::Id, Nation>;
	using Nations = unordered_map<Nation::Id, Nation>;



	using Vi = std::bitset<32>;


	struct Core {
		// const
		shared_ptr<BuildTypes> bts;
		shared_ptr<UnitTypes> uts;

		// state
		Nations nations;
		Colonies colonies;
		Units units;

		// map state
		Coord w{0};
		Coord h{0};
		Terrs terrs;

		// visibility
		boost::multi_array<Vi, 2> vis;

		// detail
		uint32 next_id{0};

		explicit
		Core():
			terrs(Coords(0,0), boost::fortran_storage_order()),
			vis(Coords(0,0), boost::fortran_storage_order())
		{
			bts = make_shared<BuildTypes>();
			uts = make_shared<UnitTypes>();
			w = h = 0;
			next_id = 0;
		}

		~Core() {
			clear_units();
			clear_colonies();
			clear_nations();
		}


		Unit::Id get_id(Unit const& u) const { return u.id; }
		UnitType::Id get_id(UnitType const& ut) const { return ut.id; }


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

		Coords get_coords(Unit const& u) const;

		Terr & get_terr(Coords const& z);
		Terr const& get_terr(Coords const& z) const;

		template <typename T> inline
		Terr & get_terr(T const& u);

		template <typename T>
		Terr const& get_terr(T const& u) const;


		void resize(Coords const& dim);
		void set_terr(Coords const& z, Terr const& t) {	terrs(z) = t; }

		template <Dir::t D>	bool in_bound(Coords const& p) const;
		template <Dir::t D>	bool on_bound(Coords const& p) const;

		bool in_bounds(Coords const& p) const;


		void free(Unit & u);
		void free(Colony & c);
		void free(Nation & c);

		void clear_units();
		void clear_colonies();
		void clear_nations();


	};

	inline Coords Core::get_coords(Terr const& t) const {
		// offset = x*w + y
		uint32 offset = uint32(&t - &terrs[0][0]);
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
		auto& ts = get_cont<T>();
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
	T const& Core::get(typename T::Id const& id) const {
		auto it = get_cont<T>().find(id);
		if (it == get_cont<T>().end()) {
			throw Error("no such object");
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
		vis.resize(Coords(w,h));
	}



	inline void Core::free(Unit & u) {
		if (u.terr) {
			u.terr->sub(u);
			u.terr = nullptr;
		}
		if (u.build) {
			u.build->sub(u);
			u.build = nullptr;
		}
		if (u.field) {
			u.field->sub(u);
			u.field = nullptr;
		}
		u.type = nullptr;
		u.nation = nullptr;
	}

	inline void Core::free(Colony & c) {
		if (c.terr) {
			c.terr->colony = nullptr;
			c.terr = nullptr;
		}
	}

	inline void Core::free(Nation & c) {
		if (c.player) {
			// send disconnect sig? no need this is internal pointer
			c.player = nullptr;
		}
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

	inline void Core::clear_nations() {
		// remove all nations; first remove colonies
		for (auto& p: nations) {
			free(p.second);
		}
		nations.clear();
	}


	template <> inline Terr & Core::get<Terr>(Terr::Id const& id) { return get_terr(id); }
	template <> inline Terr const& Core::get(Terr::Id const& id) const { return get_terr(id); }

	template <>	inline Units & Core::get_cont<Unit>() { return units; }
	template <>	inline Units const& Core::get_cont<Unit>() const { return units; }
	template <>	inline Colonies & Core::get_cont<Colony>() { return colonies; }
	template <>	inline Colonies const& Core::get_cont<Colony>() const { return colonies; }
	template <>	inline Nations & Core::get_cont<Nation>() { return nations; }
	template <>	inline Nations const& Core::get_cont<Nation>() const { return nations; }

	template <>	inline UnitTypes & Core::get_cont<UnitType>() { return *uts; }
	template <>	inline UnitTypes const& Core::get_cont<UnitType>() const { return *uts; }
	template <>	inline BuildTypes & Core::get_cont<BuildType>() { return *bts; }
	template <>	inline BuildTypes const& Core::get_cont<BuildType>() const { return *bts; }



	template <> inline bool Core::in_bound<Dir::A>(Coords const& p) const { return 0 <= p[0]; }
	template <> inline bool Core::in_bound<Dir::D>(Coords const& p) const { return p[0] < w; }
	template <> inline bool Core::in_bound<Dir::W>(Coords const& p) const { return 0 <= p[1]; }
	template <> inline bool Core::in_bound<Dir::X>(Coords const& p) const {	return p[1] < h; }

	template <> inline bool Core::on_bound<Dir::A>(Coords const& p) const {	return 0 == p[0]; }
	template <> inline bool Core::on_bound<Dir::D>(Coords const& p) const { return p[0] == w - 1; }
	template <> inline bool Core::on_bound<Dir::W>(Coords const& p) const {	return 0 == p[1]; }
	template <> inline bool Core::on_bound<Dir::X>(Coords const& p) const { return p[1] == h - 1; }

	inline bool Core::in_bounds(Coords const& p) const {
		return in_bound<Dir::A>(p) and in_bound<Dir::D>(p)
		   and in_bound<Dir::W>(p) and in_bound<Dir::X>(p);
	}




}


#endif
