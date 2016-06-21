#pragma once

#include "objs.hpp"
#include "makeable.hpp"
#include "workplace.hpp"

namespace col{

	// colonist   spec type, spec level
	// frigate



	struct BuildType: Makeable{
		using Id = Makeable::Id;

		// params
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
		
		// constructors
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

		// getters
		Id get_id() const { return id; }
		string const& get_name() const { return name; }
		Item const& get_proditem() const { return proditem; }
		Id get_place_on_id() const { return place_on_id; }
		
		Amount get_cost() const { return cost_hammers; }
		Class get_class() const { return ClassBuildType; }
		
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
	int const BuildUnused18{18};
	int const BuildCustomHouse{19};
	int const BuildPrintingPress{20};
	int const BuildNewspaper{21};

	int const BuildWeaversHouse{22};
	int const BuildWeaversShop{23};
	int const BuildTextileMill{24};

	int const BuildTobacconistsHouse{25};
	int const BuildTobacconistsShop{26};
	int const BuildCigarFactory{27};

	int const BuildRumDistillersHouse{28};
	int const BuildRumDistillery{29};
	int const BuildRumFactory{30};

	int const BuildCapitolunused1{31};
	int const BuildCapitolExpansion2{32};

	int const BuildFurTradersHouse{33};
	int const BuildFurTradingPost{34};
	int const BuildFurFactory{35};

	int const BuildCarpentersShop{36};
	int const BuildLumberMill{37};
	
	int const BuildChurch{38};
	int const BuildCathedral{39};
	int const BuildBlacksmithsHouse{40};
	int const BuildBlacksmithsShop{41};
	int const BuildIronWorks{42};
	int const BuildTrees3{43};
	int const BuildTrees2{44};
	int const BuildTrees1{45};
	int const BuildCoast{46};
	int const BuildStable{47};
	int const BuildStableWarehouse{48};


	

	struct Build: Workplace {
		using Id = int;

		BuildType const* type;
		int8 free_slots;
				
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
		
		void morph(BuildType const& bt) {
			type = &bt;			
		}

		// Workplace

		PlaceType::type place_type() override {
			return PlaceType::Build;
		}
		
		virtual Class get_class() const { return ClassBuild; }

		string const& get_name() const {
			return this->type->name;
		}
		
		// getters
		BuildType const& get_type() const { return *type; }
		int get_slots() const override { return type->slots; };
		auto get_consitem() const -> decltype(type->itemcons) const& override { return type->itemcons; };
		auto get_min_colony() const -> decltype(type->min_colony) const& { return type->min_colony; };
		auto get_place_on_type_id() const -> decltype(type->place_on_id) const& { return type->place_on_id; };
		
		
		
		
		Amount get_prod() const { 
			return type->prod; 
		}
		
		Amount get_cons() const { 
			return type->cons;			
		};

		Amount get_prod(Env const& env, Unit const& unit, Item const& item) const override;
		
		Amount get_cons(Env const& env, Unit const& unit, Item const& item) const override;


		Build & set_type(BuildType const& type) {
			this->type = &type;
			set_proditem(type.proditem);
			// set free slots
			return *this;
		}


		Item const& get_proditem() const override {
			return type->proditem;
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
