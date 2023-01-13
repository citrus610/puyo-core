#include "fieldbit.h"

bool FieldBit::operator == (const FieldBit& other)
{
    return this->data[0] == other.data[0] && this->data[1] == other.data[1];
};

bool FieldBit::operator != (const FieldBit& other)
{
    return !(*this == other);
};

FieldBit FieldBit::operator | (const FieldBit& other)
{
    FieldBit result;
    result.data[0] = this->data[0] | other.data[0];
    result.data[1] = this->data[1] | other.data[1];
    return result;
};

FieldBit FieldBit::operator & (const FieldBit& other)
{
    FieldBit result;
    result.data[0] = this->data[0] & other.data[0];
    result.data[1] = this->data[1] & other.data[1];
    return result;
};

FieldBit FieldBit::operator ~ ()
{
    FieldBit result;
    result.data[0] = ~this->data[0];
    result.data[1] = ~this->data[1];
    return result;
};

FieldBit FieldBit::shift_up(const FieldBit& fieldbit)
{
    FieldBit result;
    result.data[0] = (fieldbit.data[0] << 1) & 0x1FFF1FFF1FFF1FFF;
    result.data[1] = (fieldbit.data[1] << 1) & 0x000000001FFF1FFF;
    return result;
};

FieldBit FieldBit::shift_down(const FieldBit& fieldbit)
{
    FieldBit result;
    result.data[0] = (fieldbit.data[0] >> 1) & 0x1FFF1FFF1FFF1FFF;
    result.data[1] = (fieldbit.data[1] >> 1) & 0x000000001FFF1FFF;
    return result;
};

FieldBit FieldBit::shift_right(const FieldBit& fieldbit)
{
    FieldBit result;
    result.data[1] = ((fieldbit.data[1] << 16) | (fieldbit.data[0] >> 48)) & 0x000000001FFF1FFF;
    result.data[0] = fieldbit.data[0] << 16;
    return result;
};

FieldBit FieldBit::shift_left(const FieldBit& fieldbit)
{
    FieldBit result;
    result.data[0] = (fieldbit.data[0] >> 16) | (fieldbit.data[1] << 48);
    result.data[1] = fieldbit.data[1] >> 16;
    return result;
};

FieldBit FieldBit::expand(const FieldBit& fieldbit)
{
    return FieldBit::shift_up(fieldbit) | FieldBit::shift_down(fieldbit) | FieldBit::shift_right(fieldbit) | FieldBit::shift_left(fieldbit) | fieldbit;
};

FieldBit FieldBit::mask_12(const FieldBit& fieldbit)
{
    FieldBit result;
    result.data[0] = fieldbit.data[0] & 0x0FFF0FFF0FFF0FFF;
    result.data[1] = fieldbit.data[1] & 0x0FFF0FFF;
    return result;
};

void FieldBit::set_bit(i8 x, i8 y)
{
    if (x < 0 || x > 5 || y < 0 || y > 12) {
        return;
    }
    ((u16*)this->data)[x] |= 1ULL << y;
};

bool FieldBit::get_bit(i8 x, i8 y)
{
    if (x < 0 || x > 5 || y < 0 || y > 12) {
        return true;
    }
    return ((u16*)this->data)[x] & (1ULL << y);
};

u32 FieldBit::get_count()
{
    return std::popcount(this->data[0]) + std::popcount(this->data[1]);
};

u16 FieldBit::get_col(i8 x)
{
    if (x < 0 || x > 5) {
        return ~0;
    }
    return ((u16*)this->data)[x];
};

FieldBit FieldBit::get_mask_pop()
{
    FieldBit org = FieldBit::mask_12(*this);

    FieldBit shift_up = FieldBit::shift_up(org) & org;
    FieldBit shift_down = FieldBit::shift_down(org) & org;
    FieldBit shift_right = FieldBit::shift_right(org) & org;
    FieldBit shift_left = FieldBit::shift_left(org) & org;

    FieldBit u_and_d = shift_up & shift_down;
    FieldBit l_and_r = shift_right & shift_left;
    FieldBit u_or_d = shift_up | shift_down;
    FieldBit l_or_r = shift_right | shift_left;

    FieldBit link_3 = (u_and_d & l_or_r) | (l_and_r & u_or_d);
    FieldBit link_2 = u_and_d | l_and_r | (u_or_d & l_or_r);
    FieldBit link_2_up = FieldBit::shift_up(link_2) & link_2;
    FieldBit link_2_down = FieldBit::shift_down(link_2) & link_2;
    FieldBit link_2_right = FieldBit::shift_right(link_2) & link_2;
    FieldBit link_2_left = FieldBit::shift_left(link_2) & link_2;

    return FieldBit::expand(link_3 | link_2_up | link_2_down | link_2_right | link_2_left) & org;
};

FieldBit FieldBit::get_mask_group(i8 x, i8 y)
{
    FieldBit org_m12 = FieldBit::mask_12(*this);
    if (org_m12.data[0] == 0 && org_m12.data[1] == 0) {
        return FieldBit { 0 };
    }
    FieldBit mask = { 0 };
    mask.set_bit(x, y);
    while (true)
    {
        FieldBit mask_expand = FieldBit::expand(mask) & org_m12;
        if (mask_expand == mask) {
            break;
        }
        mask = mask_expand;
    }
    return mask;
};

FieldBit FieldBit::get_mask_group_lsb()
{
    FieldBit org_m12 = FieldBit::mask_12(*this);
    if (org_m12.data[0] == 0 && org_m12.data[1] == 0) {
        return FieldBit { 0 };
    }
    FieldBit mask = { 0 };
    if (org_m12.data[0] == 0) {
        mask.data[1] = org_m12.data[1] & (~org_m12.data[1] + 1);
    }
    else {
        mask.data[0] = org_m12.data[0] & (~org_m12.data[0] + 1);
    }
    while (true)
    {
        FieldBit mask_expand = FieldBit::expand(mask) & org_m12;
        if (mask_expand == mask) {
            break;
        }
        mask = mask_expand;
    }
    return mask;
};

void FieldBit::pop(FieldBit& mask)
{
    for (i32 i = 0; i < 6; ++i) {
        ((u16*)this->data)[i] = pext16(((u16*)this->data)[i], ~((u16*)mask.data)[i]);
    }
};

void FieldBit::print()
{
    for (i32 i = 0; i < 6; ++i) {
        std::cout << std::bitset<13>(((u16*)this->data)[5 - i]) << std::endl;
    }
    std::cout << std::endl;
};