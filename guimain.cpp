#include "guimain.h"
#include "ai.h"
#include "board.h"
#include "boardview.h"
#include <QMessageBox>
#include <QString>
#include <QThread>

namespace minesweeper {

struct RedrawCallback : public AICallback {
private:
    GuiMain* gm;
    double intervalSeconds;
    int lastNestLevel = 0;

public:
    RedrawCallback(GuiMain* gm, double intervalSeconds)
        : gm(gm)
        , intervalSeconds(intervalSeconds)
    {
    }
    ~RedrawCallback() {}

    // AICallback interface
public:
    void before_start(const QSharedPointer<Board>&) override
    {
    }

    bool on_step(const QSharedPointer<Board>& board, int /*current_step*/, int nest_level) override
    {
        if (board->failed() || board->cleared())
            return false;
        if (lastNestLevel != nest_level) {
            gm->replaceBoard(board);
        }
        gm->redrawAll();
        QThread::msleep(intervalSeconds * 1000);
        return true;
    }
};

GuiMain::GuiMain()
{
    qRegisterMetaType<QSharedPointer<Board>>("QSharedPointer<Board>");

    view = new BoardView();
    view->setCallback(std::make_unique<MainCallback>());

    setGeometry(30, 30, 700, 700);
    setWindowTitle("Cane MineSweeper");
    setCentralWidget(view);

    connect(this, &GuiMain::redrawAll, view, &BoardView::forceRedraw);
    connect(this, &GuiMain::replaceBoard, view, &BoardView::setBoard);

    newGame(16, 16, 32);
}

void GuiMain::autoSolve(double intervalSeconds)
{
    QThread* thread = QThread::create([board = this->board, this, intervalSeconds]() {
        RedrawCallback rc(this, intervalSeconds);
        try {
            MineAI::solve_all(board, true, rc);
        } catch (const AIReasoningError& e) {
            qDebug("WARNING: this is unsolvable!");
        }
    });
    thread->start();
}

void GuiMain::newGame(int width, int height, int n_bombs)
{
    QSharedPointer<LazyInitBoard> board(new LazyInitBoard(width, height, n_bombs));
    emit replaceBoard(board);
}

void MainCallback::onWin(minesweeper::BoardView& bv)
{
    bv.setDiscloseBombs(true);
    QMessageBox::information(&bv, "Finish!", QString::fromUtf8(u8"あなたの勝ちです!"));
}

void MainCallback::onLose(minesweeper::BoardView& bv)
{
    bv.setDiscloseBombs(true);
    QMessageBox::warning(&bv, "...", QString::fromUtf8(u8"あなたの負けです!"));
}

} // namespace minesweeper
