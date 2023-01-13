#include <iostream>
#include "../core/field.h"

int main() {
    Field f;
    const char c[13][7] = {
        "B.YRGY",
        "BBBYRB",
        "GBYRGG",
        "BGYRGB",
        "GRGYRB",
        "RGYRYB",
        "GRGYRY",
        "GRGYRY",
        "GBBGYG",
        "BYRBGG",
        "GBYRBY",
        "GBYRBY",
        "GBYRBY",
    };
    f.from(c);

    auto a = FieldBit::mask_12(f.data[3]);
    a.print();
    f.data[3].get_mask_pop().print();

    Chain::Data ch = f.pop();
    f.print();
    std::cout << int(ch.count) << " " << int(ch.score) << std::endl;

    std::cout << int(bench_pop()) << " ns" << std::endl;

    return 0;
}
