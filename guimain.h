#ifndef MINESWEEPER_GUIMAIN_H
#define MINESWEEPER_GUIMAIN_H

#include "boardview.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

namespace minesweeper {

struct MainCallback : public WinLoseAction {
    // WinLoseAction interface
public:
    ~MainCallback() {}
    void onWin(BoardView& bv) override;
    void onLose(BoardView& bv) override;
};

class GuiMain : public QMainWindow {
    Q_OBJECT

    Ui::MainWindow* ui;
    QSharedPointer<Board> board;

public:
    GuiMain(QWidget* parent = nullptr);

    void autoSolve(double intervalSeconds);

signals:
    void redrawAll();
    void replaceBoard(QSharedPointer<Board> newBoard);

public slots:
    void newGame(int width, int height, int n_bombs);
    void showNewGameWindow();
    void startAutoSolve();
};

} // namespace minesweeper

Q_DECLARE_METATYPE(QSharedPointer<minesweeper::Board>)

#endif // MINESWEEPER_GUIMAIN_H
