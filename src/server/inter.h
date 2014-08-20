#ifndef INTER9483762856_H
#define INTER9483762856_H

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
#include "base.h"
#include "objs.h"
#include "player.h"
#include "csv.h"
#include "terr.h"
#include "build.h"
#include "unit.h"
#include "roll.h"
#include "error.h"
#include "field.h"

namespace col{
	namespace inter{
		template<class T>
		struct type_name;
	}
}

#define ENABLE_TYPENAME(A)                                                      \
template<>                                                                      \
struct type_name<A> {                                                           \
	static std::string get() {                                                  \
		return #A;                                                              \
	}                                                                           \
};                                                                              \




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
		set-current-player player_id (activate)
		...


	 more effects:
		set-biome terr_id biome_id
		set-alt terr_id alt
		resize x y
		destroy-all-units  (kill vs destroy difference?)
		destroy-all-colonies
		destroy-all-...

	 */



	/*
	 action(7):
		+build-colony unit_id    // claim-land
		+improve unit_id phys_id (Plow|Road|Forest)
		+destroy unit_id phys_id (Plow|Road|Forest)

		+move/board dx dy unit_id
		attack dx dy unit_id
		equip unit_type_id unit_id

	 	ready

	 action/effect(7):
		toogle-board unit_id

	 	work-build build_id unit_id
		work-field field_id unit_id
		work-none unit_id

	 	prod-build terr_id build_id item_id
		prod-field terr_id field_id item_id

	 	construct terr_id build_id build_type_id

	 effect(15):
		error text

		init-colony terr_id name
		init-unit terr_id unit-type-id
		init-build terr_id build_id build_type_id
		kill unit_id

		add-phys terr_id phys_id
		sub-phys terr_id phys_id

		set-tp unit_id num

		sub-item terr_id item_id num
	 	add-item terr_id item_id num
		set-item terr_id item_id num

		morph-unit unit_id unit_type_id
		morph-build terr_id build_id build_type_id

		set-turn num
		set-current-player player_id (activate)

	 effect(3)(only editor):
		resize x y
		set-biome terr_id biome_id
		set-alt terr_id alt



	 */




	namespace inter {


		// action
		CC94_DEFINE_INTER(
			build_colony,
			(Unit::Id, unit_id)
		)

		// action
		CC94_DEFINE_INTER(
			improve,
			(Unit::Id, unit_id)
			(Phys::Id, phys_id)
		)

		// action
		CC94_DEFINE_INTER(
			destroy,
			(Unit::Id, unit_id)
			(Phys::Id, phys_id)
		)

		// action
		CC94_DEFINE_INTER(
			move_board,
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
		)


		// action/effect
		CC94_DEFINE_INTER(
			toogle_board,
			(Unit::Id, unit_id)
		)

		// action/effect
		CC94_DEFINE_INTER(
			work_build,
			(Build::Id, build_id)
			(Unit::Id, unit_id)
		)

		// action/effect
		CC94_DEFINE_INTER(
			work_field,
			(Field::Id, field_id)
			(Unit::Id, unit_id)
		)

		// action/effect
		CC94_DEFINE_INTER(
			work_none,
			(Unit::Id, unit_id)
		)

		// action/effect
		CC94_DEFINE_INTER(
			prod_build,
			(Terr::Id, terr_id)
			(Build::Id, build_id)
			(Item::Id, item_id)
		)

		// action/effect
		CC94_DEFINE_INTER(
			prod_field,
			(Terr::Id, terr_id)
			(Field::Id, field_id)
			(Item::Id, item_id)
		)

		// action/effect
		CC94_DEFINE_INTER(
			construct,
			(Terr::Id, terr_id)
			(Build::Id, build_id)
			(BuildType::Id, build_type_id)
		)

		// effect
		CC94_DEFINE_INTER(
			error,
			(std::string, text)
		)

		// effect
		CC94_DEFINE_INTER(
			init_colony,
			(Terr::Id, terr_id)
		)

		// effect
		CC94_DEFINE_INTER(
			init_unit,
			(Terr::Id, terr_id)
			(UnitType::Id, unit_type_id)
		)

		// effect
		CC94_DEFINE_INTER(
			init_build,
			(Terr::Id, terr_id)
			(Build::Id, build_id)
			(BuildType::Id, build_type_id)
		)

		// effect
		CC94_DEFINE_INTER(
			kill,
			(Unit::Id, unit_id)
		)

		// effect
		CC94_DEFINE_INTER(
			add_phys,
			(Terr::Id, terr_id)
			(Phys::Id, phys_id)
		)

		// effect
		CC94_DEFINE_INTER(
			sub_phys,
			(Terr::Id, terr_id)
			(Phys::Id, phys_id)
		)

		// effect
		CC94_DEFINE_INTER(
			set_tp,
			(Unit::Id, unit_id)
			(int, num)
		)

		// effect
		CC94_DEFINE_INTER(
			sub_item,
			(Terr::Id, terr_id)
			(Item::Id, item_id)
			(int, num)
		)

		// effect
		CC94_DEFINE_INTER(
			add_item,
			(Terr::Id, terr_id)
			(Item::Id, item_id)
			(int, num)
		)

		// effect
		CC94_DEFINE_INTER(
			set_item,
			(Terr::Id, terr_id)
			(Item::Id, item_id)
			(int, num)
		)

		// effect
		CC94_DEFINE_INTER(
			morph_unit,
			(Unit::Id, unit_id)
			(UnitType::Id, unit_type_id)
		)

		// effect
		CC94_DEFINE_INTER(
			morph_build,
			(Terr::Id, terr_id)
			(Build::Id, build_id)
			(BuildType::Id, build_type_id)
		)

		// effect
		CC94_DEFINE_INTER(
			set_turn,
			(int, num)
		)

		// effect
		CC94_DEFINE_INTER(
			set_current_player,
			(Id<Player>, player_id)
		)

		// effect (editor)
		CC94_DEFINE_INTER(
			resize,
			(Coord, x)
			(Coord, y)
		)


		// effect (editor)
		CC94_DEFINE_INTER(
			set_biome,
			(Terr::Id, terr_id)
			(Biome::Id, biome_id)
		)


		// effect (editor)
		CC94_DEFINE_INTER(
			set_alt,
			(Terr::Id, terr_id)
			(int, alt)
		)


		// for testing
		CC94_DEFINE_INTER(
			echo,
			(int, a)
		)


		using Any = boost::variant<
			echo,
			ready,
			move_board,
			improve,
			destroy,
			build_colony,
			init_colony,
			set_tp
		>;




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


#endif
