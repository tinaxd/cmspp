#include "guimain.h"
#include "ai.h"
#include "board.h"
#include "boardconfigview.h"
#include "boardview.h"
#include "ui_mainwindow.h"
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

GuiMain::GuiMain(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qRegisterMetaType<QSharedPointer<Board>>("QSharedPointer<Board>");

    setGeometry(30, 30, 700, 700);
    setWindowTitle("Cane MineSweeper");

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
    board = QSharedPointer<Board>(new LazyInitBoard(width, height, n_bombs, true));
    emit replaceBoard(board);
}

void GuiMain::showNewGameWindow()
{
    std::optional<int> width, height, bombs;
    if (board) {
        width = board->width();
        height = board->height();
        bombs = board->init_bombs();
    }
    auto* config = new BoardConfigView(width, height, bombs, this);
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
