#include "field.h"

bool Field::operator == (const Field& other)
{
    for (u8 i = 0; i < Cell::COUNT; ++i) {
        if (this->data[i] != other.data[i]) {
            return false;
        }
    }
    return true;
};

bool Field::operator != (const Field& other)
{
    return !(*this == other);
};

void Field::set_cell(i8 x, i8 y, Cell::Type cell)
{
    assert(x >= 0 && x < 6);
    assert(y >= 0 && y < 13);
    this->data[static_cast<u8>(cell)].set_bit(x, y);
};

u32 Field::get_count()
{
    u32 result = 0;
    for (u8 i = 0; i < Cell::COUNT; ++i) {
        result += this->data[i].get_count();
    }
    return result;
};

u32 Field::get_count(u8 height[6])
{
    return height[0] + height[1] + height[2] + height[3] + height[4] + height[5];
};

Cell::Type Field::get_cell(i8 x, i8 y)
{
    if (x < 0 || x > 5 || y < 0 || y > 12) {
        return Cell::Type::NONE;
    }
    for (u8 i = 0; i < Cell::COUNT; ++i) {
        if (this->data[i].get_bit(x, y)) {
            return Cell::Type(i);
        }
    }
    return Cell::Type::NONE;
};

u8 Field::get_height(i8 x)
{
    assert(x >= 0 && x < 6);
    uint16_t column_or = this->data[0].get_col(x);
    for (u8 i = 1; i < Cell::COUNT; ++i) {
        column_or |= this->data[i].get_col(x);
    }
    return 16 - std::countl_zero(column_or);
};

u8 Field::get_height_max()
{
    u8 result = 0;
    for (i32 i = 0; i < 6; ++i) {
        result = std::max(result, this->get_height(i));
    }
    return result;
};

void Field::get_height(u8 height[6])
{
    for (i32 i = 0; i < 6; ++i) {
        height[i] = this->get_height(i);
    }
};

FieldBit Field::get_mask()
{
    FieldBit result = this->data[0];
    for (u8 i = 1; i < Cell::COUNT; ++i) {
        result = result | this->data[i];
    }
    return result;
};

Field Field::get_mask_pop()
{
    Field result = { 0 };
    for (u8 i = 0; i < Cell::COUNT - 1; ++i) {
        result.data[i] = this->data[i].get_mask_pop();
    }
    return result;
};

u8 Field::get_drop_pair_frame(i8 x, Direction::Type direction)
{
    return 1 + (this->get_height(x) != this->get_height(x + Direction::get_offset_x(direction)));
};

bool Field::is_occupied(i8 x, i8 y)
{
    if (x < 0 || x > 5 || y < 0) {
        return true;
    }
    return y < this->get_height(x);
};

bool Field::is_occupied(i8 x, i8 y, u8 height[6])
{
    if (x < 0 || x > 5 || y < 0) {
        return true;
    }
    return y < height[x];
};

bool Field::is_colliding_pair(i8 x, i8 y, Direction::Type direction)
{
    u8 height[6];
    this->get_height(height);
    return this->is_colliding_pair(x, y, direction, height);
};

bool Field::is_colliding_pair(i8 x, i8 y, Direction::Type direction, u8 height[6])
{
    return this->is_occupied(x, y, height) || this->is_occupied(x + Direction::get_offset_x(direction), y + Direction::get_offset_y(direction), height);
};

void Field::drop_puyo(i8 x, Cell::Type cell)
{
    assert(x >= 0 && x < 6);
    u8 height = this->get_height(x);
    if (height < 13) {
        this->set_cell(x, height, cell);
    }
};

void Field::drop_pair(i8 x, Direction::Type direction, Cell::Pair pair)
{
    assert(x >= 0 && x < 6);
    switch (direction)
    {
    case Direction::Type::UP:
        this->drop_puyo(x, pair.first);
        this->drop_puyo(x, pair.second);
        break;
    case Direction::Type::RIGHT:
        this->drop_puyo(x, pair.first);
        this->drop_puyo(x + 1, pair.second);
        break;
    case Direction::Type::DOWN:
        this->drop_puyo(x, pair.second);
        this->drop_puyo(x, pair.first);
        break;
    case Direction::Type::LEFT:
        this->drop_puyo(x, pair.first);
        this->drop_puyo(x - 1, pair.second);
        break;
    }
};

Chain::Data Field::pop()
{
    Chain::Data chain = Chain::Data {
        .count = 0,
        .score = 0
    };

    for (i32 index = 0; index < 20; ++index) {
        // Get pop mask
        Field pop = this->get_mask_pop();
        FieldBit mask_pop = pop.get_mask();
        u32 pop_count = mask_pop.get_count();
        if (pop_count == 0) {
            chain.count = index;
            break;
        }

        // Clear puyo
        mask_pop = mask_pop | (FieldBit::expand(mask_pop) & this->data[static_cast<u8>(Cell::Type::GARBAGE)]);
        for (u8 cell = 0; cell < Cell::COUNT; ++cell) {
            this->data[cell].pop(mask_pop);
        }

        // Calculate chain's link's score
        u32 chain_power = Chain::POWER[index];
        u32 color = 0;
        for (u8 cell = 0; cell < Cell::COUNT - 1; ++cell) {
            color += (pop.data[cell].data[0] | pop.data[cell].data[1]) > 0;
        }
        u32 bonus_color = Chain::COLOR_BONUS[color];
        u32 group_bonus = 0;
        for (u8 cell = 0; cell < Cell::COUNT - 1; ++cell) {
            while (true)
            {
                FieldBit group = pop.data[cell].get_mask_group_lsb();
                if (group.data[0] == 0 && group.data[1] == 0) {
                    break;
                }
                pop.data[cell] = pop.data[cell] & (~group);
                group_bonus += Chain::GROUP_BONUS[std::min(11U, group.get_count())];
            }
        }
        chain.score += pop_count * 10 * std::clamp(chain_power + bonus_color + group_bonus, 1U, 999U);
    }

    return chain;
};

void Field::from(const char c[13][7])
{
    *this = Field { 0 };
    for (i8 y = 0; y < 13; ++y) {
        for (i8 x = 0; x < 6; ++x) {
            if (c[12 - y][x] == '.' || c[12 - y][x] == ' ') {
                continue;
            }
            this->set_cell(x, y, Cell::from_char(c[12 - y][x]));
        }
    }
};

void Field::print()
{
    using namespace std;
    for (i8 y = 12; y >= 0; --y) {
        for (i8 x = 0; x < 6; ++x) {
            cout << Cell::to_char(this->get_cell(x, y));
        }
        cout << "\n";
    }
    cout << endl;
};