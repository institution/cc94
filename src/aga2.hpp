#ifndef AGA2_HPP
#define AGA2_HPP
#include <cassert>
#include <array>
#include <cmath>
#include <iostream>

namespace aga2 {

const uint n = 2;

template <class R>
class Mv0 {
public:
    using Array = std::array<R,1>;

    using Iter = typename Array::iterator;

    Array arr;

    Mv0<R>(const R& a00) {
        arr[0] = a00;
    }

    Mv0<R>() {
    }

    operator R() {
        return arr[0];
    }

    R& operator[](const uint d) {
        assert(d < size());
        return arr[d];
    }

    template<class F>
    Mv0<F> cast() {
        return Mv0<F>(F(arr[0]));
    }

    const R& operator[](const uint d) const {
        assert(d < size());
        return arr[d];
    }

    uint size() const {
        return 1;
    }

    Iter begin() {
        return arr.begin();
    }

    Iter end() {
        return arr.end();
    }
};

template<class R = float> class Mv0;

template <class R>
class Mv1 {
public:
    using Array = std::array<R,2>;

    using Iter = typename Array::iterator;

    Array arr;

    Mv1<R>(const R& a10, const R& a11) {
        arr[0] = a10;
        arr[1] = a11;
    }

    Mv1<R>() {
    }

    R& operator[](const uint d) {
        assert(d < size());
        return arr[d];
    }

    template<class F>
    Mv1<F> cast() {
        return Mv1<F>(F(arr[0]), F(arr[1]));
    }

    const R& operator[](const uint d) const {
        assert(d < size());
        return arr[d];
    }

    uint size() const {
        return 2;
    }

    Iter begin() {
        return arr.begin();
    }

    Iter end() {
        return arr.end();
    }
};

template<class R = float> class Mv1;

template <class R>
class Mv2 {
public:
    using Array = std::array<R,1>;

    using Iter = typename Array::iterator;

    Array arr;

    explicit Mv2<R>(const R& a20) {
        arr[0] = a20;
    }

    Mv2<R>() {
    }

    explicit operator R() {
        return arr[0];
    }

    R& operator[](const uint d) {
        assert(d < size());
        return arr[d];
    }

    template<class F>
    Mv2<F> cast() {
        return Mv2<F>(F(arr[0]));
    }

    const R& operator[](const uint d) const {
        assert(d < size());
        return arr[d];
    }

    uint size() const {
        return 1;
    }

    Iter begin() {
        return arr.begin();
    }

    Iter end() {
        return arr.end();
    }
};

template<class R = float> class Mv2;

template <class R>
class Mv02 {
public:
    using Array = std::array<R,2>;

    using Iter = typename Array::iterator;

    Array arr;

    Mv02<R>(const R& a00, const R& a20) {
        arr[0] = a00;
        arr[1] = a20;
    }

    Mv02<R>() {
    }

    R& operator[](const uint d) {
        assert(d < size());
        return arr[d];
    }

    template<class F>
    Mv02<F> cast() {
        return Mv02<F>(F(arr[0]), F(arr[1]));
    }

    const R& operator[](const uint d) const {
        assert(d < size());
        return arr[d];
    }

    uint size() const {
        return 2;
    }

    Iter begin() {
        return arr.begin();
    }

    Iter end() {
        return arr.end();
    }
};

template<class R = float> class Mv02;

// eq
template <class R>
bool operator==(Mv0<R> const& x, Mv0<R> const& y) {

    return (x[0] == y[0]);
}

template <class R>
bool operator==(Mv1<R> const& x, Mv1<R> const& y) {

    return (x[0] == y[0]);
}

template <class R>
bool operator==(Mv2<R> const& x, Mv2<R> const& y) {

    return (x[0] == y[0]);
}

template <class R>
bool operator==(Mv02<R> const& x, Mv02<R> const& y) {

    return (x[0] == y[0]) && (x[1] == y[1]);
}

// neq
template <class R>
bool operator!=(Mv0<R> const& x, Mv0<R> const& y) {

    return (x[0] != y[0]);
}

template <class R>
bool operator!=(Mv1<R> const& x, Mv1<R> const& y) {

    return (x[0] != y[0]);
}

template <class R>
bool operator!=(Mv2<R> const& x, Mv2<R> const& y) {

    return (x[0] != y[0]);
}

template <class R>
bool operator!=(Mv02<R> const& x, Mv02<R> const& y) {

    return (x[0] != y[0]) || (x[1] != y[1]);
}

// mul
template <class R>
Mv0<R> operator*(Mv0<R> const& x, Mv0<R> const& y) {
    assert(&x != &y);

    return Mv0<R>((x[0] * y[0]));
}

template <class R>
Mv1<R> operator*(Mv0<R> const& x, Mv1<R> const& y) {

    return Mv1<R>((x[0] * y[0]), (x[0] * y[1]));
}

template <class R>
Mv2<R> operator*(Mv0<R> const& x, Mv2<R> const& y) {

    return Mv2<R>((x[0] * y[0]));
}

template <class R>
Mv02<R> operator*(Mv0<R> const& x, Mv02<R> const& y) {

    return Mv02<R>((x[0] * y[0]), (x[0] * y[1]));
}

template <class R>
Mv1<R> operator*(Mv1<R> const& x, Mv0<R> const& y) {

    return Mv1<R>((x[0] * y[0]), (x[1] * y[0]));
}

template <class R>
Mv02<R> operator*(Mv1<R> const& x, Mv1<R> const& y) {
    assert(&x != &y);

    return Mv02<R>(((x[1] * y[1]) + (x[0] * y[0])), ((-x[1]) + (x[0] * y[1])));
}

template <class R>
Mv1<R> operator*(Mv1<R> const& x, Mv2<R> const& y) {

    return Mv1<R>((-x[1]), (x[0] * y[0]));
}

template <class R>
Mv1<R> operator*(Mv1<R> const& x, Mv02<R> const& y) {

    return Mv1<R>(((-x[1]) + (x[0] * y[0])), ((x[0] * y[1]) + (x[1] * y[0])));
}

template <class R>
Mv2<R> operator*(Mv2<R> const& x, Mv0<R> const& y) {

    return Mv2<R>((x[0] * y[0]));
}

template <class R>
Mv1<R> operator*(Mv2<R> const& x, Mv1<R> const& y) {

    return Mv1<R>((x[0] * y[1]), (-x[0]));
}

template <class R>
Mv0<R> operator*(Mv2<R> const& x, Mv2<R> const& y) {
    assert(&x != &y);

    return Mv0<R>((-x[0]));
}

template <class R>
Mv02<R> operator*(Mv2<R> const& x, Mv02<R> const& y) {

    return Mv02<R>((-x[0]), (x[0] * y[0]));
}

template <class R>
Mv02<R> operator*(Mv02<R> const& x, Mv0<R> const& y) {

    return Mv02<R>((x[0] * y[0]), (x[1] * y[0]));
}

template <class R>
Mv1<R> operator*(Mv02<R> const& x, Mv1<R> const& y) {

    return Mv1<R>(((x[1] * y[1]) + (x[0] * y[0])), ((-x[1]) + (x[0] * y[1])));
}

template <class R>
Mv02<R> operator*(Mv02<R> const& x, Mv2<R> const& y) {

    return Mv02<R>((-x[1]), (x[0] * y[0]));
}

template <class R>
Mv02<R> operator*(Mv02<R> const& x, Mv02<R> const& y) {
    assert(&x != &y);

    return Mv02<R>(((-x[1]) + (x[0] * y[0])), ((x[0] * y[1]) + (x[1] * y[0])));
}

// inn
template <class R>
Mv0<R> operator|(Mv0<R> const& x, Mv0<R> const& y) {
    assert(&x != &y);

    return Mv0<R>((x[0] * y[0]));
}

template <class R>
Mv1<R> operator|(Mv0<R> const& x, Mv1<R> const& y) {

    return Mv1<R>((x[0] * y[0]), (x[0] * y[1]));
}

template <class R>
Mv2<R> operator|(Mv0<R> const& x, Mv2<R> const& y) {

    return Mv2<R>((x[0] * y[0]));
}

template <class R>
Mv02<R> operator|(Mv0<R> const& x, Mv02<R> const& y) {

    return Mv02<R>((x[0] * y[0]), (x[0] * y[1]));
}

template <class R>
Mv1<R> operator|(Mv1<R> const& x, Mv0<R> const& y) {

    return Mv1<R>((x[0] * y[0]), (x[1] * y[0]));
}

template <class R>
Mv0<R> operator|(Mv1<R> const& x, Mv1<R> const& y) {
    assert(&x != &y);

    return Mv0<R>(((x[1] * y[1]) + (x[0] * y[0])));
}

template <class R>
Mv1<R> operator|(Mv1<R> const& x, Mv2<R> const& y) {

    return Mv1<R>((-x[1]), (x[0] * y[0]));
}

template <class R>
Mv1<R> operator|(Mv1<R> const& x, Mv02<R> const& y) {

    return Mv1<R>(((-x[1]) + (x[0] * y[0])), ((x[0] * y[1]) + (x[1] * y[0])));
}

template <class R>
Mv2<R> operator|(Mv2<R> const& x, Mv0<R> const& y) {

    return Mv2<R>((x[0] * y[0]));
}

template <class R>
Mv1<R> operator|(Mv2<R> const& x, Mv1<R> const& y) {

    return Mv1<R>((x[0] * y[1]), (-x[0]));
}

template <class R>
Mv0<R> operator|(Mv2<R> const& x, Mv2<R> const& y) {
    assert(&x != &y);

    return Mv0<R>((-x[0]));
}

template <class R>
Mv02<R> operator|(Mv2<R> const& x, Mv02<R> const& y) {

    return Mv02<R>((-x[0]), (x[0] * y[0]));
}

template <class R>
Mv02<R> operator|(Mv02<R> const& x, Mv0<R> const& y) {

    return Mv02<R>((x[0] * y[0]), (x[1] * y[0]));
}

template <class R>
Mv1<R> operator|(Mv02<R> const& x, Mv1<R> const& y) {

    return Mv1<R>(((x[1] * y[1]) + (x[0] * y[0])), ((-x[1]) + (x[0] * y[1])));
}

template <class R>
Mv02<R> operator|(Mv02<R> const& x, Mv2<R> const& y) {

    return Mv02<R>((-x[1]), (x[0] * y[0]));
}

template <class R>
Mv02<R> operator|(Mv02<R> const& x, Mv02<R> const& y) {
    assert(&x != &y);

    return Mv02<R>(((-x[1]) + (x[0] * y[0])), ((x[0] * y[1]) + (x[1] * y[0])));
}

// inn2
template <class R>
Mv0<R> inn2(Mv0<R> const& x) {

    return Mv0<R>(pow(x[0], 2));
}

template <class R>
Mv0<R> inn2(Mv1<R> const& x) {

    return Mv0<R>((pow(x[0], 2) + pow(x[1], 2)));
}

template <class R>
Mv0<R> inn2(Mv2<R> const& x) {

    return Mv0<R>((-pow(x[0], 2)));
}

template <class R>
Mv02<R> inn2(Mv02<R> const& x) {

    return Mv02<R>((pow(x[0], 2) + (-pow(x[1], 2))), (2 * x[0]));
}

// add
template <class R>
Mv0<R> operator+(Mv0<R> const& x, Mv0<R> const& y) {
    assert(&x != &y);

    return Mv0<R>((x[0] + y[0]));
}

template <class R>
void operator+=(Mv0<R> & x, Mv0<R> const& y) {

    x = x + y;
}

template <class R>
Mv02<R> operator+(Mv0<R> const& x, Mv2<R> const& y) {

    return Mv02<R>(x[0], y[0]);
}

template <class R>
Mv02<R> operator+(Mv0<R> const& x, Mv02<R> const& y) {

    return Mv02<R>((x[0] + y[0]), y[1]);
}

template <class R>
Mv1<R> operator+(Mv1<R> const& x, Mv1<R> const& y) {
    assert(&x != &y);

    return Mv1<R>((x[0] + y[0]), (x[1] + y[1]));
}

template <class R>
void operator+=(Mv1<R> & x, Mv1<R> const& y) {

    x = x + y;
}

template <class R>
Mv02<R> operator+(Mv2<R> const& x, Mv0<R> const& y) {

    return Mv02<R>(y[0], x[0]);
}

template <class R>
Mv2<R> operator+(Mv2<R> const& x, Mv2<R> const& y) {
    assert(&x != &y);

    return Mv2<R>((x[0] + y[0]));
}

template <class R>
void operator+=(Mv2<R> & x, Mv2<R> const& y) {

    x = x + y;
}

template <class R>
Mv02<R> operator+(Mv2<R> const& x, Mv02<R> const& y) {

    return Mv02<R>(y[0], (x[0] + y[1]));
}

template <class R>
Mv02<R> operator+(Mv02<R> const& x, Mv0<R> const& y) {

    return Mv02<R>((x[0] + y[0]), x[1]);
}

template <class R>
void operator+=(Mv02<R> & x, Mv0<R> const& y) {

    x = x + y;
}

template <class R>
Mv02<R> operator+(Mv02<R> const& x, Mv2<R> const& y) {

    return Mv02<R>(x[0], (x[1] + y[0]));
}

template <class R>
void operator+=(Mv02<R> & x, Mv2<R> const& y) {

    x = x + y;
}

template <class R>
Mv02<R> operator+(Mv02<R> const& x, Mv02<R> const& y) {
    assert(&x != &y);

    return Mv02<R>((x[0] + y[0]), (x[1] + y[1]));
}

template <class R>
void operator+=(Mv02<R> & x, Mv02<R> const& y) {

    x = x + y;
}

// sub
template <class R>
Mv0<R> operator-(Mv0<R> const& x, Mv0<R> const& y) {
    assert(&x != &y);

    return Mv0<R>(((-y[0]) + x[0]));
}

template <class R>
Mv02<R> operator-(Mv0<R> const& x, Mv2<R> const& y) {

    return Mv02<R>(x[0], (-y[0]));
}

template <class R>
Mv02<R> operator-(Mv0<R> const& x, Mv02<R> const& y) {

    return Mv02<R>(((-y[0]) + x[0]), (-y[1]));
}

template <class R>
Mv1<R> operator-(Mv1<R> const& x, Mv1<R> const& y) {
    assert(&x != &y);

    return Mv1<R>(((-y[0]) + x[0]), ((-y[1]) + x[1]));
}

template <class R>
Mv02<R> operator-(Mv2<R> const& x, Mv0<R> const& y) {

    return Mv02<R>((-y[0]), x[0]);
}

template <class R>
Mv2<R> operator-(Mv2<R> const& x, Mv2<R> const& y) {
    assert(&x != &y);

    return Mv2<R>(((-y[0]) + x[0]));
}

template <class R>
Mv02<R> operator-(Mv2<R> const& x, Mv02<R> const& y) {

    return Mv02<R>((-y[0]), ((-y[1]) + x[0]));
}

template <class R>
Mv02<R> operator-(Mv02<R> const& x, Mv0<R> const& y) {

    return Mv02<R>(((-y[0]) + x[0]), x[1]);
}

template <class R>
Mv02<R> operator-(Mv02<R> const& x, Mv2<R> const& y) {

    return Mv02<R>(x[0], ((-y[0]) + x[1]));
}

template <class R>
Mv02<R> operator-(Mv02<R> const& x, Mv02<R> const& y) {
    assert(&x != &y);

    return Mv02<R>(((-y[0]) + x[0]), ((-y[1]) + x[1]));
}

// ostream
template <class R>
std::ostream& operator<<(std::ostream& t, const Mv0<R>& x) {
    t << "Mv0<R>(" << x[0] << ")";
}

template <class R>
std::ostream& operator<<(std::ostream& t, const Mv1<R>& x) {
    t << "Mv1<R>(" << x[0] << "," << x[1] << ")";
}

template <class R>
std::ostream& operator<<(std::ostream& t, const Mv2<R>& x) {
    t << "Mv2<R>(" << x[0] << ")";
}

template <class R>
std::ostream& operator<<(std::ostream& t, const Mv02<R>& x) {
    t << "Mv02<R>(" << x[0] << "," << x[1] << ")";
}

// const
const Mv1<float> e0(1,0);
const Mv1<float> e1(0,1);
const Mv2<float> I(1);

} // aga2

#endif // AGA2_HPP
