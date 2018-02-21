#include "serialize3.hpp"

namespace col{

	void read_unittype(RCParser & a, Env & env)
	{
		UnitType::Id id;	
		read(a, id);
		auto & u = env.get_unittype(id);
		u.id = id;	
		read(a, u.speed);
		read(a, u.attack);
		read(a, u.combat);
		read(a, u.slots);
		read(a, u.size);
		read(a, u.icon);
		read(a, u.travel);
		read(a, u.base);
		read(a, u.item1);
		read(a, u.num1);
		read(a, u.item2);
		read(a, u.num2);
		read(a, u.cost);
		read(a, u.icon_expert);
		read(a, u.name);
	}

	void read_buildtype(RCParser & a, Env & env)
	{
		BuildType::Id id;	
		read(a, id);
		auto & b = env.get_buildtype(id);
		b.id = id;	
		read(a, b.slots);
		read(a, b.proditem);
		read(a, b.itemcons);
		read(a, b.cost_hammers);
		read(a, b.cost_tools);
		read(a, b.min_colony);
		read(a, b.place_on_id);
		read(a, b.prod);
		read(a, b.cons);
		read(a, b.icon);
		read(a, b.name);
	}

	void read_defs(RCParser & p, Env & env)
	{
		// TODO: init to zeroes
		(*env.bts).resize(100);
		(*env.uts).resize(100);
			
		
		while (p.ok())
		{
			auto s = p.read_symbol();
			if (s == "unittype") {
				read_unittype(p, env);			
			}
			else if (s == "buildtype") {
				read_buildtype(p, env);			
			}
			else {
				p.set_error("unknown command: %||", s);
			}
		}
		p.end();
	}

	void read_defs(Path const& path, Env & env)
	{
		RCParser p(path);
		read_defs(p, env);
		p.close();
	}


}
