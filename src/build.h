#ifndef BUILD2_H
#define BUILD2_H

#include "objs.h"

namespace col{

	// colonist   spec type, spec level
	// frigate


	struct BuildType{
		using Id = uint32;

		Id id;
		string name;


		BuildType() {}

		BuildType(Id const& id, string const& name = ""):
			id(id),
			name(name)
		{}

		BuildType(vector<string> const& xs) {
			assert(xs.size() >= 2);
			name = trim_copy(xs[0]);
			id = stoi(xs[1]);
		}

	};

	// yep... csv... anyway we need to move this somehow out of binary - not as easy as it seems - depends on well defined game rules
	int const BuildStockade{1};
	int const BuildFort{2};
	int const BuildFortress{3};
	int const BuildArmory{4};
	int const BuildMagazine{5};
	int const BuildArsenal{6};
	int const BuildDocks{7};
	int const BuildDrydock{8};
	int const BuildShipyard{9};
	int const BuildTownHall{10};
	int const BuildTownHall2{11};
	int const BuildTownHall3{12};
	int const BuildSchoolhouse{13};
	int const BuildCollege{14};
	int const BuildUniversity{15};
	int const BuildWarehouse{16};
	int const BuildFence{17};
	int const BuildCustomHouse{18};
	int const BuildPrintingPress{19};
	int const BuildNewspaper{20};

	int const BuildWeaversHouse{21};
	int const BuildWeaversShop{22};
	int const BuildTextileMill{23};

	int const BuildTobacconistsHouse{24};
	int const BuildTobacconistsShop{25};
	int const BuildCigarFactory{26};

	int const BuildRumDistillersHouse{27};
	int const BuildRumDistillery{28};
	int const BuildRumFactory{29};

	int const BuildCapitolunused1{30};
	int const BuildCapitolExpansion2{31};
	int const BuildFurTradersHouse{32};
	int const BuildFurTradingPost{33};
	int const BuildFurFactory{34};
	int const BuildCarpentersShop{35};
	int const BuildLumberMill{36};
	int const BuildChurch{37};
	int const BuildCathedral{38};
	int const BuildBlacksmithsHouse{39};
	int const BuildBlacksmithsShop{40};
	int const BuildIronWorks{41};
	int const BuildTrees3{42};
	int const BuildTrees2{43};
	int const BuildTrees1{43};
	int const BuildCoast{44};
	int const BuildStable{45};
	int const BuildStableWarehouse{46};





	struct Build: Workplace {

		BuildType const* type;
		int8 free_slots;

		Build(BuildType const& type):
			Workplace(),
			type(&type),
			free_slots(3)
		{}

		Build() = default;
		Build(Build &&) = default;
		Build(Build const&) = default;
		Build& operator=(Build const&) = default;


		BuildType::Id get_type_id() const {
			return type->id;
		}

		// Workplace

		PlaceType::type place_type() {
			return PlaceType::Build;
		}


		uint16 get_yield(Item const& item, bool const& is_expert) const {
			auto id = get_type_id();
			int r = 0;
			switch(item) {
				case ItemRum:
					if (id == BuildRumDistillersHouse) {
						r = 3;
					}
					break;
				case ItemCigars:
					if (id == BuildTobacconistsHouse) {
						r = 3;
					}
					break;
				case ItemCloth:
					if (id == BuildWeaversHouse) {
						r = 3;
					}
					break;
				case ItemCoats:
					if (id == BuildFurTradersHouse) {
						r = 3;
					}
					break;
				case ItemTools:
					if (id == BuildBlacksmithsShop) {
						r = 3;
					}
					break;
				case ItemMuskets:
					if (id == BuildArmory) {
						r = 3;
					}
					break;
				case ItemHammers:
					if (id == BuildCarpentersShop) {
						r = 3;
					}
					if (id == BuildLumberMill) {
						r = 6;
					}
					break;
				case ItemCross:
					if (id == BuildChurch) {
						r = 3;
					}
					break;
				case ItemBell:
					if (id == BuildTownHall) {
						r = 3;
					}
					break;
			}
			return r;
		}

		bool assign(bool const& exec=1) {
			if (!free_slots) {
				return false;
			}
			if (exec) {
				free_slots -= 1;
			}
			return true;
		}

		bool leave(bool const& exec=1) {
			if (free_slots >= 3) {
				return false;
			}
			if (exec) {
				free_slots += 1;
			}
			return true;
		}

	};

	using buildp = unique_ptr<Build>;


	ostream& operator<<(ostream &out, Build const& obj);


}

#endif
