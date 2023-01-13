#pragma once

#include "def.h"
#include "cell.h"

namespace Chain
{
    struct Data
    {
        i32 count = 0;
        i32 score = 0;
    };

    constexpr u32 COLOR_BONUS[] = { 0, 0, 3, 6, 12, 24 };
    constexpr u32 GROUP_BONUS[] = { 0, 0, 0, 0, 0, 2, 3, 4, 5, 6, 7, 10 };
    constexpr u32 POWER[] = { 0, 8, 16, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512 };
};