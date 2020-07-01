#include "guimain.h"
#include "ai.h"
#include "board.h"
#include "boardconfigview.h"
#include "boardview.h"

namespace minesweeper {

wxDEFINE_EVENT(MAIN_REDRAW_ALL, wxCommandEvent);
wxDEFINE_EVENT(MAIN_REPLACE_BOARD, BoardReplaceEvent);

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
    void before_start(const std::shared_ptr<Board>&) override
    {
    }

    bool on_step(const std::shared_ptr<Board>& board, int /*current_step*/, int nest_level) override
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
    : wxFrame(nullptr, wxID_ANY, "Logical Sweeper")
{
    SetSize({700, 700});

    connect(this, &GuiMain::redrawAll, ui->centralwidget, &BoardView::forceRedraw);
    connect(this, &GuiMain::replaceBoard, ui->centralwidget, &BoardView::setBoard);

    connect(ui->actionNew_Game, &QAction::triggered, this, &GuiMain::showNewGameWindow);
    connect(ui->actionShow_Answer, &QAction::triggered, this, &GuiMain::startAutoSolve);

    ui->centralwidget->setCallback(std::make_unique<MainCallback>());

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
    board = std::shared_ptr<Board>(new LazyInitBoard(width, height, n_bombs, true));
    BoardReplaceEvent event(MAIN_REPLACE_BOARD, GetId(), board);
    event.SetEventObject(this);
    ProcessWindowEvent(event);
}

void GuiMain::showNewGameWindow()
{
    auto* config = new BoardConfigView(this);
    connect(config, &BoardConfigView::onFinish, [this, config](int width, int height, int bombs) {
        newGame(width, height, bombs);
        config->deleteLater();
    });
    connect(config, &BoardConfigView::onCancel, config, &QWidget::deleteLater);
    config->show();
}

void GuiMain::startAutoSolve()
{
    autoSolve(0.2);
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
