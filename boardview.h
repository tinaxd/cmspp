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

    static const QColor CLOSED_COLOR;
    static const QColor OPENED_COLOR;
    //static const QColor ASSUMED_OPENED_COLOR;
    static const QColor FLAGGED_COLOR;
    static const QColor ASSUMED_FLAGGED_COLOR;
    static const QColor BACKGROUND_COLOR;
    static const QColor HIGHLIGHT_COLOR;
    static const QColor LINE_COLOR;

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

    void forceRedraw()
    {
        update();
        repaint();
    }
};
}

#endif // BOARDVIEW_H
