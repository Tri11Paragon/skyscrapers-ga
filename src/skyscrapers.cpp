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
#include <blt/std/logging.h>

namespace sky
{
    void problem_t::print()
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

        auto top_problems = blt::string::split(lines[1], '\t');

        if (top_problems.size() != static_cast<blt::size_t>(problem.board_size))
        {
            BLT_WARN("File is incorrectly formatted. Expected BOARD_SIZE '%d' number of elements got %lu", problem.board_size, top_problems.size());
            return blt::unexpected(problem_t::error_t::INCORRECT_BOARD_DATA_FOR_SIZE);
        }

        for (const auto& arrow : top_problems)
            problem.top.push_back(std::stoi(arrow));

        for (blt::size_t i = 0; i < static_cast<blt::size_t>(problem.board_size); i++)
        {
            const auto index = i + 2;
            if (index >= lines.size())
            {
                BLT_WARN("File is incorrectly formatted. Expected BOARD_SIZE '%d' number of rows describing the sizes of the board but got %lu",
                         problem.board_size, lines.size());
                return blt::unexpected(problem_t::error_t::INCORRECT_BOARD_DATA_FOR_SIZE);
            }
            auto data = blt::string::split(lines[index], '\t');
            if (data.size() != 2)
            {
                BLT_WARN("File is incorrectly formatted. Expected 2 points for the side data descriptors, got %lu", data.size());
                return blt::unexpected(problem_t::error_t::INCORRECT_BOARD_DATA_FOR_SIZE);
            }
            problem.left.push_back(std::stoi(data[0]));
            problem.right.push_back(std::stoi(data[1]));
        }

        auto bottom_problems = blt::string::split(lines[8], '\t');

        if (bottom_problems.size() != static_cast<blt::size_t>(problem.board_size))
        {
            BLT_WARN("File is incorrectly formatted. Expected BOARD_SIZE '%d' number of elements got %lu", problem.board_size, top_problems.size());
            return blt::unexpected(problem_t::error_t::INCORRECT_BOARD_DATA_FOR_SIZE);
        }

        for (const auto& arrow : bottom_problems)
            problem.bottom.push_back(std::stoi(arrow));

        return problem;
    }
}
