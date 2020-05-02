#include "ai.h"
#include "board.h"
#include <memory>

using namespace minesweeper;

int
main(int argc, char** argv)
{

    // AI Test
    auto b = std::make_shared<Board>(16, 16, 32);
    AICallback cb;
    cb.before_start = [](Board& b) {
        b.show_game_state(std::cout, true) << std::endl;
    };
    cb.on_step = [](Board&, int current_step, int nest_level) {
        std::cout << "[Step " << current_step << " (nest=" << nest_level << ")]"
                  << std::endl;
        return true;
    };
    bool result = MineAI::solve_all(b, true, cb);

    if (result) {
        b->show_game_state(std::cout, true) << std::endl << "Finish!" << std::endl;
    } else {
        b->show_game_state(std::cout, true) << std::endl
                                            << "Failed..." << std::endl;
    }

    return 0;
}
