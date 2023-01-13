#pragma once

#include "def.h"
#include "cell.h"
#include "direction.h"
#include "chain.h"

class FieldBit
{
public:
    u64 data[2] = { 0 };
public:
    bool operator == (const FieldBit& other);
    bool operator != (const FieldBit& other);
    FieldBit operator | (const FieldBit& other);
    FieldBit operator & (const FieldBit& other);
    FieldBit operator ~ ();
public:
    static FieldBit shift_up(const FieldBit& fieldbit);
    static FieldBit shift_down(const FieldBit& fieldbit);
    static FieldBit shift_right(const FieldBit& fieldbit);
    static FieldBit shift_left(const FieldBit& fieldbit);
    static FieldBit expand(const FieldBit& fieldbit);
    static FieldBit mask_12(const FieldBit& fieldbit);
public:
    void set_bit(i8 x, i8 y);
    bool get_bit(i8 x, i8 y);
    u32 get_count();
    u16 get_col(i8 x);
    FieldBit get_mask_pop();
    FieldBit get_mask_group(i8 x, i8 y);
    FieldBit get_mask_group_lsb();
public:
    void pop(FieldBit& mask);
    void print();
};