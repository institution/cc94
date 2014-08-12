#ifndef ACT_H
#define ACT_H


namespace col{


	//
	/*
		load/buy unit_id item_id num
		unload/sell unit_id item_id num

		build-colony unit_id name   // claim-land

		improve unit_id phys_id (Plow|Road|Forest)
		destroy unit_id phys_id (Plow|Road|Forest)

		toogle-board unit_id

		move/board dx dy unit_id
		attack dx dy unit_id

		work-build build_id unit_id
		work-field field_id unit_id
		work-none unit_id

		prod-build terr_id build_id item_id
		prod-field terr_id field_id item_id

		construct terr_id build_id build_type_id

		equip unit_type_id unit_id

		ready

		>>


	 */

	namespace inter {
		struct build_colony{
			Unit::Id unit_id;
			build_colony(Unit::Id unit_id):
				unit_id(unit_id) {}
		};

		struct improve{
			Unit::Id unit_id;
			Phys::Id phys_id;
			improve(Unit::Id unit_id, Phys::Id phys):
				unit_id(unit_id), phys_id(phys_id) {}
		};
		struct destroy{
			Unit::Id unit_id;
			Phys::Id phys_id;
			destroy(Unit::Id unit_id, Phys::Id phys):
				unit_id(unit_id), phys_id(phys_id) {}
		};
		struct toogle_board{
			Unit::Id unit_id;
			toogle_board(Unit::Id unit_id):
				unit_id(unit_id) {}
		};
		struct move_board{
			int8 dx;
			int8 dy;
			Unit::Id unit_id;
			move_board(int8 dx, int8 dy, Unit::Id unit_id):
				dx(dx), dy(dy), unit_id(unit_id) {}
		};
		struct attack{
			int8 dx;
			int8 dy;
			Unit::Id unit_id;
			attack(int8 dx, int8 dy, Unit::Id unit_id):
				dx(dx), dy(dy), unit_id(unit_id) {}
		};
		struct work_build{
			Build::Id build_id;
			Unit::Id unit_id;
			work_build(Build::Id build_id, Unit::Id unit_id):
				build_id(build_id), unit_id(unit_id) {}
		};
		struct work_field{
			Field::Id build_id;
			Unit::Id unit_id;
			work_field(Field::Id build_id, Unit::Id unit_id):
				build_id(build_id), unit_id(unit_id) {}
		};
		struct work_none{
			Unit::Id unit_id;
			work_none(Unit::Id unit_id):
				unit_id(unit_id) {}
		};
		struct prod_build{
			Terr::Id terr_id;
			Build::Id build_id;
			Item::Id item_id;
			prod_build(Terr::Id terr_id, Build::Id build_id, Item::Id item_id):
				terr_id(terr_id), build_id(build_id), item_id(item_id) {}
		};
		struct prod_field{
			Terr::Id terr_id;
			Field::Id build_id;
			Item::Id item_id;
			prod_field(Terr::Id terr_id, Field::Id build_id, Item::Id item_id):
				terr_id(terr_id), build_id(build_id), item_id(item_id) {}

		};
		struct construct{
			Terr::Id terr_id;
			Build::Id build_id;
			BuildType::Id build_type_id;
			construct(Terr::Id terr_id, Build::Id build_id, BuildType::Id build_type_id):
				terr_id(terr_id), build_id(build_id), build_type_id(build_type_id) {}
		};
		struct equip{
			Unit::Id unit_id;
			UnitType::Id unit_type_id;
			equip(Unit::Id unit_id, UnitType::Id unit_type_id):
				unit_id(unit_id), unit_type_id(unit_type_id) {}
		};
		struct ready{
			ready() {}
		};

	}


}


#endif
