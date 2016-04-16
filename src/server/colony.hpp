#pragma once

#include "base.hpp"
#include "unit.hpp"
#include "build.hpp"
#include "error.hpp"
#include "field.hpp"
#include "storage.hpp"

namespace col {






	template<class T>
	struct cont{ using type = vector<T>; };

	template<>
	struct cont<Build>{ using type = array<Build,15>; };




	struct Colony {

		using Id = uint32;

		Id id;
		string name;

		Storage store;

		Amount max_storage{100};

		int get_max_storage() { return max_storage; }

		Terr *terr{nullptr};
		
		using Fields = cont<Field>::type;
		Fields fields;

		Colony & add_field(Field const& f) { fields.push_back(f); return *this; }
		void sub_field(Field const& f) { fields.erase(find(fields.begin(), fields.end(), f)); }


		cont<Build>::type builds;

		Build * select_place_for(BuildType const& bt)
		{		
			using SlotId = cont<Build>::type::size_type;
			
			for (SlotId i = 0; i < builds.size(); ++i) 
			{
				auto & b = builds.at(i);
				if (b.get_type().id == bt.get_place_on_id()) 
				{
					return &b;
				}
			}			
			return nullptr;
		}
		


		Store & get_store() { return store; }
		Store const& get_store() const { return store; }
		
		/*vector<Workplace&> && get_workplaces() {
			
		}*/
		
		vector<Workplace const*> get_workplaces() const {
			vector<Workplace const*> fs;
			
			for (auto const& f: fields) {
				fs.push_back(&f);
			}
			
			for (auto const& f: builds) {
				fs.push_back(&f);
			}
			
			return fs;
			
		}


		int get_build_index(Build const& b) const {
			return &b - &builds[0];
		}

		int get_field_index(Field const& f) const;


		Build& get_build(int i) {
			return builds.at(i);
		}

		Build const& get_build(int i) const {
			return builds.at(i);
		}

		Colony & set_build(int const& i, Build const& build) { builds.at(i) = build; return *this; }




		PlaceType::type place_type() {
			return PlaceType::Colony;
		}

		string const& get_name() const {
			return name;
		}



		Colony(Id const& id, string const& name=""):
			id(id),
			name(name),
			builds()
		{}

		Colony() = default;
		Colony(Colony &&) = default;

		Colony(Colony const&) = delete;

		~Colony() {
			assert(terr == nullptr);
		}
		
		Terr & get_terr() const { return *terr; }
		//Terr const& get_terr() const { return *terr; }
			


		template <typename T>
		typename cont<T>::type & get_cont();

		template <typename T>
		typename cont<T>::type const& get_cont() const;



	};




	template <>	inline
	typename cont<Field>::type & Colony::get_cont<Field>() {
		return fields;
	}

	template <>	inline
	typename cont<Build>::type & Colony::get_cont<Build>() {
		return builds;
	}

	template <>	inline
	typename cont<Field>::type const& Colony::get_cont<Field>() const {
		return fields;
	}

	template <>	inline
	typename cont<Build>::type const& Colony::get_cont<Build>() const {
		return builds;
	}

}

