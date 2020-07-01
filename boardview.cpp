#include "boardview.h"

using namespace minesweeper;

const wxColour BoardView::CLOSED_COLOR{0xa9, 0xa9, 0xa9};
const wxColour BoardView::OPENED_COLOR{0x00, 0x64, 0x00};
const wxColour BoardView::FLAGGED_COLOR{0xff, 0x45, 0x00};
const wxColour BoardView::BACKGROUND_COLOR{0xd3, 0xd3, 0xd3};
const wxColour BoardView::HIGHLIGHT_COLOR{0xad, 0xd8, 0xe6};
const wxColour BoardView::LINE_COLOR{0xf0, 0xf8, 0xff};
//const QColor BoardView::ASSUMED_OPENED_COLOR { 0xa4, 0xd6, 0xa4 };
const wxColour BoardView::ASSUMED_FLAGGED_COLOR{0xbf, 0xb2, 0x5f};

BoardView::BoardView(wxWindow *parent,
                     wxWindowID id)
    : wxWindow(parent, id)
{
    //setMouseTracking(true);
}

// QSize BoardView::sizeHint() const
// {
//     if (board.isNull())
//     {
//         return QSize(cellWidth, cellHeight);
//     }
//     return QSize(cellWidth * board->width(), cellHeight * board->height());
// }

// QSize BoardView::minimumSizeHint() const
// {
//     return sizeHint();
// }

void BoardView::onPaint(wxPaintEvent &ev)
{
    wxPaintDC painter(this);

    const double width = this->GetClientSize().GetWidth();
    const double height = this->GetClientSize().GetHeight();

    painter.SetBrush(BACKGROUND_COLOR);
    painter.DrawRectangle(0, 0, width, height);

    if (!board)
    {
        return;
    }

    const auto nColumns = board->width();
    const auto nRows = board->height();

    const auto cellWidth = width / nColumns;
    const auto cellHeight = height / nRows;

    const double margin = 5.0;

    painter.SetPen(LINE_COLOR);
    painter.SetFont(wxFont(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT));

    for (auto x = 0; x < nColumns; x++)
    {
        for (auto y = 0; y < nRows; y++)
        {
            auto cellIndex = board->from_point(x, y);
            auto initX = (width * x) / nColumns;
            auto initY = (height * y) / nRows;
            auto drawNumber = false;
            auto flagged = false;

            auto &cell = (*board)[cellIndex];
            switch (cell.state())
            {
            case CellState::Closed:
                if (highlightedCell.has_value() && highlightedCell.value() == cellIndex)
                {
                    painter.SetBrush(HIGHLIGHT_COLOR);
                }
                else
                {
                    painter.SetBrush(CLOSED_COLOR);
                }
                break;
            case CellState::Flagged:
                painter.SetBrush(CLOSED_COLOR);
                flagged = true;
                break;
            case CellState::Opened:
                drawNumber = true;
                painter.SetBrush(OPENED_COLOR);
                break;
            }

            painter.DrawRectangle(initX, initY, cellWidth - margin, cellHeight - margin);

            if (drawNumber)
            {
                painter.SetBrush(wxColour(0xdb, 0x70, 0x93));
                painter.SetPen(wxColour(0xdb, 0x70, 0x93));
                if (cell.is_assumption())
                {
                    painter.DrawText("?", wxRealPoint{initX + (cellWidth / 3), initY + (cellHeight / 2)});
                }
                else
                {
                    int bombs = cell.neighbor_bombs();
                    if (bombs != 0)
                    {
                        painter.DrawText(std::to_string(bombs), wxRealPoint{initX + (cellWidth / 3), initY + (cellHeight / 2)});
                    }
                }
            }

            if (flagged)
            {
                if (cell.is_assumption())
                {
                    painter.SetBrush(ASSUMED_FLAGGED_COLOR);
                }
                else
                {
                    painter.SetBrush(FLAGGED_COLOR);
                }
                auto cellWidthM = cellWidth - margin;
                auto cellHeightM = cellHeight - margin;
                auto flagWidth = cellWidthM / 2.0;
                auto flagHeight = cellHeightM / 2.0;
                auto initFX = initX + (cellWidthM / 4.0);
                auto initFY = initY + (cellHeightM / 4.0);
                painter.DrawRectangle(initFX, initFY, flagWidth, flagHeight);
            }

            if (discloseBombs_ && cell.has_bomb())
            {
                painter.SetPen(LINE_COLOR);
                auto cellWidthM = cellWidth - margin;
                auto cellHeightM = cellHeight - margin;
                wxRealPoint p1{initX + (cellWidthM / 5.0), initY + (cellHeightM / 5.0)};
                wxRealPoint p2{initX + (cellWidthM * 4.0 / 5.0), initY + (cellHeightM * 4.0 / 5.0)};
                wxRealPoint p3{initX + (cellWidthM * 4.0 / 5.0), initY + (cellWidthM / 5.0)};
                wxRealPoint p4{initX + (cellWidthM / 5.0), initY + (cellHeightM * 4.0 / 5.0)};
                painter.DrawLine(p1, p2);
                painter.DrawLine(p3, p4);
            }
        }
    }
}

std::pair<int, int> BoardView::getIndexFromMouseCord(double x, double y) const
{
    const auto width = static_cast<double>(this->GetClientSize().GetWidth());
    const auto height = static_cast<double>(this->GetClientSize().GetHeight());

    const auto nColumns = board->width();
    const auto nRows = board->height();

    const auto cellWidth = width / nColumns;
    const auto cellHeight = height / nRows;

    return std::make_pair<int, int>(static_cast<int>(x / cellWidth), static_cast<int>(y / cellHeight));
}

void BoardView::onMove(wxMouseEvent &ev)
{
    if (!board)
    {
        highlightedCell = std::optional<int>();
        return;
    }

    const auto x = ev.GetX();
    const auto y = ev.GetY();

    const auto res = getIndexFromMouseCord(x, y);

    const auto xIndex = res.first;
    const auto yIndex = res.second;

    if (xIndex < 0 || xIndex >= board->width() || yIndex < 0 || yIndex >= board->height())
    {
        highlightedCell = std::optional<int>();
        return;
    }

    highlightedCell = std::make_optional(board->from_point(xIndex, yIndex));
    Refresh();
}

void BoardView::onClick(wxMouseEvent &ev)
{
    if (!board)
    {
        return;
    }

    const auto x = ev.GetX();
    const auto y = ev.GetY();
    std::cerr << "clicked: " << x << " " << y << std::endl;

    const auto res = getIndexFromMouseCord(x, y);

    const auto xIndex = res.first;
    const auto yIndex = res.second;

    if (xIndex < 0 || xIndex >= board->width() || yIndex < 0 || yIndex >= board->height())
    {
        std::cerr << "ignore click" << std::endl;
        return;
    }

    if (ev.GetButton() == wxMOUSE_BTN_LEFT)
    {
        if ((*board)[res].state() == CellState::Flagged)
        {
            std::cout << "flag block" << std::endl;
        }
        else
        {
            std::cout << "open cell " << xIndex << " " << yIndex << std::endl;
            board->open_cell(xIndex, yIndex);
        }
    }
    else if (ev.GetButton() == wxMOUSE_BTN_RIGHT)
    {
        std::cout << "toggle-flag cell " << xIndex << " " << yIndex << std::endl;
        board->toggle_flag(xIndex, yIndex);
    }
    Refresh();
    judge();
}

void BoardView::setBoard(std::shared_ptr<Board> board)
{
    this->board = board;
    connect(this->board.get(), &Board::generationStarted, this, &BoardView::onGenerationStarted);
    connect(this->board.get(), &Board::generationFinished, this, &BoardView::onGenerationFinished);
    setDiscloseBombs(false);
    Refresh();
}

void BoardView::onGenerationStarted()
{
    if (progressView != nullptr)
    {
        std::cerr << "remove previous progress view" << std::endl;
        progressView->Destroy();
        progressView = nullptr;
    }

    std::cerr << "showing new progress view" << std::endl;
    progressView = new BoardGenerationProgress(this, wxID_ANY);
    connect(this->board.get(), &Board::onAttempt, progressView, &BoardGenerationProgress::updateAttempts);
    progressView->Show();
}

void BoardView::onGenerationFinished()
{
    std::cerr << "removing progressView: " << progressView << std::endl;
    if (progressView != nullptr)
    {
        progressView->Hide();
        progressView->Destroy();
        progressView = nullptr;
    }
}

void BoardView::judge()
{
    if (!board)
    {
        return;
    }

    if (finalAction == nullptr)
    {
        return;
    }

    if (board->failed())
    {
        finalAction->onLose(*this);
    }
    else if (board->cleared())
    {
        finalAction->onWin(*this);
    }
}
