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

        void print() const;

        [[nodiscard]] blt::i32 min() const // NOLINT
        {
            return 1;
        }

        [[nodiscard]] blt::i32 max() const
        {
            return board_size;
        }
    };

    blt::expected<problem_t, problem_t::error_t> problem_from_file(std::string_view path);

    struct solution_t
    {
        blt::i32 board_size;
        std::vector<blt::i32> board_data;

        explicit solution_t(const blt::i32 board_size): board_size(board_size)
        {
            board_data.resize(board_size * board_size);
        }

        void init(const problem_t& problem);

        // checks to see if the row contains duplicates. zero means all good
        [[nodiscard]] blt::i32 row_incorrect_count(blt::i32 row) const;
        // checks to see if the arrows are correct for this row
        [[nodiscard]] blt::i32 row_view_count(const problem_t& problem, blt::i32 row) const;
        [[nodiscard]] blt::i32 column_incorrect_count(blt::i32 column) const;
        [[nodiscard]] blt::i32 column_view_count(const problem_t& problem, blt::i32 column) const;

        [[nodiscard]] blt::i32 fitness(const problem_t& problem) const;

        [[nodiscard]] blt::i32 get(const blt::i32 row, const blt::i32 column) const
        {
            return board_data[row * board_size + column];
        }

        void set(const blt::i32 row, const blt::i32 column, const blt::i32 value)
        {
            board_data[row * board_size + column] = value;
        }

        void print() const;

        void print(const problem_t& problem) const;
    };

    problem_t make_test_problem();

    solution_t make_test_solution();
    solution_t make_test_solution_bad1();
    solution_t make_test_solution_bad2();
    solution_t make_test_solution_bad3();
}

#endif //SKYSCRAPERS_H
