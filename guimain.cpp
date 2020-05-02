#include "guimain.h"
#include "board.h"
#include "boardview.h"
#include <QMessageBox>
#include <QString>

namespace minesweeper {

GuiMain::GuiMain()
{
    QSharedPointer<Board> b(new Board(16, 16, 32));
    auto* view = new BoardView();
    view->setBoard(b);
    view->setCallback(std::make_unique<MainCallback>());

    setGeometry(0, 10, 700, 700);
    setWindowTitle("Cane MineSweeper");
    setCentralWidget(view);
}

} // namespace minesweeper

void minesweeper::MainCallback::onWin(minesweeper::BoardView& bv)
{
    bv.setDiscloseBombs(true);
    QMessageBox::information(&bv, "Finish!", QString::fromUtf8(u8"あなたの勝ちです!"));
}

void minesweeper::MainCallback::onLose(minesweeper::BoardView& bv)
{
    bv.setDiscloseBombs(true);
    QMessageBox::warning(&bv, "...", QString::fromUtf8(u8"あなたの負けです!"));
}
