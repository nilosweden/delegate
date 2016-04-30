#include <iostream>
#include "delegate.hpp"

class base
{
public:
    base()
    {}

    void operator()(int x)
    {
        std::cerr << "Im operator () in Base, x is: " << x << std::endl;
    }

    virtual void print(int x)
    {
        std::cerr << "Im Bases print function: " << x << std::endl;
    }
};

class derived : public base
{
public:
    derived()
    {}

    void print(int x)
    {
        std::cerr << "Im derived print function: " << x << std::endl;
    }
};


void func(int x)
{
    std::cerr << "Im Func() in main: " << x << std::endl;
}

int main()
{
    base b;
    derived d;
    fsx::delegate<void(int)> delegate;

    //Bind lambda
    delegate.bind([](int x){
        std::cerr << "Im lambda: " << x << std::endl;
    });

    //Bind capturing lambda
    delegate.bind([&](int x){
        std::cerr << "Im capturing lambda, Ill call base print function now, btw x is: " << x << std::endl;
        b.print(x);
    });

    //Call operator ()
    delegate.bind(b);

    //Bind basic func
    delegate.bind<&func>();

    //Bind member function
    delegate.bind<base, &base::print>(b);

    //Bind derived
    delegate.bind<base, &base::print>(d);

    //Invoke and call all
    delegate.invoke(10);
    return 0;
}
