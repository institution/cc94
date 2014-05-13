#include "envgame.h"


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


namespace col {



	void copy_det(EnvGame &trg, EnvGame const& src, Player::Id const& pid)
	{

		// copy availbe information
		trg.clear();

		trg.uts = src.uts;
		trg.tts = src.tts;
		trg.bts = src.bts;

		std::stringstream f;

		{
			boost::archive::text_oarchive ar(f);
			ar << src;
		}

		{
			boost::archive::text_iarchive ar(f);
			ar >> trg;
		}

		trg.action_count = src.action_count;
		
		// determinisation
		//roll::seed();

	}


}