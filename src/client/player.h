#ifndef PLAYER_547547
#define PLAYER_547547

namespace col{

	struct Player {
		virtual void play(Env &, Nation::Id, Nation::Auth) = 0;

		virtual void start() {}
		virtual void stop() {}

		virtual ~Player() {}
	};
}


#endif
