#pragma once
#include "col.hpp"
#include "env.hpp"

namespace col {
	
	using Ins = int8_t;
	Ins const
		InsNone{0},
		InsWait{1},       // Space for N turns
		InsMove{2},
		InsAttack{3},
		InsBuild{4},
		InsClear{5},
		InsRoad{6},
		InsPlow{7},
		InsWork{9},        // automatically assigned to units working inside city
		InsAssign{10},     // assign to work; arg = workplace_id
		InsColonize{11},
		InsEnd{12};
		
	inline string get_ins_name(Ins ins) {
		switch (ins) {
			case InsNone: return "InsNone";
			case InsWait: return "InsWait";
			case InsMove: return "InsMove";
			case InsAttack: return "InsAttack";
			case InsBuild: return "InsBuild";
			case InsClear: return "InsClear";
			case InsRoad: return "InsRoad";
			case InsPlow: return "InsPlow";
			case InsWork: return "InsWork";
			case InsAssign: return "InsAssign";
			case InsColonize: return "InsColonize";
			default: 
				print(std::cerr, "WARINING: unknown ins code: %||\n", ins);
				return "InsError";			
		}	
	}
		
	using Arg = int8_t;
		
	struct Cmd{
		Ins ins;
		Arg arg;
		Cmd() = default;
		explicit Cmd(Ins ins): ins(ins), arg(0) {}
		explicit Cmd(Ins ins, Arg arg): ins(ins), arg(arg) {}
	};
	
	inline bool operator==(Cmd a, Cmd b) {
		return a.ins == b.ins and a.arg == b.arg;
	}
	
	inline ostream & operator<<(ostream & o, Cmd const& cmd) {
		o << format("(%|| %||)", get_ins_name(cmd.ins), int(cmd.arg));
		return o;
	}
	

	struct Cmds {
		vector<Cmd> cmds;
		
		
		
		size_t size() const { return cmds.size(); }
		Cmd const& at(size_t i) const { return cmds.at(i); }
		Cmd & at(size_t i) { return cmds.at(i); }
		
		
		void pop() { 
			assert(has());
			cmds.pop_back();
		}
		
		bool has() const { return cmds.size() > 0;}
		void clear() { cmds.clear(); }
		
		Cmd get() const { 
			auto l = cmds.size();
			if (l > 0) {
				return cmds.at(l-1); 
			}
			else {
				return Cmd(InsNone);
			}				
		}
		
		void radd(Cmd cmd) {	
			cmds.push_back(Cmd(InsNone));
			cmds.at(cmds.size() - 1) = cmd;			
		}
		
					
		void add(Cmd cmd) {	
			cmds.push_back(Cmd(InsNone));
			for (auto i = cmds.size(); i >= 2; --i)
			{
				cmds[i-1] = cmds[i-2];
			}				
			cmds[0] = cmd;
		}
		
		void adds(Cmds const& other) {				
			for (auto i = other.cmds.size(); i > 0; --i)
			{
				add(other.cmds[i-1]);
			}			
		}
		
		void set(Cmd cmd) {				
			clear();
			add(cmd);
		}
	};
	
	
	inline ostream & operator<<(ostream & o, Cmds const& cmds) 
	{
		o << "(";
		for (size_t i = 0; i < cmds.size(); ++i)
		{
			o << cmds.at(i);
		}
		o << ")";		
		return o;
	}
	
	
	
}
