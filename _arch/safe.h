#ifndef NUM_H
#define NUM_H

template <typename T>
struct Num {
	T t;
	
	Num() {}
	
	explicit Num(const T &tt): t(tt) {		
	}
	
};



template <typename T> 
inline 
Num<T> operator*(Num<T> const& a, Num<T> const& b) {
	return Num<T>(a.t * a.t);
}

template <typename T> 
inline 
Num<T> operator/(Num<T> const& a, Num<T> const& b) {
	return Num<T>(a.t / a.t);
}

template <typename T> 
inline 
Num<T> operator+(Num<T> const& a, Num<T> const& b) {
	return Num<T>(a.t + a.t);
}

template <typename T> 
inline 
Num<T> operator-(Num<T> const& a, Num<T> const& b) {
	return Num<T>(a.t - a.t);
}

template <typename T> 
inline 
bool operator==(Num<T> const& a, Num<T> const& b) {
	return a.t == b.t;
}

template <typename T> 
inline 
bool operator!=(Num<T> const& a, Num<T> const& b) {
	return a.t != b.t;
}

template <typename T> 
inline 
bool operator>(Num<T> const& a, Num<T> const& b) {
	return a.t > b.t;
}

template <typename T> 
inline 
bool operator<(Num<T> const& a, Num<T> const& b) {
	return a.t < b.t;
}

template <typename T> 
inline 
bool operator>=(Num<T> const& a, Num<T> const& b) {
	return a.t >= b.t;
}

template <typename T> 
inline 
bool operator<=(Num<T> const& a, Num<T> const& b) {
	return a.t <= b.t;
}

template <typename T> 
inline 
Num<T> operator+(Num<T> const& a) {
	return Num<T>(+a.t);
}

template <typename T> 
inline 
Num<T> operator-(Num<T> const& a) {
	return Num<T>(-a.t);
}

template <typename T> 
inline 
Num<T> operator%(Num<T> const& a, Num<T> const& b) {
	return Num<T>(a.t % b.t);
}

template <typename T> 
inline 
Num<T>& operator++(Num<T> const& a) {
	++a.t;
	return a;
}

template <typename T> 
inline 
Num<T> operator++(Num<T> const& a, int) {
	return Num<T>(a.t++);
}

template <typename T> 
inline 
Num<T>& operator--(Num<T> const& a) {
	--a.t;
	return a;
}

template <typename T> 
inline 
Num<T> operator--(Num<T> const& a, int) {
	return Num<T>(a.t--);
}


#endif
