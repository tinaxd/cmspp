#ifndef BOARDVIEW_H
#define BOARDVIEW_H

#include "board.h"
#include <QColor>
#include <QPair>
#include <QSharedPointer>
#include <QWidget>
#include <memory>
#include <optional>

namespace minesweeper {

class BoardView;

struct WinLoseAction {
    virtual ~WinLoseAction() {}
    virtual void onWin(BoardView& bv) = 0;
    virtual void onLose(BoardView& bv) = 0;
};

class BoardView : public QWidget {
    Q_OBJECT

    QSharedPointer<Board> board;
    bool discloseBombs_ = false;
    std::optional<int> highlightedCell;
    std::unique_ptr<WinLoseAction> finalAction;

    int cellWidth = 42;
    int cellHeight = 42;

    static constexpr QColor CLOSED_COLOR { 0xa9, 0xa9, 0xa9 };
    static constexpr QColor OPENED_COLOR { 0x00, 0x64, 0x00 };
    static constexpr QColor FLAGGED_COLOR { 0xff, 0x45, 0x00 };
    static constexpr QColor BACKGROUND_COLOR { 0xd3, 0xd3, 0xd3 };
    static constexpr QColor HIGHLIGHT_COLOR { 0xad, 0xd8, 0xe6 };
    static constexpr QColor LINE_COLOR { 0xf0, 0xf8, 0xff };

    QPair<int, int> getIndexFromMouseCord(double x, double y) const;

    void judge();

public:
    explicit BoardView(QWidget* parent = nullptr);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

    void setCallback(std::unique_ptr<WinLoseAction> finalAction) { this->finalAction = std::move(finalAction); };
    void clearCallback() { finalAction = nullptr; }

    void setDiscloseBombs(bool yes)
    {
        discloseBombs_ = yes;
        update();
        repaint();
    }
    const bool& discloseBombs() const { return discloseBombs_; }

public slots:
    void setBoard(QSharedPointer<Board> board)
    {
        this->board = board;
        setDiscloseBombs(false);
        update();
        repaint();
    };
};
}

#endif // BOARDVIEW_H
