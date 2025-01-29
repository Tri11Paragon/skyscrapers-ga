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
#include <genetic_algorithm.h>
#include <blt/std/hashmap.h>
#include <blt/std/logging.h>
#include <blt/std/random.h>

namespace sky
{
    void genetic_algorithm::run_step(const blt::i32 elites, const blt::i32 k)
    {
        std::vector<individual_t> next_generation;

        const double total_chance = crossover_rate + mutation_rate;
        const double adjusted_crossover = crossover_rate / total_chance;

        if (elites > 0)
        {
            std::sort(individuals.begin(), individuals.end(), [](const auto& a, const auto& b)
            {
                return a.fitness < b.fitness;
            });
            for (blt::i32 i = 0; i < elites; ++i)
                next_generation.push_back(individuals[i]);
        }

        for (blt::size_t i = 0; i < individuals.size(); i++)
        {
            if (get_random().choice(adjusted_crossover))
            {
                const solution_t& p1 = select(k);
                const solution_t* p2;
                do
                {
                    p2 = &select(k);
                }
                while (p2 == &p1);

                auto [c1, c2] = crossover(p1, *p2);
                next_generation.emplace_back(c1, c1.fitness(m_problem));
                if (next_generation.size() + 1 < individuals.size())
                {
                    next_generation.emplace_back(c2, c2.fitness(m_problem));
                    ++i;
                }
            }
            else
            {
                const solution_t& p1 = select(k);
                auto c1 = mutate(p1);
                next_generation.emplace_back(c1, c1.fitness(m_problem));
            }
        }

        individuals.clear();
        individuals = std::move(next_generation);
    }

    double genetic_algorithm::average_fitness() const
    {
        double total_fitness = 0.0;

        for (const auto& i : individuals)
            total_fitness += i.fitness;

        return total_fitness / static_cast<double>(individuals.size());
    }

    std::vector<individual_t> genetic_algorithm::get_best(const blt::i32 amount)
    {
        std::sort(individuals.begin(), individuals.end(), [](const auto& a, const auto& b)
        {
            return a.fitness < b.fitness;
        });

        std::vector<individual_t> best;
        for (blt::size_t i = 0; i < static_cast<blt::size_t>(amount); i++)
            best.push_back(individuals[i]);

        return best;
    }

    blt::random::random_t& genetic_algorithm::get_random() const // NOLINT
    {
        thread_local blt::random::random_t random{std::random_device{}()};
        return random;
    }

    const solution_t& genetic_algorithm::select(const blt::i32 k) const
    {
        thread_local blt::hashset_t<blt::size_t> selected_indexes;
        selected_indexes.clear();

        blt::size_t index = 0;
        blt::i32 best_fitness = std::numeric_limits<blt::i32>::max();

        for (blt::i32 i = 0; i < k; ++i)
        {
            blt::size_t point;
            do
            {
                point = get_random().get_u64(0, individuals.size());
            }
            while (selected_indexes.contains(point));
            selected_indexes.insert(point);
            if (individuals[point].fitness < best_fitness)
            {
                index = point;
                best_fitness = individuals[point].fitness;
            }
        }
        return individuals[index].solution;
    }

    std::pair<solution_t, solution_t> genetic_algorithm::crossover(solution_t first, solution_t second) const
    {
        auto& random = get_random();

        const auto first_begin = random.get_size_t(0, first.board_size * first.board_size - 1);
        const auto first_end = random.get_size_t(first_begin + 1, first.board_size * first.board_size);

        const auto size = first_end - first_begin;

        const auto second_begin = random.get_size_t(0, second.board_size * second.board_size - size);

        std::vector<blt::i32> temp;
        temp.resize(size);

        std::memcpy(temp.data(), second.board_data.data() + second_begin, sizeof(blt::i32) * size);
        std::memcpy(second.board_data.data() + second_begin, first.board_data.data() + first_begin, sizeof(blt::i32) * size);
        std::memcpy(first.board_data.data() + first_begin, temp.data(), sizeof(blt::i32) * size);

        return {first, second};
    }

    solution_t genetic_algorithm::mutate(solution_t individual) const
    {
        auto& random = get_random();

        // TODO: better mutation
        const blt::i32 points = random.get_i32(0, 5);
        for (blt::i32 i = 0; i < points; ++i)
        {
            const auto index = random.get_size_t(0, individual.board_size * individual.board_size);
            const auto replacement = random.get_i32(m_problem.min(), m_problem.max() + 1);
            individual.board_data[index] = replacement;
        }

        return individual;
    }
}
