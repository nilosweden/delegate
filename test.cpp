#define CATCH_CONFIG_MAIN
#include "delegate.hpp"
#include <catch/catch.hpp>

//Saves results between function calls
std::vector<int> result;

//Basic class
class base
{
public:
    base()
    {}

    void operator()(int x)
    {
        result.push_back(x);
    }

    virtual void print(int x)
    {
        result.push_back(x);
    }

    virtual ~base()
    {}
};

//Derived class
class derived : public base
{
public:
    derived()
    {}

    void print(int x) override
    {
        result.push_back(x*2);
    }

    void operator()(int x)
    {
        result.push_back(x*2);
    }
};

//Basic function
void basic_function(int x)
{
    result.push_back(x*3);
}

TEST_CASE( "Delegate can bind, invoke and clear" )
{
    fsx::delegate<void(int)> delegate;

    SECTION( "Bind 1 function and check count" )
    {
        delegate.bind<&basic_function>();
        REQUIRE(delegate.size() == 1);
    }

    SECTION( "Bind basic function and invoke it" )
    {
        delegate.bind<&basic_function>();
        delegate.invoke(10);
        REQUIRE(result[0] == 30);
        result.clear();
    }

    SECTION( "Bind basic function and clear the delegate" )
    {
        delegate.bind<&basic_function>();
        delegate.clear();
        REQUIRE(delegate.size() == 0);
    }

    SECTION ( "Invoke callable Base class operator()" )
    {
        base b;
        delegate.bind(b).invoke(100);
        REQUIRE(result[0] == 100);
        result.clear();
    }

    SECTION ( "Invoke Derived class Print function through polymorphism" )
    {
        derived d;
        delegate.bind<base, &base::print>(d).invoke(100);
        REQUIRE(result[0] == 200);
        result.clear();
    }

    SECTION ( "Invoke non-capturing lambda" )
    {
        auto lambda = [](int x){
            result.push_back(x*5);
        };
        delegate.bind(lambda).invoke(10);
        REQUIRE(result[0] == 50);
        result.clear();
    }

    SECTION ( "Invoke capturing lambda and call Base print from inside" )
    {
        base b;
        auto lambda = [&](int x){
            result.push_back(x*10);
            b.print(x);
        };
        delegate.bind(lambda).invoke(10);
        REQUIRE(result[0] == 100);
        REQUIRE(result[1] == 10);
        result.clear();
    }

    SECTION ( "Invoke capturing lambda and bind function from inside" )
    {
        auto lambda = [&delegate](int x){
            result.push_back(x*10);
            delegate.clear();
            delegate.bind<&basic_function>();
            delegate.invoke(20);
        };
        delegate.bind(lambda).invoke(10);
        REQUIRE(result[0] == 100);
        REQUIRE(result[1] == 60);
        result.clear();
    }

    SECTION ( "Bind lambda and do multiple invokes" )
    {
        auto lambda = [](int x){
            result.push_back(x*5);
        };
        delegate.bind(lambda).bind<&basic_function>().invoke(10).invoke(20).invoke(30);
        REQUIRE(result[0] == 50);
        REQUIRE(result[1] == 30);
        REQUIRE(result[2] == 100);
        REQUIRE(result[3] == 60);
        REQUIRE(result[4] == 150);
        REQUIRE(result[5] == 90);
        result.clear();
    }
}
