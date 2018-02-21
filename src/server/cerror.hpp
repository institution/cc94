#pragma once

#include "base.hpp"

namespace col {
	/// This will store FIRST error that was set
	struct CError
	{
		std::string error;
		
		template <class... Args>
		void set(char const* fmt, Args... args)
		{
			if (error.empty())
			{
				error = format(fmt, args...);
			}
		}
		
		std::string const& get() const { return error; }
		bool has() const { return not error.empty(); }
		void clear() { error.clear(); }

		operator bool() const { return has(); }
	};
}
