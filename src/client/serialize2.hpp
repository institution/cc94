#pragma once

#include "server/base.hpp"
#include "server/serialize.hpp"
#include "console.hpp"
#include "simple_ai.hpp"

namespace col{
    
    using simple_ai::SimpleAi;
    using simple_ai::ClassSimpleAi;
    
    template<class A>
    void write(A & f, Console & con) 
    {
		write(f, con.mcc);		
	}
	
	template<class A>
    void read(A & f, Console & con) 
    {
		read(f, con.mcc);		
	}
	
	
	template<class A>
    void write(A & f, SimpleAi & obj) 
    {
		write(f, obj.cc);			
	}
	
	template<class A>
    void read(A & f, SimpleAi & obj) 
    {
		
		read(f, obj.cc);	
	}
	

				
		
	inline void write_file(std::string const& fname, Runner & run) 
	{
		std::ofstream f(fname, std::ios::binary);
		
		write_mark(f, "ENV=");
		write(f, run.env);
		
		write_mark(f, "AGES");
		write<uint8_t>(f, run.agents.size());		
		for (size_t i = 0; i < run.agents.size(); ++i)
		{
			Agent * a = run.agents.at(i);
			write(f, a->get_class());
			switch (a->get_class())
			{
				case ClassConsole:
					write(f, *static_cast<Console*>(a));
					break;
				case ClassSimpleAi:
					write(f, *static_cast<SimpleAi*>(a));
					break;
				default:
					break;
			}
		}
		
	}
    
    inline void read_file(std::string const& fname, Runner & run) 
    {
		std::ifstream f(fname, std::ios::binary);
		
		read_mark(f, "ENV=");
		read(f, run.env);

		read_mark(f, "AGES");		
		uint8_t N = read<uint8_t>(f);
		print("read_file: N=%||\n", int(N));
		for (auto i = 0; i < N; ++i)
		{
			auto cls = read<Class>(f);
			switch (cls)
			{
				case ClassConsole: {
					Console & con = *static_cast<Console*>(run.agents.at(0));
					read(f, con);
					break;
				}
				case ClassSimpleAi: {
					run.add_agent<simple_ai::SimpleAi>(run.env, ControlNone);
					SimpleAi & smi = *static_cast<SimpleAi*>(run.agents.at(1));
					read(f, smi);
					break;
				}
				default: {
					fail("unknown class: code=%||; n=%||; fpos=%||\n", int(cls), i, f.tellg());
					break;
				}
			}

			
		}
		
		
		
	}
	
}
