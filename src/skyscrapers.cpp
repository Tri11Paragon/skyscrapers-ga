/*
 *  <Short Description>
 *  Copyright (C) 2025  Brett Terpstra
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
#include <skyscrapers.h>

#include <blt/fs/loader.h>
#include <blt/std/hashmap.h>
#include <blt/std/logging.h>
#include <blt/std/random.h>

namespace sky
{
    void problem_t::print() const
    {
        BLT_TRACE("Board Size: %d", board_size);
        BLT_TRACE_STREAM << "\t";
        for (int i = 0; i < board_size; i++)
            BLT_TRACE_STREAM << top[i] << '\t';
        BLT_TRACE_STREAM << "\n";
        for (int i = 0; i < board_size; i++)
        {
            BLT_TRACE_STREAM << left[i];
            for (int j = 0; j < board_size + 1; j++)
                BLT_TRACE_STREAM << '\t';
            BLT_TRACE_STREAM << right[i] << "\n";
        }
        BLT_TRACE_STREAM << "\t";
        for (int i = 0; i < board_size; i++)
            BLT_TRACE_STREAM << bottom[i] << '\t';
        BLT_TRACE_STREAM << "\n";
    }

    blt::expected<problem_t, problem_t::error_t> problem_from_file(const std::string_view path)
    {
        const auto lines = blt::fs::getLinesFromFile(path);

        const auto size_line = blt::string::split(lines.front(), '\t');

        if (size_line.size() != 2)
        {
            BLT_WARN("File is incorrectly formatted. First line is expected to describe board size like 'BOARD_SIZE:	#`");
            return blt::unexpected(problem_t::error_t::MISSING_BOARD_SIZE);
        }

        problem_t problem{std::stoi(size_line[1])};

        if (lines.size() != static_cast<blt::size_t>(problem.board_size) + 3)
        {
            BLT_TRACE(lines.size());
            BLT_TRACE(problem.board_size + 1);
            BLT_WARN("File is incorrectly formatted. Expected problem to be defined as a series of lines, describing the structure of the board");
            return blt::unexpected(problem_t::error_t::MISSING_BOARD_DATA);
        }

        blt::size_t index = 1;

        auto top_problems = blt::string::split(lines[index++], '\t');

        if (top_problems.size() != static_cast<blt::size_t>(problem.board_size))
        {
            BLT_WARN("File is incorrectly formatted. Expected BOARD_SIZE '%d' number of elements got %lu", problem.board_size, top_problems.size());
            return blt::unexpected(problem_t::error_t::INCORRECT_BOARD_DATA_FOR_SIZE);
        }

        for (const auto& arrow : top_problems)
            problem.top.push_back(std::stoi(arrow));

        for (blt::size_t i = 0; i < static_cast<blt::size_t>(problem.board_size); i++)
        {
            if (index >= lines.size())
            {
                BLT_WARN("File is incorrectly formatted. Expected BOARD_SIZE '%d' number of rows describing the sizes of the board but got %lu",
                         problem.board_size, lines.size());
                return blt::unexpected(problem_t::error_t::INCORRECT_BOARD_DATA_FOR_SIZE);
            }
            auto data = blt::string::split(lines[index++], '\t');
            if (data.size() != 2)
            {
                BLT_WARN("File is incorrectly formatted. Expected 2 points for the side data descriptors, got %lu", data.size());
                return blt::unexpected(problem_t::error_t::INCORRECT_BOARD_DATA_FOR_SIZE);
            }
            problem.left.push_back(std::stoi(data[0]));
            problem.right.push_back(std::stoi(data[1]));
        }

        auto bottom_problems = blt::string::split(lines[index], '\t');

        if (bottom_problems.size() != static_cast<blt::size_t>(problem.board_size))
        {
            BLT_WARN("File is incorrectly formatted. Expected BOARD_SIZE '%d' number of elements got %lu", problem.board_size, top_problems.size());
            return blt::unexpected(problem_t::error_t::INCORRECT_BOARD_DATA_FOR_SIZE);
        }

        for (const auto& arrow : bottom_problems)
            problem.bottom.push_back(std::stoi(arrow));

        return problem;
    }

    void solution_t::init(const problem_t& problem)
    {
        blt::random::random_t random{std::random_device{}()};
        for (auto& v : board_data)
            v = random.get_i32(problem.min(), problem.max() + 1);
    }

    blt::i32 solution_t::row_incorrect_count(const blt::i32 row) const
    {
        thread_local std::vector<blt::i32> value_counts;
        value_counts.resize(board_size);
        std::memset(value_counts.data(), 0, sizeof(blt::i32) * board_size);

        for (blt::i32 column = 0; column < board_size; column++)
            ++value_counts[get(row, column) - 1];

        blt::i32 sum = 0;
        for (const auto v : value_counts)
            sum += std::abs(v - 1);

        return sum;
    }

    blt::i32 solution_t::row_view_count(const problem_t& problem, const blt::i32 row) const
    {
        blt::i32 sees_left = 0;
        blt::i32 sees_right = 0;

        blt::i32 highest_left = 0;
        blt::i32 highest_right = 0;

        for (blt::i32 column = 0; column < board_size; column++)
        {
            if (get(row, column) > highest_left)
            {
                ++sees_left;
                highest_left = get(row, column);
            }
        }

        for (blt::i32 column = board_size - 1; column >= 0; column--)
        {
            if (get(row, column) > highest_right)
            {
                ++sees_right;
                highest_right = get(row, column);
            }
        }

        const auto left = problem.left[row];
        const auto right = problem.right[row];

        return std::abs(left - sees_left) + std::abs(right - sees_right);
    }

    blt::i32 solution_t::column_view_count(const problem_t& problem, blt::i32 column) const
    {
        blt::i32 sees_top = 0;
        blt::i32 sees_bottom = 0;

        blt::i32 highest_top = 0;
        blt::i32 highest_bottom = 0;

        for (blt::i32 row = 0; row < board_size; row++)
        {
            if (get(row, column) > highest_top)
            {
                ++sees_top;
                highest_top = get(row, column);
            }
        }

        for (blt::i32 row = board_size - 1; row >= 0; row--)
        {
            if (get(row, column) > highest_bottom)
            {
                ++sees_bottom;
                highest_bottom = get(row, column);
            }
        }

        const auto top = problem.top[column];
        const auto bottom = problem.bottom[column];

        return std::abs(top - sees_top) + std::abs(bottom - sees_bottom);
    }

    blt::i32 solution_t::column_incorrect_count(const blt::i32 column) const
    {
        thread_local std::vector<blt::i32> value_counts;
        value_counts.resize(board_size);
        std::memset(value_counts.data(), 0, sizeof(blt::i32) * board_size);

        for (blt::i32 row = 0; row < board_size; row++)
            ++value_counts[get(row, column) - 1];

        blt::i32 sum = 0;
        for (const auto v : value_counts)
            sum += std::abs(v - 1);

        return sum;
    }

    blt::i32 solution_t::fitness(const problem_t& problem) const
    {
        blt::i32 fitness = 0;
        for (blt::i32 i = 0; i < board_size; i++)
        {
            fitness += row_incorrect_count(i);
            fitness += row_view_count(problem, i);
            fitness += column_incorrect_count(i);
            fitness += column_view_count(problem, i);
        }
        return fitness;
    }

    void solution_t::print() const
    {
        for (blt::i32 i = 0; i < board_size; i++)
        {
            for (blt::i32 j = 0; j < board_size; j++)
            {
                BLT_TRACE_STREAM << get(i, j);
                if (j < board_size - 1)
                    BLT_TRACE_STREAM << '\t';
            }
            BLT_TRACE_STREAM << '\n';
        }
    }

    void solution_t::print(const problem_t& problem) const
    {
        BLT_TRACE("Board Size: %d", board_size);
        BLT_TRACE_STREAM << "\t";
        for (int i = 0; i < board_size; i++)
            BLT_TRACE_STREAM << problem.top[i] << '\t';
        BLT_TRACE_STREAM << "\n";
        for (int i = 0; i < board_size; i++)
        {
            BLT_TRACE_STREAM << problem.left[i];
            BLT_TRACE_STREAM << '\t';
            for (int j = 0; j < board_size; j++)
            {
                BLT_TRACE_STREAM << get(i, j);
                BLT_TRACE_STREAM << '\t';
            }
            BLT_TRACE_STREAM << problem.right[i] << "\n";
        }
        BLT_TRACE_STREAM << "\t";
        for (int i = 0; i < board_size; i++)
            BLT_TRACE_STREAM << problem.bottom[i] << '\t';
        BLT_TRACE_STREAM << "\n";
    }

    problem_t make_test_problem()
    {
        problem_t problem{3};
        problem.top = {2, 1, 2};
        problem.bottom = {1, 3, 2};
        problem.left = {2, 3, 1};
        problem.right = {2, 1, 2};

        return problem;
    }

    solution_t make_test_solution()
    {
        solution_t solution{3};

        solution.board_data = {
            2, 3, 1,
            1, 2, 3,
            3, 1, 2
        };

        return solution;
    }

    solution_t make_test_solution_bad1()
    {
        solution_t solution{3};

        solution.board_data = {
            2, 3, 1,
            1, 2, 3,
            1, 1, 2
        };

        return solution;
    }

    solution_t make_test_solution_bad2()
    {
        solution_t solution{3};

        solution.board_data = {
            1, 3, 2,
            1, 2, 3,
            3, 1, 2
        };

        return solution;
    }

    solution_t make_test_solution_bad3()
    {
        solution_t solution{3};

        solution.board_data = {
            3, 2, 1,
            1, 3, 2,
            3, 2, 1
        };

        return solution;
    }
}
