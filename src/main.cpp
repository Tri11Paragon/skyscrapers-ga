#include <genetic_algorithm.h>
#include <blt/gfx/window.h>
#include "blt/gfx/renderer/resource_manager.h"
#include "blt/gfx/renderer/batch_2d_renderer.h"
#include "blt/gfx/renderer/camera.h"
#include <blt/parse/argparse.h>
#include <skyscrapers.h>
#include <imgui.h>

blt::gfx::matrix_state_manager global_matrices;
blt::gfx::resource_manager resources;
blt::gfx::batch_renderer_2d renderer_2d(resources, global_matrices);
blt::gfx::first_person_camera camera;

void init(const blt::gfx::window_data&)
{
    using namespace blt::gfx;


    global_matrices.create_internals();
    resources.load_resources();
    renderer_2d.create();
}

void update(const blt::gfx::window_data& data)
{
    global_matrices.update_perspectives(data.width, data.height, 90, 0.1, 2000);

    camera.update();
    camera.update_view(global_matrices);
    global_matrices.update();

    renderer_2d.render(data.width, data.height);
}

void destroy(const blt::gfx::window_data&)
{
    global_matrices.cleanup();
    resources.cleanup();
    renderer_2d.cleanup();
    blt::gfx::cleanup();
}

int main(int argc, const char** argv)
{
    blt::arg_parse parser;

    parser.addArgument(blt::arg_builder("file").build());

    auto args = parser.parse_args(argc, argv);

    if (!args.contains("file"))
    {
        BLT_WARN("Please provide a skyscraper formatted file!");
        return EXIT_FAILURE;
    }

    const auto file = args.get<std::string>("file");

    auto problem = sky::problem_from_file(file);

    if (!problem)
    {
        BLT_WARN("Unable to parse skyscraper file!");
        return EXIT_FAILURE;
    }

    const auto& problem_d = problem.value();
    problem_d.print();

    sky::genetic_algorithm ga{problem_d, 500, 0.8, 0.1};

    for (blt::i32 i = 0; i < 50; i++)
    {
        ga.run_step(2, 5);
        BLT_TRACE("Ran GP generation %d with average fitness %lf", i, ga.average_fitness());
        auto best = ga.get_best(1);
        BLT_TRACE("Best individual has fitness: %d", best.front().fitness);
    }

    auto best = ga.get_best(1);
    BLT_TRACE("Best individual: %d", best.front().solution.fitness(problem_d));
    best.front().solution.print(problem_d);

    BLT_TRACE("----------");

    const auto test = sky::make_test_problem();
    auto sol = sky::make_test_solution();

    test.print();
    BLT_TRACE(sol.fitness(test));

    BLT_TRACE(sky::make_test_solution_bad1().fitness(test));
    BLT_TRACE(sky::make_test_solution_bad2().fitness(test));
    BLT_TRACE(sky::make_test_solution_bad3().fitness(test));


    // blt::gfx::init(blt::gfx::window_data{"My Sexy Window", init, update, destroy}.setSyncInterval(1));
}