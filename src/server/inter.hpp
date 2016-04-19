#pragma once


#include <boost/fusion/include/sequence.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include <boost/fusion/include/define_struct_inline.hpp>
#include <boost/fusion/include/invoke.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


#include "col.hpp"
#include "meta.hpp"
#include "base.hpp"
#include "objs.hpp"
#include "nation.hpp"
#include "csv.hpp"
#include "terr.hpp"
#include "build.hpp"
#include "unit.hpp"
#include "roll.hpp"
#include "error.hpp"
#include "field.hpp"
#include "type_name.hpp"




#define CC94_DEFINE_INTER(NAME, ATTRIBUTES)                                     \
	BOOST_FUSION_DEFINE_STRUCT_INLINE(                                          \
		NAME,                                                                   \
		ATTRIBUTES                                                              \
	)                                                                           \
	                                                                            \
	ENABLE_TYPENAME(NAME)                                                       \









namespace col{


	//
	/*
	 action: check if possible
	 effect: no checks


		load/buy unit_id item_id num
		unload/sell unit_id item_id num


	 effect:
		error text

	 action:
		build-colony unit_id name   // claim-land
	 effect:
		init-colony terr_id name
 		set-tp unit_id num

	 action:
		improve unit_id phys_id (Plow|Road|Forest)
		destroy unit_id phys_id (Plow|Road|Forest)
	 effect:
		add-phys terr_id phys_id
		sub-phys terr_id phys_id
		set-time unit_id num

	 acction/effect:
		toogle-board unit_id

	 action:
		move/board dx dy unit_id
	 effect:
		move-unit terr_id unit_id
		toogle-board unit_id
		set-time unit_id num

	 action:
		attack dx dy unit_id
	 effect:
		kill unit_id
		set-time unit_id num

	 action/effect:
		work-build build_id unit_id
		work-field field_id unit_id
		work-none unit_id

	 action/effect:
		prod-build terr_id build_id item_id
		prod-field terr_id field_id item_id

	 action/effect:
		construct terr_id build_id build_type_id

	 action:
		equip unit_type_id unit_id
	 effect:
		sub-item terr_id item_id num
	 	add-item terr_id item_id num
		morph-unit unit_id unit_type_id

	 action:
		ready
	 effect(on turn):
		add-item terr_id item_id num
		sub-item terr_id item_id num
		set-item terr_id item_id num
		create-unit terr_id unit-type-id
		morph-build terr_id build_id build_type_id
		kill unit_id
		set-time unit_id num
		set-turn num
		set-current-nation nation_id (activate)
		...


	 more effects:
		set-biome terr_id biome_id
		set-alt terr_id alt
		resize x y
		destroy-all-units  (kill vs destroy difference?)
		destroy-all-colonies
		destroy-all-...

	 */







	namespace inter {

		// action
		CC94_DEFINE_INTER(
			load_cargo,
			(Unit::Id, unit_id)
			(Item, item)
			(Amount, num)
		)

		// action
		CC94_DEFINE_INTER(
			build_colony,
			(Unit::Id, unit_id)
		)

		// action
		CC94_DEFINE_INTER(
			improve,
			(Unit::Id, unit_id)
			(Phys, phys_id)
		)

		// action
		CC94_DEFINE_INTER(
			destroy,
			(Unit::Id, unit_id)
			(Phys, phys_id)
		)

		// action
		CC94_DEFINE_INTER(
			move_unit,
			(int8, dx)
			(int8, dy)
			(Unit::Id, unit_id)
		)

		// action
		CC94_DEFINE_INTER(
			attack,
			(int8, dx)
			(int8, dy)
			(Unit::Id, unit_id)
		)

		// action
		CC94_DEFINE_INTER(
			equip,
			(Unit::Id, unit_id)
			(UnitType::Id, unit_type_id)
		)

		// action
		CC94_DEFINE_INTER(
			ready,
			(Nation::Id, nation_id)
		)

		// action
		CC94_DEFINE_INTER(
			toogle_board,
			(Unit::Id, unit_id)
		)

		// action
		CC94_DEFINE_INTER(
			work_build,
			(Build::Id, build_id)
			(Unit::Id, unit_id)
		)

		// action
		CC94_DEFINE_INTER(
			work_field,
			(Field::Id, field_id)
			(Unit::Id, unit_id)
		)

		// action
		CC94_DEFINE_INTER(
			work_none,
			(Unit::Id, unit_id)
		)

		// action
		CC94_DEFINE_INTER(
			prod_build,
			(Terr::Id, terr_id)
			(Build::Id, build_id)
			(Item, item_id)
		)

		// action
		CC94_DEFINE_INTER(
			prod_field,
			(Terr::Id, terr_id)
			(Field::Id, field_id)
			(Item, item_id)
		)

		// action
		CC94_DEFINE_INTER(
			construct_build,
			(Terr::Id, terr_id)
			(Build::Id, build_id)
			(BuildType::Id, buildtype_id)
		)
		
		// action
		CC94_DEFINE_INTER(
			construct_unit,
			(Terr::Id, terr_id)
			(Build::Id, build_id)
			(UnitType::Id, unittype_id)
		)

		//CC94_DEFINE_INTER(
		//	load,
		//	(string, data)
		//)

		CC94_DEFINE_INTER(
			start,
		)

		CC94_DEFINE_INTER(
			activate,
		)


		using Any = boost::variant<

			load_cargo,

			improve,
			destroy,

			ready,
			move_unit,
			improve,
			destroy,
			build_colony,

			toogle_board,

			work_build,
			work_field,
			work_none,

			prod_build,
			prod_field,

			construct_build,
			construct_unit

		>;
		
	
	

	/**
		
		transfer_cargo(Store x, Store y, Manifest manifest)
		move_unit(Unit u, Dir dir)

		improve(Unit u, Phys phys),
		destroy(Unit u, Phys phys),

		ready,
		move_unit,
		improve,
		destroy,
		build_colony,

		toogle_board,

		work_build,
		work_field,
		work_none,

		prod_build,
		prod_field,

		construct_build,
		construct_unit
	**/


	/*
	 action(7):

		+build-colony unit_id    // claim-land
		+improve unit_id phys_id (Plow|Road|Forest)
		+destroy unit_id phys_id (Plow|Road|Forest)

		+move/board dx dy unit_id
		attack dx dy unit_id
		equip unit_type_id unit_id

	 	ready  // nation_id defaults to current player


	 pseudoaction
		get-state nation_id

	 action/effect(7):
		+ toogle-board unit_id

	 	+ work-build build_id unit_id
		+ work-field field_id unit_id
		+ work-none unit_id

	 	+ prod-build terr_id build_id item_id
		+ prod-field terr_id field_id item_id

	 	+ construct terr_id build_id build_type_id

	 effect(15):
		+ error text

		+ init-colony terr_id
		+ init-unit terr_id unit-type-id
		init-build terr_id build_id build_type_id
		kill-unit unit_id

		+ add-phys terr_id phys_id
		+ sub-phys terr_id phys_id

		+ set-tp unit_id num

		+ sub-item terr_id item num
	 	+ add-item terr_id item num
		+ set-item terr_id item num

		+ morph-unit unit_id unit_type_id
		+ morph-build terr_id build_id build_type_id

		+ set-turn num
		+ set-current-nation nation_id (activate)

	 effect(3)(only editor):
		+ reset x y
		+ set-biome terr_id biome
		+ set-alt terr_id alt

		+ load_nation nation_id *data


	 */


	}


}

/*
template <class A>
struct my_write{
	A * ar;

	my_write(A & ar): ar(&ar) { }

	template <class U>
	void operator()(U & u) const {
		(*ar) & u;
	}
};


namespace boost {
	namespace serialization {

		template<class A>
		void serialize(A & ar, inter::Move & m, const unsigned int version)
		{
			boost::fusion::for_each(m, ::my_write<A>(ar));
		}

	}
}
*/


