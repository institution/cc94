//#include <X11/Xlib.h>  // XInitThreads
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string>


namespace os{

	inline std::string get_home_dir() {
		struct passwd *pw = getpwuid(getuid());

		const char *homedir = pw->pw_dir;

		return std::string(homedir);
	}

}
