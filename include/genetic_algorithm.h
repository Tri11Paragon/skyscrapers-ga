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

#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

#include <limits>
#include <utility>
#include <skyscrapers.h>
#include <blt/std/random.h>

namespace sky
{
    struct individual_t
    {
        solution_t solution;
        blt::i32 fitness = std::numeric_limits<blt::i32>::max();

        individual_t(solution_t solution, const blt::i32 fitness): solution(std::move(solution)), fitness(fitness)
        {
        }

        void replace(const problem_t& problem, const solution_t& new_solution)
        {
            solution = new_solution;
            fitness = solution.fitness(problem);
        }
    };

    class genetic_algorithm
    {
    public:
        genetic_algorithm(problem_t problem, const blt::i32 individual_count, const double crossover_rate = 0.8, const double mutation_rate = 0.1):
            crossover_rate(crossover_rate), mutation_rate(mutation_rate), m_problem(std::move(problem))
        {
            for (blt::i32 i = 0; i < individual_count; i++)
            {
                solution_t solution{problem.board_size};
                solution.init(problem);
                individuals.emplace_back(solution, solution.fitness(m_problem));
            }
        }

        void run_step(blt::i32 elites = 2, blt::i32 k = 5);

        [[nodiscard]] double average_fitness() const;

        [[nodiscard]] std::vector<individual_t> get_best(blt::i32 amount);

        [[nodiscard]] blt::random::random_t& get_random() const;

        [[nodiscard]] const solution_t& select(blt::i32 k = 5) const;

        [[nodiscard]] std::pair<solution_t, solution_t> crossover(solution_t first, solution_t second) const;

        [[nodiscard]] solution_t mutate(solution_t individual) const;

    private:
        double crossover_rate, mutation_rate;
        problem_t m_problem;
        std::vector<individual_t> individuals;
    };
}

#endif //GENETIC_ALGORITHM_H
