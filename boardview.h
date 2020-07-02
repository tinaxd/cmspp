#ifndef BOARDVIEW_H
#define BOARDVIEW_H

#include "board.h"
#include "boardgenerationprogress.h"
#include <utility>
#include <memory>
#include <optional>
#include <wx/wx.h>

namespace minesweeper
{

    class BoardView;

    struct WinLoseAction
    {
        virtual ~WinLoseAction() {}
        virtual void onWin(BoardView &bv) = 0;
        virtual void onLose(BoardView &bv) = 0;
    };

    class BoardReplaceEvent;

    class BoardView : public wxWindow
    {
        std::shared_ptr<Board> board;
        bool discloseBombs_ = false;
        std::optional<int> highlightedCell;
        std::unique_ptr<WinLoseAction> finalAction;

        int cellWidth = 42;
        int cellHeight = 42;

        static const wxColour CLOSED_COLOR;
        static const wxColour OPENED_COLOR;
        //static const QColor ASSUMED_OPENED_COLOR;
        static const wxColour FLAGGED_COLOR;
        static const wxColour ASSUMED_FLAGGED_COLOR;
        static const wxColour BACKGROUND_COLOR;
        static const wxColour HIGHLIGHT_COLOR;
        static const wxColour LINE_COLOR;

        std::pair<int, int> getIndexFromMouseCord(double x, double y) const;

        BoardGenerationProgress *progressView = nullptr;

        void judge();

    public:
        explicit BoardView(wxWindow *parent, wxWindowID id);

        // QSize minimumSizeHint() const override;
        // QSize sizeHint() const override;

        // void paintEvent(QPaintEvent* event) override;
        // void mouseMoveEvent(QMouseEvent* event) override;
        // void mousePressEvent(QMouseEvent* event) override;
        void onPaint(wxPaintEvent &ev);
        void onClick(wxMouseEvent &ev);
        void onMove(wxMouseEvent &ev);

        void setCallback(std::unique_ptr<WinLoseAction> finalAction) { this->finalAction = std::move(finalAction); };
        void clearCallback() { finalAction = nullptr; }

        void setDiscloseBombs(bool yes)
        {
            discloseBombs_ = yes;
            Refresh();
        }
        const bool &discloseBombs() const { return discloseBombs_; }

        void setBoard(BoardReplaceEvent &ev);

        void forceRedraw(wxCommandEvent &ev)
        {
            Refresh();
        }

        void onGenerationStarted();
        void onGenerationFinished();
    };
} // namespace minesweeper

#endif // BOARDVIEW_H
