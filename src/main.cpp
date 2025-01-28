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

    auto& problem_d = problem.value();
    problem_d.print();

    blt::gfx::init(blt::gfx::window_data{"My Sexy Window", init, update, destroy}.setSyncInterval(1));
}