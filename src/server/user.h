#ifndef USER_H
#define USER_H


namespace col{

	struct User{

		virtual void activate() = 0;
		// CONTROL FLOW
		// 1: master -> ai: activate
		//    ai -> master: sends commands
		//    ai -> master: ready
		//    goto 1 or 2
		// 2: master -> ai: win/lose

	};

}


#endif
