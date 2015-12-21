#ifndef AGA2_HPP
#define AGA2_HPP
#include <cassert>
#include <array>
#include <cmath>
#include <iostream>
#include <type_traits>

namespace aga2 {

using uint = unsigned int;

uint const n = 2;

// safe casting...
template<class From, class To>
using is_safe_cast = std::is_same<
                     typename std::common_type<From, To>::type,
                     To
                     >;

template<class From, class To>
using enable_if_safe_cast = std::enable_if<
                            is_safe_cast<From, To>::value
                            >;


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

    using scalar_type = R;

    Array arr;

    // cast constructor from scalar type
    Mv0<R>(R const& a00) {
        arr[0] = a00;
    }

    // unpack
    void unpack(R & a00) const {
        a00 = arr[0];
    }

    // null constructor
    Mv0<R>() {
    }

    // explicit cast constructor
    template<class SrcType>
    explicit
    Mv0(Mv0<SrcType> const& other) {
        arr[0] = R(other[0]);
    }

    operator R() const {
        return arr[0];
    }

    R& operator[](uint const& d) {
        assert(d < size());
        return arr[d];
    }

    const R& operator[](uint const& d) const {
        assert(d < size());
        return arr[d];
    }

    uint size() const {
        return 1;
    }

    // operator=
    template <class A,
              typename enable_if_safe_cast<A,R>::type* = nullptr>
    void operator=(Mv0<A> const& x) {

        (*this)[0] = x[0];
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

    using scalar_type = R;

    Array arr;

    // cast constructor from scalar type
    Mv1<R>(R const& a10, R const& a11) {
        arr[0] = a10;
        arr[1] = a11;
    }

    // unpack
    void unpack(R & a10, R & a11) const {
        a10 = arr[0];
        a11 = arr[1];
    }

    // null constructor
    Mv1<R>() {
    }

    // explicit cast constructor
    template<class SrcType>
    explicit
    Mv1(Mv1<SrcType> const& other) {
        arr[0] = R(other[0]);
        arr[1] = R(other[1]);
    }

    R& operator[](uint const& d) {
        assert(d < size());
        return arr[d];
    }

    const R& operator[](uint const& d) const {
        assert(d < size());
        return arr[d];
    }

    uint size() const {
        return 2;
    }

    // operator=
    template <class A,
              typename enable_if_safe_cast<A,R>::type* = nullptr>
    void operator=(Mv1<A> const& x) {

        (*this)[0] = x[0];
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

    using scalar_type = R;

    Array arr;

    // cast constructor from scalar type
    explicit  Mv2<R>(R const& a20) {
        arr[0] = a20;
    }

    // unpack
    void unpack(R & a20) const {
        a20 = arr[0];
    }

    // null constructor
    Mv2<R>() {
    }

    // explicit cast constructor
    template<class SrcType>
    explicit
    Mv2(Mv2<SrcType> const& other) {
        arr[0] = R(other[0]);
    }

    explicit operator R() const {
        return arr[0];
    }

    R& operator[](uint const& d) {
        assert(d < size());
        return arr[d];
    }

    const R& operator[](uint const& d) const {
        assert(d < size());
        return arr[d];
    }

    uint size() const {
        return 1;
    }

    // operator=
    template <class A,
              typename enable_if_safe_cast<A,R>::type* = nullptr>
    void operator=(Mv2<A> const& x) {

        (*this)[0] = x[0];
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

    using scalar_type = R;

    Array arr;

    // cast constructor from scalar type
    Mv02<R>(R const& a00, R const& a20) {
        arr[0] = a00;
        arr[1] = a20;
    }

    // unpack
    void unpack(R & a00, R & a20) const {
        a00 = arr[0];
        a20 = arr[1];
    }

    // null constructor
    Mv02<R>() {
    }

    // explicit cast constructor
    template<class SrcType>
    explicit
    Mv02(Mv02<SrcType> const& other) {
        arr[0] = R(other[0]);
        arr[1] = R(other[1]);
    }

    R& operator[](uint const& d) {
        assert(d < size());
        return arr[d];
    }

    const R& operator[](uint const& d) const {
        assert(d < size());
        return arr[d];
    }

    uint size() const {
        return 2;
    }

    // operator=
    template <class A,
              typename enable_if_safe_cast<A,R>::type* = nullptr>
    void operator=(Mv02<A> const& x) {

        (*this)[0] = x[0];
        (*this)[1] = x[1];
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
template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv0<C> operator*(Mv0<A> const& x, Mv0<B> const& y) {

    return Mv0<C>((x[0] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator*(Mv0<A> const& x, Mv1<B> const& y) {

    return Mv1<C>((x[0] * y[0]), (x[0] * y[1]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv2<C> operator*(Mv0<A> const& x, Mv2<B> const& y) {

    return Mv2<C>((x[0] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator*(Mv0<A> const& x, Mv02<B> const& y) {

    return Mv02<C>((x[0] * y[0]), (x[0] * y[1]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator*(Mv1<A> const& x, Mv0<B> const& y) {

    return Mv1<C>((x[0] * y[0]), (x[1] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator*(Mv1<A> const& x, Mv1<B> const& y) {

    return Mv02<C>(((x[0] * y[0]) + (x[1] * y[1])), ((x[0] * y[1]) + (-1 * x[1] * y[0])));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator*(Mv1<A> const& x, Mv2<B> const& y) {

    return Mv1<C>((-1 * x[1] * y[0]), (x[0] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator*(Mv1<A> const& x, Mv02<B> const& y) {

    return Mv1<C>(((x[0] * y[0]) + (-1 * x[1] * y[1])), ((x[0] * y[1]) + (x[1] * y[0])));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv2<C> operator*(Mv2<A> const& x, Mv0<B> const& y) {

    return Mv2<C>((x[0] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator*(Mv2<A> const& x, Mv1<B> const& y) {

    return Mv1<C>((x[0] * y[1]), (-1 * x[0] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv0<C> operator*(Mv2<A> const& x, Mv2<B> const& y) {

    return Mv0<C>((-1 * x[0] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator*(Mv2<A> const& x, Mv02<B> const& y) {

    return Mv02<C>((-1 * x[0] * y[1]), (x[0] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator*(Mv02<A> const& x, Mv0<B> const& y) {

    return Mv02<C>((x[0] * y[0]), (x[1] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator*(Mv02<A> const& x, Mv1<B> const& y) {

    return Mv1<C>(((x[0] * y[0]) + (x[1] * y[1])), ((x[0] * y[1]) + (-1 * x[1] * y[0])));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator*(Mv02<A> const& x, Mv2<B> const& y) {

    return Mv02<C>((-1 * x[1] * y[0]), (x[0] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator*(Mv02<A> const& x, Mv02<B> const& y) {

    return Mv02<C>(((x[0] * y[0]) + (-1 * x[1] * y[1])), ((x[0] * y[1]) + (x[1] * y[0])));
}

// mul when one arg is Scalar
template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator*(A const& x, Mv1<B> const& y) {

    return Mv1<C>((x * y[0]), (x * y[1]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv2<C> operator*(A const& x, Mv2<B> const& y) {

    return Mv2<C>((x * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator*(A const& x, Mv02<B> const& y) {

    return Mv02<C>((x * y), (x * y[1]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator*(Mv1<A> const& x, B const& y) {

    return Mv1<C>((x[0] * y), (x[1] * y));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv2<C> operator*(Mv2<A> const& x, B const& y) {

    return Mv2<C>((x[0] * y));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator*(Mv02<A> const& x, B const& y) {

    return Mv02<C>((x * y), (x[1] * y));
}

// inner product
template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv0<C> operator|(Mv1<A> const& x, Mv1<B> const& y) {

    return Mv0<C>(((x[0] * y[0]) + (x[1] * y[1])));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator|(Mv1<A> const& x, Mv2<B> const& y) {

    return Mv1<C>((-1 * x[1] * y[0]), (x[0] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator|(Mv1<A> const& x, Mv02<B> const& y) {

    return Mv1<C>((-1 * x[1] * y[1]), (x[0] * y[1]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator|(Mv2<A> const& x, Mv1<B> const& y) {

    return Mv1<C>((x[0] * y[1]), (-1 * x[0] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv0<C> operator|(Mv2<A> const& x, Mv2<B> const& y) {

    return Mv0<C>((-1 * x[0] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv0<C> operator|(Mv2<A> const& x, Mv02<B> const& y) {

    return Mv0<C>((-1 * x[0] * y[1]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator|(Mv02<A> const& x, Mv1<B> const& y) {

    return Mv1<C>((x[1] * y[1]), (-1 * x[1] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv0<C> operator|(Mv02<A> const& x, Mv2<B> const& y) {

    return Mv0<C>((-1 * x[1] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv0<C> operator|(Mv02<A> const& x, Mv02<B> const& y) {

    return Mv0<C>((-1 * x[1] * y[1]));
}

// outer product
template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv0<C> operator^(Mv0<A> const& x, Mv0<B> const& y) {

    return Mv0<C>((x[0] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator^(Mv0<A> const& x, Mv1<B> const& y) {

    return Mv1<C>((x[0] * y[0]), (x[0] * y[1]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv2<C> operator^(Mv0<A> const& x, Mv2<B> const& y) {

    return Mv2<C>((x[0] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator^(Mv0<A> const& x, Mv02<B> const& y) {

    return Mv02<C>((x[0] * y[0]), (x[0] * y[1]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator^(Mv1<A> const& x, Mv0<B> const& y) {

    return Mv1<C>((x[0] * y[0]), (x[1] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv2<C> operator^(Mv1<A> const& x, Mv1<B> const& y) {

    return Mv2<C>(((x[0] * y[1]) + (-1 * x[1] * y[0])));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator^(Mv1<A> const& x, Mv02<B> const& y) {

    return Mv1<C>((x[0] * y[0]), (x[1] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv2<C> operator^(Mv2<A> const& x, Mv0<B> const& y) {

    return Mv2<C>((x[0] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv2<C> operator^(Mv2<A> const& x, Mv02<B> const& y) {

    return Mv2<C>((x[0] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator^(Mv02<A> const& x, Mv0<B> const& y) {

    return Mv02<C>((x[0] * y[0]), (x[1] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator^(Mv02<A> const& x, Mv1<B> const& y) {

    return Mv1<C>((x[0] * y[0]), (x[0] * y[1]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv2<C> operator^(Mv02<A> const& x, Mv2<B> const& y) {

    return Mv2<C>((x[0] * y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator^(Mv02<A> const& x, Mv02<B> const& y) {

    return Mv02<C>((x[0] * y[0]), ((x[0] * y[1]) + (x[1] * y[0])));
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
template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv0<C> operator+(Mv0<A> const& x, Mv0<B> const& y) {

    return Mv0<C>((x[0] + y[0]));
}

template <class A, class B,
          typename enable_if_safe_cast<B,A>::type* = nullptr>
void operator+=(Mv0<A> & x, Mv0<B> const& y) {

    x = x + y;
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator+(Mv0<A> const& x, Mv2<B> const& y) {

    return Mv02<C>(x[0], y[0]);
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator+(Mv0<A> const& x, Mv02<B> const& y) {

    return Mv02<C>((x[0] + y[0]), y[1]);
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator+(Mv1<A> const& x, Mv1<B> const& y) {

    return Mv1<C>((x[0] + y[0]), (x[1] + y[1]));
}

template <class A, class B,
          typename enable_if_safe_cast<B,A>::type* = nullptr>
void operator+=(Mv1<A> & x, Mv1<B> const& y) {

    x = x + y;
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator+(Mv2<A> const& x, Mv0<B> const& y) {

    return Mv02<C>(y[0], x[0]);
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv2<C> operator+(Mv2<A> const& x, Mv2<B> const& y) {

    return Mv2<C>((x[0] + y[0]));
}

template <class A, class B,
          typename enable_if_safe_cast<B,A>::type* = nullptr>
void operator+=(Mv2<A> & x, Mv2<B> const& y) {

    x = x + y;
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator+(Mv2<A> const& x, Mv02<B> const& y) {

    return Mv02<C>(y[0], (x[0] + y[1]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator+(Mv02<A> const& x, Mv0<B> const& y) {

    return Mv02<C>((x[0] + y[0]), x[1]);
}

template <class A, class B,
          typename enable_if_safe_cast<B,A>::type* = nullptr>
void operator+=(Mv02<A> & x, Mv0<B> const& y) {

    x = x + y;
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator+(Mv02<A> const& x, Mv2<B> const& y) {

    return Mv02<C>(x[0], (x[1] + y[0]));
}

template <class A, class B,
          typename enable_if_safe_cast<B,A>::type* = nullptr>
void operator+=(Mv02<A> & x, Mv2<B> const& y) {

    x = x + y;
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator+(Mv02<A> const& x, Mv02<B> const& y) {

    return Mv02<C>((x[0] + y[0]), (x[1] + y[1]));
}

template <class A, class B,
          typename enable_if_safe_cast<B,A>::type* = nullptr>
void operator+=(Mv02<A> & x, Mv02<B> const& y) {

    x = x + y;
}

// sub
template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv0<C> operator-(Mv0<A> const& x, Mv0<B> const& y) {

    return Mv0<C>((x[0] + (-y[0])));
}

template <class A, class B,
          typename enable_if_safe_cast<B,A>::type* = nullptr>
void operator-=(Mv0<A> & x, Mv0<B> const& y) {

    x = x - y;
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator-(Mv0<A> const& x, Mv2<B> const& y) {

    return Mv02<C>(x[0], (-y[0]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator-(Mv0<A> const& x, Mv02<B> const& y) {

    return Mv02<C>((x[0] + (-y[0])), (-y[1]));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> operator-(Mv1<A> const& x, Mv1<B> const& y) {

    return Mv1<C>((x[0] + (-y[0])), (x[1] + (-y[1])));
}

template <class A, class B,
          typename enable_if_safe_cast<B,A>::type* = nullptr>
void operator-=(Mv1<A> & x, Mv1<B> const& y) {

    x = x - y;
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator-(Mv2<A> const& x, Mv0<B> const& y) {

    return Mv02<C>((-y[0]), x[0]);
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv2<C> operator-(Mv2<A> const& x, Mv2<B> const& y) {

    return Mv2<C>((x[0] + (-y[0])));
}

template <class A, class B,
          typename enable_if_safe_cast<B,A>::type* = nullptr>
void operator-=(Mv2<A> & x, Mv2<B> const& y) {

    x = x - y;
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator-(Mv2<A> const& x, Mv02<B> const& y) {

    return Mv02<C>((-y[0]), (x[0] + (-y[1])));
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator-(Mv02<A> const& x, Mv0<B> const& y) {

    return Mv02<C>((x[0] + (-y[0])), x[1]);
}

template <class A, class B,
          typename enable_if_safe_cast<B,A>::type* = nullptr>
void operator-=(Mv02<A> & x, Mv0<B> const& y) {

    x = x - y;
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator-(Mv02<A> const& x, Mv2<B> const& y) {

    return Mv02<C>(x[0], (x[1] + (-y[0])));
}

template <class A, class B,
          typename enable_if_safe_cast<B,A>::type* = nullptr>
void operator-=(Mv02<A> & x, Mv2<B> const& y) {

    x = x - y;
}

template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv02<C> operator-(Mv02<A> const& x, Mv02<B> const& y) {

    return Mv02<C>((x[0] + (-y[0])), (x[1] + (-y[1])));
}

template <class A, class B,
          typename enable_if_safe_cast<B,A>::type* = nullptr>
void operator-=(Mv02<A> & x, Mv02<B> const& y) {

    x = x - y;
}

// rotated
template<class A, class B,
         class C = typename std::common_type<A, B>::type>

Mv1<C> rotated(Mv1<A> const& x, Mv02<B> const& y) {

    return Mv1<C>(((x[0] * pow(y[0], 2)) + (-1 * x[0] * pow(y[1], 2)) + (2 * x[1] * y[0] * y[1])), ((x[1] * pow(y[0], 2)) + (-1 * x[1] * pow(y[1], 2)) + (-2 * x[0] * y[0] * y[1])));
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
auto const _1 = Mv0<float>(1);
auto const e0 = Mv1<float>(1,0);
auto const e1 = Mv1<float>(0,1);
auto const I = Mv2<float>(1);

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
