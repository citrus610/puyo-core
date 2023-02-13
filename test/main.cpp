#include <iostream>
#include "../core/core.h"

int main()
{
    auto t = bench_pop(10000);
    printf("time: %d ns\n", t);

    return 0;
};
