#ifndef AGA2_HPP
#define AGA2_HPP
#include <cassert>
#include <array>
#include <cmath>
#include <iostream>

namespace aga2 {

using uint = unsigned int;

uint const n = 2;

template <class R>
class Mv0 {
public:
    using Array = std::array<R,1>;

    using Iter = typename Array::iterator;
    using iterator = typename Array::iterator;
    using const_iterator = typename Array::const_iterator;
    using value_type = typename Array::value_type;
    using reference = typename Array::reference;
    using const_reference = typename Array::const_reference;
    using pointer = typename Array::pointer;
    using difference_type = typename Array::difference_type;
    using size_type = typename Array::size_type;

    Array arr;

    Mv0<R>(R const& a00) {
        arr[0] = a00;
    }

    Mv0<R>() {
    }

    operator R() const {
        return arr[0];
    }

    R& operator[](uint const& d) {
        assert(d < size());
        return arr[d];
    }

    template<class F>
    Mv0<F> cast() const {
        return Mv0<F>(F(arr[0]));
    }

    const R& operator[](uint const& d) const {
        assert(d < size());
        return arr[d];
    }

    uint size() const {
        return 1;
    }

    R norm2() const {

        auto& x = *this;
        return pow(x[0], 2);
    }


    Mv0<R> rev() const {

        auto& x = *this;
        return Mv0<R>(x[0]);
    }


    Mv0<R> inv() const {

        auto& x = *this;
        return Mv0<R>(pow(x[0], -1));
    }


    Mv0<R> operator-() const {

        auto& x = *this;
        return Mv0<R>((-x[0]));
    }


    R const* data() const {
        return arr.data();
    }

    bool empty() const {
        return false;
    }

    iterator begin() {
        return arr.begin();
    }

    iterator end() {
        return arr.end();
    }

    const_iterator begin() const {
        return arr.begin();
    }

    const_iterator end() const {
        return arr.end();
    }

    void swap(Mv0& b) {
        assert(false);
    }

    template<class A>
    void serialize(A & ar, unsigned int const version) {
        for (auto& c: *this) {
            ar & c;
        }
    }
};

template<class R = float> class Mv0;

template <class R>
class Mv1 {
public:
    using Array = std::array<R,2>;

    using Iter = typename Array::iterator;
    using iterator = typename Array::iterator;
    using const_iterator = typename Array::const_iterator;
    using value_type = typename Array::value_type;
    using reference = typename Array::reference;
    using const_reference = typename Array::const_reference;
    using pointer = typename Array::pointer;
    using difference_type = typename Array::difference_type;
    using size_type = typename Array::size_type;

    Array arr;

    Mv1<R>(R const& a10, R const& a11) {
        arr[0] = a10;
        arr[1] = a11;
    }

    Mv1<R>() {
    }

    R& operator[](uint const& d) {
        assert(d < size());
        return arr[d];
    }

    template<class F>
    Mv1<F> cast() const {
        return Mv1<F>(F(arr[0]), F(arr[1]));
    }

    const R& operator[](uint const& d) const {
        assert(d < size());
        return arr[d];
    }

    uint size() const {
        return 2;
    }

    R norm2() const {

        auto& x = *this;
        return (pow(x[0], 2) + pow(x[1], 2));
    }


    Mv1<R> rev() const {

        auto& x = *this;
        return Mv1<R>(x[0], x[1]);
    }


    Mv1<R> inv() const {

        auto& x = *this;
        return Mv1<R>((x[0] * pow((pow(x[0], 2) + pow(x[1], 2)), -1)), (x[1] * pow((pow(x[0], 2) + pow(x[1], 2)), -1)));
    }


    Mv1<R> operator-() const {

        auto& x = *this;
        return Mv1<R>((-x[0]), (-x[1]));
    }


    R const* data() const {
        return arr.data();
    }

    bool empty() const {
        return false;
    }

    iterator begin() {
        return arr.begin();
    }

    iterator end() {
        return arr.end();
    }

    const_iterator begin() const {
        return arr.begin();
    }

    const_iterator end() const {
        return arr.end();
    }

    void swap(Mv1& b) {
        assert(false);
    }

    template<class A>
    void serialize(A & ar, unsigned int const version) {
        for (auto& c: *this) {
            ar & c;
        }
    }
};

template<class R = float> class Mv1;

template <class R>
class Mv2 {
public:
    using Array = std::array<R,1>;

    using Iter = typename Array::iterator;
    using iterator = typename Array::iterator;
    using const_iterator = typename Array::const_iterator;
    using value_type = typename Array::value_type;
    using reference = typename Array::reference;
    using const_reference = typename Array::const_reference;
    using pointer = typename Array::pointer;
    using difference_type = typename Array::difference_type;
    using size_type = typename Array::size_type;

    Array arr;

    explicit Mv2<R>(R const& a20) {
        arr[0] = a20;
    }

    Mv2<R>() {
    }

    explicit operator R() const {
        return arr[0];
    }

    R& operator[](uint const& d) {
        assert(d < size());
        return arr[d];
    }

    template<class F>
    Mv2<F> cast() const {
        return Mv2<F>(F(arr[0]));
    }

    const R& operator[](uint const& d) const {
        assert(d < size());
        return arr[d];
    }

    uint size() const {
        return 1;
    }

    R norm2() const {

        auto& x = *this;
        return pow(x[0], 2);
    }


    Mv2<R> rev() const {

        auto& x = *this;
        return Mv2<R>((-x[0]));
    }


    Mv2<R> inv() const {

        auto& x = *this;
        return Mv2<R>((-pow(x[0], -1)));
    }


    Mv2<R> operator-() const {

        auto& x = *this;
        return Mv2<R>((-x[0]));
    }


    R const* data() const {
        return arr.data();
    }

    bool empty() const {
        return false;
    }

    iterator begin() {
        return arr.begin();
    }

    iterator end() {
        return arr.end();
    }

    const_iterator begin() const {
        return arr.begin();
    }

    const_iterator end() const {
        return arr.end();
    }

    void swap(Mv2& b) {
        assert(false);
    }

    template<class A>
    void serialize(A & ar, unsigned int const version) {
        for (auto& c: *this) {
            ar & c;
        }
    }
};

template<class R = float> class Mv2;

template <class R>
class Mv02 {
public:
    using Array = std::array<R,2>;

    using Iter = typename Array::iterator;
    using iterator = typename Array::iterator;
    using const_iterator = typename Array::const_iterator;
    using value_type = typename Array::value_type;
    using reference = typename Array::reference;
    using const_reference = typename Array::const_reference;
    using pointer = typename Array::pointer;
    using difference_type = typename Array::difference_type;
    using size_type = typename Array::size_type;

    Array arr;

    Mv02<R>(R const& a00, R const& a20) {
        arr[0] = a00;
        arr[1] = a20;
    }

    Mv02<R>() {
    }

    R& operator[](uint const& d) {
        assert(d < size());
        return arr[d];
    }

    template<class F>
    Mv02<F> cast() const {
        return Mv02<F>(F(arr[0]), F(arr[1]));
    }

    const R& operator[](uint const& d) const {
        assert(d < size());
        return arr[d];
    }

    uint size() const {
        return 2;
    }

    R norm2() const {

        auto& x = *this;
        return (pow(x[0], 2) + pow(x[1], 2));
    }


    Mv02<R> rev() const {

        auto& x = *this;
        return Mv02<R>(x[0], (-x[1]));
    }


    Mv02<R> inv() const {

        auto& x = *this;
        return Mv02<R>((x[0] * pow((pow(x[0], 2) + pow(x[1], 2)), -1)), (-1 * x[1] * pow((pow(x[0], 2) + pow(x[1], 2)), -1)));
    }


    Mv02<R> operator-() const {

        auto& x = *this;
        return Mv02<R>((-x[0]), (-x[1]));
    }


    R const* data() const {
        return arr.data();
    }

    bool empty() const {
        return false;
    }

    iterator begin() {
        return arr.begin();
    }

    iterator end() {
        return arr.end();
    }

    const_iterator begin() const {
        return arr.begin();
    }

    const_iterator end() const {
        return arr.end();
    }

    void swap(Mv02& b) {
        assert(false);
    }

    template<class A>
    void serialize(A & ar, unsigned int const version) {
        for (auto& c: *this) {
            ar & c;
        }
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

    return (x[0] == y[0]) && (x[1] == y[1]);
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

    return (x[0] != y[0]) || (x[1] != y[1]);
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

    return Mv02<R>(((x[0] * y[0]) + (x[1] * y[1])), ((-1 * x[1] * y[0]) + (x[0] * y[1])));
}

template <class R>
Mv1<R> operator*(Mv1<R> const& x, Mv2<R> const& y) {

    return Mv1<R>((-1 * x[1] * y[0]), (x[0] * y[0]));
}

template <class R>
Mv1<R> operator*(Mv1<R> const& x, Mv02<R> const& y) {

    return Mv1<R>(((x[0] * y[0]) + (-1 * x[1] * y[1])), ((x[0] * y[1]) + (x[1] * y[0])));
}

template <class R>
Mv2<R> operator*(Mv2<R> const& x, Mv0<R> const& y) {

    return Mv2<R>((x[0] * y[0]));
}

template <class R>
Mv1<R> operator*(Mv2<R> const& x, Mv1<R> const& y) {

    return Mv1<R>((x[0] * y[1]), (-1 * x[0] * y[0]));
}

template <class R>
Mv0<R> operator*(Mv2<R> const& x, Mv2<R> const& y) {
    assert(&x != &y);

    return Mv0<R>((-1 * x[0] * y[0]));
}

template <class R>
Mv02<R> operator*(Mv2<R> const& x, Mv02<R> const& y) {

    return Mv02<R>((-1 * x[0] * y[1]), (x[0] * y[0]));
}

template <class R>
Mv02<R> operator*(Mv02<R> const& x, Mv0<R> const& y) {

    return Mv02<R>((x[0] * y[0]), (x[1] * y[0]));
}

template <class R>
Mv1<R> operator*(Mv02<R> const& x, Mv1<R> const& y) {

    return Mv1<R>(((x[0] * y[0]) + (x[1] * y[1])), ((-1 * x[1] * y[0]) + (x[0] * y[1])));
}

template <class R>
Mv02<R> operator*(Mv02<R> const& x, Mv2<R> const& y) {

    return Mv02<R>((-1 * x[1] * y[0]), (x[0] * y[0]));
}

template <class R>
Mv02<R> operator*(Mv02<R> const& x, Mv02<R> const& y) {
    assert(&x != &y);

    return Mv02<R>(((x[0] * y[0]) + (-1 * x[1] * y[1])), ((x[0] * y[1]) + (x[1] * y[0])));
}

// inner product
template <class R>
Mv0<R> operator|(Mv1<R> const& x, Mv1<R> const& y) {
    assert(&x != &y);

    return Mv0<R>(((x[0] * y[0]) + (x[1] * y[1])));
}

template <class R>
Mv1<R> operator|(Mv1<R> const& x, Mv2<R> const& y) {

    return Mv1<R>((-1 * x[1] * y[0]), (x[0] * y[0]));
}

template <class R>
Mv1<R> operator|(Mv1<R> const& x, Mv02<R> const& y) {

    return Mv1<R>((-1 * x[1] * y[1]), (x[0] * y[1]));
}

template <class R>
Mv1<R> operator|(Mv2<R> const& x, Mv1<R> const& y) {

    return Mv1<R>((x[0] * y[1]), (-1 * x[0] * y[0]));
}

template <class R>
Mv0<R> operator|(Mv2<R> const& x, Mv2<R> const& y) {
    assert(&x != &y);

    return Mv0<R>((-1 * x[0] * y[0]));
}

template <class R>
Mv0<R> operator|(Mv2<R> const& x, Mv02<R> const& y) {

    return Mv0<R>((-1 * x[0] * y[1]));
}

template <class R>
Mv1<R> operator|(Mv02<R> const& x, Mv1<R> const& y) {

    return Mv1<R>((x[1] * y[1]), (-1 * x[1] * y[0]));
}

template <class R>
Mv0<R> operator|(Mv02<R> const& x, Mv2<R> const& y) {

    return Mv0<R>((-1 * x[1] * y[0]));
}

template <class R>
Mv0<R> operator|(Mv02<R> const& x, Mv02<R> const& y) {
    assert(&x != &y);

    return Mv0<R>((-1 * x[1] * y[1]));
}

// outer product
template <class R>
Mv0<R> operator^(Mv0<R> const& x, Mv0<R> const& y) {
    assert(&x != &y);

    return Mv0<R>((x[0] * y[0]));
}

template <class R>
Mv1<R> operator^(Mv0<R> const& x, Mv1<R> const& y) {

    return Mv1<R>((x[0] * y[0]), (x[0] * y[1]));
}

template <class R>
Mv2<R> operator^(Mv0<R> const& x, Mv2<R> const& y) {

    return Mv2<R>((x[0] * y[0]));
}

template <class R>
Mv02<R> operator^(Mv0<R> const& x, Mv02<R> const& y) {

    return Mv02<R>((x[0] * y[0]), (x[0] * y[1]));
}

template <class R>
Mv1<R> operator^(Mv1<R> const& x, Mv0<R> const& y) {

    return Mv1<R>((x[0] * y[0]), (x[1] * y[0]));
}

template <class R>
Mv2<R> operator^(Mv1<R> const& x, Mv1<R> const& y) {
    assert(&x != &y);

    return Mv2<R>(((-1 * x[1] * y[0]) + (x[0] * y[1])));
}

template <class R>
Mv1<R> operator^(Mv1<R> const& x, Mv02<R> const& y) {

    return Mv1<R>((x[0] * y[0]), (x[1] * y[0]));
}

template <class R>
Mv2<R> operator^(Mv2<R> const& x, Mv0<R> const& y) {

    return Mv2<R>((x[0] * y[0]));
}

template <class R>
Mv2<R> operator^(Mv2<R> const& x, Mv02<R> const& y) {

    return Mv2<R>((x[0] * y[0]));
}

template <class R>
Mv02<R> operator^(Mv02<R> const& x, Mv0<R> const& y) {

    return Mv02<R>((x[0] * y[0]), (x[1] * y[0]));
}

template <class R>
Mv1<R> operator^(Mv02<R> const& x, Mv1<R> const& y) {

    return Mv1<R>((x[0] * y[0]), (x[0] * y[1]));
}

template <class R>
Mv2<R> operator^(Mv02<R> const& x, Mv2<R> const& y) {

    return Mv2<R>((x[0] * y[0]));
}

template <class R>
Mv02<R> operator^(Mv02<R> const& x, Mv02<R> const& y) {
    assert(&x != &y);

    return Mv02<R>((x[0] * y[0]), ((x[0] * y[1]) + (x[1] * y[0])));
}

// inn2
template <class R>
Mv0<R> inn2(Mv1<R> const& x) {

    return Mv0<R>((pow(x[0], 2) + pow(x[1], 2)));
}

template <class R>
Mv0<R> inn2(Mv2<R> const& x) {

    return Mv0<R>((-pow(x[0], 2)));
}

template <class R>
Mv0<R> inn2(Mv02<R> const& x) {

    return Mv0<R>((-pow(x[1], 2)));
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
void operator-=(Mv0<R> & x, Mv0<R> const& y) {

    x = x - y;
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

    return Mv1<R>(((-y[0]) + x[0]), (x[1] + (-y[1])));
}

template <class R>
void operator-=(Mv1<R> & x, Mv1<R> const& y) {

    x = x - y;
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
void operator-=(Mv2<R> & x, Mv2<R> const& y) {

    x = x - y;
}

template <class R>
Mv02<R> operator-(Mv2<R> const& x, Mv02<R> const& y) {

    return Mv02<R>((-y[0]), (x[0] + (-y[1])));
}

template <class R>
Mv02<R> operator-(Mv02<R> const& x, Mv0<R> const& y) {

    return Mv02<R>(((-y[0]) + x[0]), x[1]);
}

template <class R>
void operator-=(Mv02<R> & x, Mv0<R> const& y) {

    x = x - y;
}

template <class R>
Mv02<R> operator-(Mv02<R> const& x, Mv2<R> const& y) {

    return Mv02<R>(x[0], ((-y[0]) + x[1]));
}

template <class R>
void operator-=(Mv02<R> & x, Mv2<R> const& y) {

    x = x - y;
}

template <class R>
Mv02<R> operator-(Mv02<R> const& x, Mv02<R> const& y) {
    assert(&x != &y);

    return Mv02<R>(((-y[0]) + x[0]), (x[1] + (-y[1])));
}

template <class R>
void operator-=(Mv02<R> & x, Mv02<R> const& y) {

    x = x - y;
}

// rotated
template <class R>
Mv1<R> rotated(Mv1<R> const& x, Mv02<R> const& y) {

    return Mv1<R>(((2 * x[1] * y[0] * y[1]) + (x[0] * pow(y[0], 2)) + (-1 * x[0] * pow(y[1], 2))), ((-2 * x[0] * y[0] * y[1]) + (x[1] * pow(y[0], 2)) + (-1 * x[1] * pow(y[1], 2))));
}

// ostream
template <class R>
std::ostream& operator<<(std::ostream& t, const Mv0<R>& x) {
    t << "Mv0<R>(" << x[0] << ")";
    return t;
}

template <class R>
std::ostream& operator<<(std::ostream& t, const Mv1<R>& x) {
    t << "Mv1<R>(" << x[0] << "," << x[1] << ")";
    return t;
}

template <class R>
std::ostream& operator<<(std::ostream& t, const Mv2<R>& x) {
    t << "Mv2<R>(" << x[0] << ")";
    return t;
}

template <class R>
std::ostream& operator<<(std::ostream& t, const Mv02<R>& x) {
    t << "Mv02<R>(" << x[0] << "," << x[1] << ")";
    return t;
}

// const
auto const _1 = Mv0<>(1);
auto const e0 = Mv1<>(1,0);
auto const e1 = Mv1<>(0,1);
auto const I = Mv2<>(1);

// misc
/*
nplane -- normal plane

angle -- angle [1]

return -- rotor
*/

template <class R>
Mv02<R> rotor(Mv2<R> const& nplane, R const& angle) {

    return Mv0<R>(cos(angle/2.0)) - nplane * Mv0<R>(sin(angle/2.0));
}

} // aga2

#endif // AGA2_HPP
