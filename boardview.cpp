#include "boardview.h"
#include <QBrush>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>

using namespace minesweeper;

BoardView::BoardView(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
}

QSize BoardView::sizeHint() const
{
    if (board.isNull()) {
        return QSize(cellWidth, cellHeight);
    }
    return QSize(cellWidth * board->width(), cellHeight * board->height());
}

QSize BoardView::minimumSizeHint() const
{
    return sizeHint();
}

void BoardView::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    const double width = this->width();
    const double height = this->height();

    painter.setBrush(BACKGROUND_COLOR);
    painter.drawRect(0, 0, width, height);

    if (board.isNull()) {
        return;
    }

    const auto nColumns = board->width();
    const auto nRows = board->height();

    const auto cellWidth = width / nColumns;
    const auto cellHeight = height / nRows;

    const double margin = 5.0;

    painter.setPen(LINE_COLOR);
    QFont font;
    font.setPointSize(24);
    painter.setFont(font);

    for (auto x = 0; x < nColumns; x++) {
        for (auto y = 0; y < nRows; y++) {
            auto cellIndex = board->from_point(x, y);
            auto initX = (width * x) / nColumns;
            auto initY = (height * y) / nRows;
            auto drawNumber = false;
            auto flagged = false;

            auto& cell = (*board)[cellIndex];
            switch (cell.state()) {
            case CellState::Closed:
                if (highlightedCell.has_value() && highlightedCell.value() == cellIndex) {
                    painter.setBrush(HIGHLIGHT_COLOR);
                } else {
                    painter.setBrush(CLOSED_COLOR);
                }
                break;
            case CellState::Flagged:
                painter.setBrush(CLOSED_COLOR);
                flagged = true;
                break;
            case CellState::Opened:
                drawNumber = true;
                painter.setBrush(OPENED_COLOR);
                break;
            }

            painter.drawRect(initX, initY, cellWidth - margin, cellHeight - margin);

            if (drawNumber) {
                int bombs = cell.neighbor_bombs();
                if (bombs != 0) {
                    painter.setBrush(QColor(0xdb, 0x70, 0x93));
                    painter.setPen(QColor(0xdb, 0x70, 0x93));
                    auto text = QString::fromStdString(std::to_string(bombs));
                    painter.drawText(QPointF { initX + (cellWidth / 3), initY + (cellHeight / 2) }, text);
                }
            }

            if (flagged) {
                painter.setBrush(FLAGGED_COLOR);
                auto cellWidthM = cellWidth - margin;
                auto cellHeightM = cellHeight - margin;
                auto flagWidth = cellWidthM / 2.0;
                auto flagHeight = cellHeightM / 2.0;
                auto initFX = initX + (cellWidthM / 4.0);
                auto initFY = initY + (cellHeightM / 4.0);
                painter.drawRect(initFX, initFY, flagWidth, flagHeight);
            }

            if (discloseBombs_ && cell.has_bomb()) {
                painter.setPen(LINE_COLOR);
                auto cellWidthM = cellWidth - margin;
                auto cellHeightM = cellHeight - margin;
                QPointF p1 { initX + (cellWidthM / 5.0), initY + (cellHeightM / 5.0) };
                QPointF p2 { initX + (cellWidthM * 4.0 / 5.0), initY + (cellHeightM * 4.0 / 5.0) };
                QPointF p3 { initX + (cellWidthM * 4.0 / 5.0), initY + (cellWidthM / 5.0) };
                QPointF p4 { initX + (cellWidthM / 5.0), initY + (cellHeightM * 4.0 / 5.0) };
                painter.drawLines(QVector<QLineF> { QLineF(p1, p2), QLineF(p3, p4) });
            }
        }
    }
}

QPair<int, int> BoardView::getIndexFromMouseCord(double x, double y) const
{
    const auto width = static_cast<double>(this->width());
    const auto height = static_cast<double>(this->height());

    const auto nColumns = board->width();
    const auto nRows = board->height();

    const auto cellWidth = width / nColumns;
    const auto cellHeight = height / nRows;

    return qMakePair<int, int>(static_cast<int>(x / cellWidth), static_cast<int>(y / cellHeight));
}

void BoardView::mouseMoveEvent(QMouseEvent* event)
{
    if (board.isNull()) {
        highlightedCell = std::optional<int>();
        return;
    }

    const auto x = event->x();
    const auto y = event->y();

    const auto res = getIndexFromMouseCord(x, y);

    const auto xIndex = res.first;
    const auto yIndex = res.second;

    if (xIndex < 0 || xIndex >= board->width() || yIndex < 0 || yIndex >= board->height()) {
        highlightedCell = std::optional<int>();
        return;
    }

    highlightedCell = std::make_optional(board->from_point(xIndex, yIndex));
    update();
    repaint();
}

void BoardView::mousePressEvent(QMouseEvent* event)
{
    if (board.isNull()) {
        return;
    }

    const auto x = event->x();
    const auto y = event->y();
    qDebug("clicked: %d %d", x, y);

    const auto res = getIndexFromMouseCord(x, y);

    const auto xIndex = res.first;
    const auto yIndex = res.second;

    if (xIndex < 0 || xIndex >= board->width() || yIndex < 0 || yIndex >= board->height()) {
        qDebug("ignore click");
        return;
    }

    if (event->button() == Qt::LeftButton) {
        if ((*board)[res].state() == CellState::Flagged) {
            qDebug("flag block");
        } else {
            qDebug("open cell %d %d", xIndex, yIndex);
            board->open_cell(xIndex, yIndex);
        }
    } else if (event->button() == Qt::RightButton) {
        qDebug("toggle-flag cell %d %d", xIndex, yIndex);
        board->toggle_flag(xIndex, yIndex);
    }
    update();
    repaint();
    judge();
}

void BoardView::judge()
{
    if (board.isNull()) {
        return;
    }

    if (finalAction == nullptr) {
        return;
    }

    if (board->failed()) {
        finalAction->onLose(*this);
    } else if (board->cleared()) {
        finalAction->onWin(*this);
    }
}