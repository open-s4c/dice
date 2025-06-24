#include <cstdio>
#include <iostream>

struct A {
    char *p_;
    A()
    {
        p_ = new char[1024];
        sprintf(p_, "%s", "hello world");
    }
    ~A()
    {
        delete[] p_;
    }
};

A a;

int
main()
{
    A b;
    std::cout << "A: " << a.p_ << std::endl;
    std::cout << "B: " << b.p_ << std::endl;
    return 0;
}
