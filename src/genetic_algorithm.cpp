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
#include <blt/std/random.h>

namespace sky {
    void genetic_algorithm::run_step()
    {

    }

    solution_t genetic_algorithm::select(const blt::i32 k) const
    {
        thread_local blt::hashset_t<blt::size_t> selected_indexes;
        thread_local blt::random::random_t random{std::random_device{}()};
        selected_indexes.clear();

        blt::size_t index = 0;
        blt::i32 best_fitness = std::numeric_limits<blt::i32>::max();

        for (blt::i32 i = 0; i < k; ++i)
        {
            blt::size_t point;
            do
            {
                point = random.get_u64(0, individuals.size());
            } while (selected_indexes.contains(point));
            selected_indexes.insert(point);
            if (individuals[point].fitness < best_fitness)
            {
                index = point;
                best_fitness = individuals[point].fitness;
            }
        }
        return individuals[index].solution;
    }

    std::pair<solution_t, solution_t> genetic_algorithm::crossover(const individual_t& first, const individual_t& second)
    {
    }

    solution_t genetic_algorithm::mutate(const individual_t& individual)
    {
    }
}
