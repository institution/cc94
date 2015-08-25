#ifndef FILESYS_HPP_4674375
#define FILESYS_HPP_4674375

#include <string>

namespace filesys{

	using Path = std::string;

	inline Path parent_path(Path const& p) {
		auto i = p.rfind('/');
		if (i != std::string::npos) {
			return p.substr(0, i);
		}
		else {
			return p;
		}
	}
	
	inline Path canonical(Path const& p) {
		return p;
	}

	inline bool is_absolute(Path const& p) {
		return p.size() > 0 and p[0] == '/';
	}

	inline Path operator/(Path const& a, Path const& b) {
		auto A = a.size();
		auto B = b.size();
		
		if (A == 0) {
			return b;
		}
		
		if (B == 0) {
			return a;
		}
		
		if (a[A-1] != '/' and b[B-1] != '/') {
			return a + std::string("/") + b;
		}
		
		if (a[A-1] == '/' and b[B-1] == '/') {
			return a + b.substr(1);
		}
		
		return a + b;	
	}
	
}
	

#endif
