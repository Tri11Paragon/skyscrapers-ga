#pragma once
/*
 *  Copyright (C) 2024  Brett Terpstra
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SKYSCRAPERS_H
#define SKYSCRAPERS_H

#include <vector>
#include <blt/std/types.h>
#include <blt/std/expected.h>
#include <string>
#include <string_view>

namespace sky
{
    struct problem_t
    {
        enum class error_t
        {
            MISSING_BOARD_SIZE,
            MISSING_BOARD_DATA,
            INCORRECT_BOARD_DATA_FOR_SIZE
        };

        blt::i32 board_size;
        std::vector<blt::i32> top, bottom, left, right;

        explicit problem_t(const blt::i32 board_size): board_size(board_size)
        {
            top.reserve(board_size);
            bottom.reserve(board_size);
            left.reserve(board_size);
            right.reserve(board_size);
        }

        void print();

        [[nodiscard]] blt::i32 min() const // NOLINT
        {
            return 1;
        }

        [[nodiscard]] blt::i32 max() const
        {
            return board_size + 1;
        }
    };

    blt::expected<problem_t, problem_t::error_t> problem_from_file(std::string_view path);
}

#endif //SKYSCRAPERS_H
