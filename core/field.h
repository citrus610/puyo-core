#pragma once

#include "fieldbit.h"

class Field
{
public:
    FieldBit data[Cell::COUNT] = { 0 };
public:
    bool operator == (const Field& other);
    bool operator != (const Field& other);
public:
    void set_cell(i8 x, i8 y, Cell::Type cell);
public:
    u32 get_count();
    u32 get_count(u8 height[6]);
    Cell::Type get_cell(i8 x, i8 y);
    u8 get_height(i8 x);
    u8 get_height_max();
    void get_height(u8 height[6]);
    FieldBit get_mask();
    Field get_mask_pop();
    u8 get_drop_pair_frame(i8 x, Direction::Type direction);
public:
    bool is_occupied(i8 x, i8 y);
    bool is_occupied(i8 x, i8 y, u8 height[6]);
    bool is_colliding_pair(i8 x, i8 y, Direction::Type direction);
    bool is_colliding_pair(i8 x, i8 y, Direction::Type direction, u8 height[6]);
public:
    void drop_puyo(i8 x, Cell::Type cell);
    void drop_pair(i8 x, Direction::Type direction, Cell::Pair pair);
public:
    Chain::Data pop();
public:
    void from(const char c[13][7]);
    void print();
};

static i64 bench_pop()
{
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
    f.print();

    i64 time = 0;
    Chain::Data chain = { 0 };

    for (i32 i = 0; i < 10000; ++i) {
        auto f_copy = f;
        auto time_start = std::chrono::high_resolution_clock::now();
        chain = f_copy.pop();
        auto time_end = std::chrono::high_resolution_clock::now();
        time += std::chrono::duration_cast<std::chrono::nanoseconds>(time_end - time_start).count();
    }

    std::cout << "count: " << int(chain.count) << std::endl;
    std::cout << "score: " << int(chain.score) << std::endl;

    return time / 10000;
};