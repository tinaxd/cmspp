#ifndef MINESWEEPER_GUIMAIN_H
#define MINESWEEPER_GUIMAIN_H

#include "boardview.h"
#include <wx/wx.h>
#include <memory>

namespace minesweeper
{

    wxDECLARE_EVENT(MAIN_REDRAW_ALL, wxCommandEvent);
    struct BoardReplaceEvent : wxEvent
    {
        BoardReplaceEvent(wxEventType eventType, int winid, std::shared_ptr<Board> newBoard)
            : wxEvent(winid, eventType), newBoard(std::move(newBoard)) {}

        virtual wxEvent *Clone() const { return new BoardReplaceEvent(*this); }

        std::shared_ptr<Board> newBoard;
    };

    struct MainCallback : public WinLoseAction
    {
        // WinLoseAction interface
    public:
        ~MainCallback() {}
        void onWin(BoardView &bv) override;
        void onLose(BoardView &bv) override;
    };

    class GuiMain : public wxFrame
    {
        std::shared_ptr<Board> board;

    public:
        GuiMain();

        void autoSolve(double intervalSeconds);

        void newGame(int width, int height, int n_bombs);
        void showNewGameWindow(wxCommandEvent &ev);
        void startAutoSolve(wxCommandEvent &ev);
    
    private:
        BoardView *central;
    };

} // namespace minesweeper

#endif // MINESWEEPER_GUIMAIN_H
