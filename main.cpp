#include "ai.h"
#include "board.h"
#include "boardview.h"
#include "guimain.h"
#include <QApplication>
#include <QSharedPointer>

using namespace minesweeper;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    GuiMain* win = new GuiMain();
    win->show();

    //win->autoSolve(0.1);

    return app.exec();

    // AI Test
    //    auto b = QSharedPointer<Board>(new Board(16, 16, 32));
    //    AICallback cb;
    //    cb.before_start = [](Board& b) {
    //        b.show_game_state(std::cout, true) << std::endl;
    //    };
    //    cb.on_step = [](Board&, int current_step, int nest_level) {
    //        std::cout << "[Step " << current_step << " (nest=" << nest_level << ")]"
    //                  << std::endl;
    //        return true;
    //    };
    //    bool result = MineAI::solve_all(b, true, cb);

    //    if (result) {
    //        b->show_game_state(std::cout, true) << std::endl << "Finish!" << std::endl;
    //    } else {
    //        b->show_game_state(std::cout, true) << std::endl
    //                                            << "Failed..." << std::endl;
    //    }

    // return 0;
}
