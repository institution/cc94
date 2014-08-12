#ifndef BUILD2_H
#define BUILD2_H

#include "objs.h"
#include "workplace.h"

namespace col{

	// colonist   spec type, spec level
	// frigate



	struct BuildType{
		using Id = uint32;

		string name;
		Id id;
		int slots;
		Item proditem{ItemNone};
		Item itemcons{ItemNone};
		int cost_hammers;
		int cost_tools;
		int min_colony;
		Id place_on_id;
		int prod;
		int cons;
		int icon;

		BuildType() {}

		BuildType(Id const& id, string const& name = ""):
			id(id),
			name(name)
		{}

		BuildType(vector<string> const& xs) {
			assert(xs.size() >= 11);

			name = trim_copy(xs[0]);
			id = stoi(xs[1]);
			slots = stoi0(xs[2]);
			proditem = stoi0(xs[3]);
			itemcons = stoi0(xs[4]);
			cost_hammers = stoi0(xs[5]);
			cost_tools = stoi0(xs[6]);
			min_colony = stoi0(xs[7]);
			place_on_id = stoi0(xs[8]);
			prod = stoi0(xs[9]);
			cons = stoi0(xs[10]);
			icon = stoi0(xs[11]);
		}

		string const& get_name() const { return name; }
		Item const& get_proditem() const { return proditem; }
		//BuildType & set_name(string const& name) { this->name = name; return *this; }


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
	int const BuildTrees1{44};
	int const BuildCoast{45};
	int const BuildStable{46};
	int const BuildStableWarehouse{47};





	struct Build: Workplace {
		using Id = int;
		
		BuildType const* type;
		int8 free_slots;
		int hammers{0};
		BuildType const* ctype{nullptr};

		BuildType const& get_type() { return *type; }

		Build(BuildType const& type):
			Workplace(),
			free_slots(type.slots)
		{
			set_type(type);
		}

		Build() = default;
		Build(Build &&) = default;
		Build(Build const&) = default;
		Build& operator=(Build const&) = default;


		BuildType::Id get_type_id() const {
			return type->id;
		}

		bool under_construction() const {
			return ctype != nullptr;
		}

		void morph() {
			type = ctype;
			ctype = nullptr;
			hammers = 0;
		}

		// Workplace

		PlaceType::type place_type() {
			return PlaceType::Build;
		}

		int const& get_hammers() const {
			return this->hammers;
		}

		string const& get_name() const {
			return this->type->name;
		}

		int get_slots() const { return type->slots; };
		auto get_consitem() const -> decltype(type->itemcons) const& { return type->itemcons; };
		auto get_min_colony() const -> decltype(type->min_colony) const& { return type->min_colony; };
		auto get_place_on_type_id() const -> decltype(type->place_on_id) const& { return type->place_on_id; };
		auto get_prod() const -> decltype(type->prod) const& { return type->prod; };
		auto get_cons() const -> decltype(type->cons) const& { return type->cons; };


		Build & set_type(BuildType const& type) {
			this->type = &type;
			set_proditem(type.proditem);
			return *this;
		}


		int get_cost_hammers() const {
			return this->ctype->cost_hammers;
		}

		int get_cost_tools() const {
			return this->ctype->cost_tools;
		}


		Item const& get_proditem() const {
			return type->proditem;
		}

		uint16 get_yield(Item const& item, bool const& is_expert) const {
			if (get_proditem() == item) {
				return get_prod();
			}
			return 0;
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
			if (free_slots >= get_slots()) {
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
