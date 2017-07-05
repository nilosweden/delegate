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

    // bind lambda
    delegate.bind([](int x){
        std::cerr << "Im lambda: " << x << std::endl;
    });

    // bind capturing lambda
    delegate.bind([&](int x){
        std::cerr << "Im capturing lambda, Ill call base print function now, btw x is: " << x << std::endl;
        b.print(x);
    });

    // call operator ()
    delegate.bind(b);

    // bind basic func
    delegate.bind<&func>();

    // bind member function
    delegate.bind<base, &base::print>(b);

    // bind derived
    delegate.bind<base, &base::print>(d);

    // invoke and call all
    delegate.invoke(10);
}
