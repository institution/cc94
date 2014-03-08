#ifndef _H
#define _H

#include <vector>
#include <iostream>
#include <functional>
#include <stdexcept>
#include <string>
#include <boost/any.hpp>


template <typename Ret, typename... Args>
Ret callfunc (std::function<Ret(Args...)> func, std::vector<boost::any> anyargs);

template <typename Ret>
Ret callfunc (std::function<Ret()> func, std::vector<boost::any> anyargs)
{
    if (anyargs.size() > 0)
        throw std::runtime_error("oops, argument list too long");
    return func();
}

template <typename Ret, typename Arg0, typename... Args>
Ret callfunc (std::function<Ret(Arg0, Args...)> func, std::vector<boost::any> anyargs)
{
    if (anyargs.size() == 0)
        throw std::runtime_error("oops, argument list too short");
    Arg0 arg0 = boost::any_cast<Arg0>(anyargs[0]);
    anyargs.erase(anyargs.begin());
    std::function<Ret(Args... args)> lambda =
        ([=](Args... args) -> Ret {
         return func(arg0, args...);
    });
    return callfunc (lambda, anyargs);
}

template <typename Ret, typename... Args>
std::function<boost::any(std::vector<boost::any>)> adaptfunc (Ret (*func)(Args...)) {
    std::function<Ret(Args...)> stdfunc = func;
    std::function<boost::any(std::vector<boost::any>)> result =
        ([=](std::vector<boost::any> anyargs) -> boost::any {
         return boost::any(callfunc(stdfunc, anyargs));
         });
    return result;
}



int func1 (int a)
{
    std::cout << "func1(" << a << ") = ";
    return 33;
}

int func2 (double a, std::string b)
{
    std::cout << "func2(" << a << ",\"" << b << "\") = ";
    return 7;
}

int func3 (std::string a, double b)
{
    std::cout << "func3(" << a << ",\"" << b << "\") = ";
    return 7;
}

int func4 (int a, int b)
{
    std::cout << "func4(" << a << "," << b << ") = ";
    return a+b;
}


int main ()
{
    std::vector<std::function<boost::any(std::vector<boost::any>)>> fcs = {
        adaptfunc(func1), adaptfunc(func2), adaptfunc(func3), adaptfunc(func4) };

    std::vector<std::vector<boost::any>> args =
    {{777}, {66.6, std::string("yeah right")}, {std::string("whatever"), 0.123}, {3, 2}};

    // correct calls will succeed
    for (int i = 0; i < fcs.size(); ++i)
        std::cout << boost::any_cast<int>(fcs[i](args[i])) << std::endl;

    // incorrect calls will throw
    for (int i = 0; i < fcs.size(); ++i)
        try {
            std::cout << boost::any_cast<int>(fcs[i](args[fcs.size()-1-i])) << std::endl;
        } catch (std::exception& e) {
            std::cout << "Could not call, got exception: " << e.what() << std::endl;
        }
}


#endif
