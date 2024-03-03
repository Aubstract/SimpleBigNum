#include <iostream>
#include "UIntX.h"

int main()
{
    const uint32_t SIZE = 128;

    UIntX<512> a = 13;
    UIntX<SIZE> b;
    UIntX<SIZE + 5> c;

    UIntX<1024> d = a;
    d = 12;
    return 0;
}
