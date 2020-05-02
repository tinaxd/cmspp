#ifndef MINESWEEPER_GUIMAIN_H
#define MINESWEEPER_GUIMAIN_H

#include "boardview.h"
#include <QMainWindow>

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
public:
    GuiMain();

signals:
};

} // namespace minesweeper

#endif // MINESWEEPER_GUIMAIN_H
