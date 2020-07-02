#include "guimain.h"
#include "ai.h"
#include "board.h"
#include "boardconfigview.h"
#include "boardview.h"
#include <thread>

namespace minesweeper
{

    wxDEFINE_EVENT(MAIN_REDRAW_ALL, wxCommandEvent);
    wxDEFINE_EVENT(MAIN_REPLACE_BOARD, BoardReplaceEvent);

    struct RedrawCallback : public AICallback
    {
    private:
        GuiMain *gm;
        double intervalSeconds;
        int lastNestLevel = 0;

    public:
        RedrawCallback(GuiMain *gm, double intervalSeconds)
            : gm(gm), intervalSeconds(intervalSeconds)
        {
        }
        ~RedrawCallback() {}

        // AICallback interface
    public:
        void before_start(const std::shared_ptr<Board> &) override
        {
        }

        bool on_step(const std::shared_ptr<Board> &board, int /*current_step*/, int nest_level) override
        {
            if (board->failed() || board->cleared())
                return false;
            if (lastNestLevel != nest_level)
            {
                BoardReplaceEvent event(MAIN_REPLACE_BOARD, gm->GetId(), board);
                event.SetEventObject(gm);
                gm->ProcessWindowEvent(event);
            }
            wxCommandEvent redr(MAIN_REDRAW_ALL, gm->GetId());
            redr.SetEventObject(gm);
            gm->ProcessWindowEvent(redr);
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long>(intervalSeconds * 1000)));
            return true;
        }
    };

    GuiMain::GuiMain()
        : wxFrame(nullptr, wxID_ANY, "Logical Sweeper", wxDefaultPosition, {700, 700}), central(new BoardView(this, wxID_ANY))
    {
        auto *sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(central, wxSizerFlags().Expand().Proportion(1));
        SetSizerAndFit(sizer);
        SetMinSize({400, 400});

        auto *game = new wxMenu;
        auto *newGame = game->Append(wxID_ANY, "New game");
        auto *showAnswer = game->Append(wxID_ANY, "Show answer");
        auto *menubar = new wxMenuBar;
        menubar->Append(game, "&Game");
        SetMenuBar(menubar);

        Bind(MAIN_REDRAW_ALL, &BoardView::forceRedraw, central);
        Bind(MAIN_REPLACE_BOARD, &BoardView::setBoard, central);

        Bind(wxEVT_MENU, &GuiMain::showNewGameWindow, this, newGame->GetId());
        Bind(wxEVT_MENU, &GuiMain::startAutoSolve, this, showAnswer->GetId());

        central->setCallback(std::make_unique<MainCallback>());

        this->newGame(16, 16, 32);
        std::cout << "GUI initialized" << std::endl;
    }

    void GuiMain::autoSolve(double intervalSeconds)
    {
        // QThread *thread = QThread::create([board = this->board, this, intervalSeconds]() {
        //     RedrawCallback rc(this, intervalSeconds);
        //     try
        //     {
        //         MineAI::solve_all(board, true, rc);
        //     }
        //     catch (const AIReasoningError &e)
        //     {
        //         qDebug("WARNING: this is unsolvable!");
        //     }
        // });
        // thread->start();
        std::cerr << "This feature is disabled now." << std::endl;
    }

    void GuiMain::newGame(int width, int height, int n_bombs)
    {
        board = std::shared_ptr<Board>(new LazyInitBoard(width, height, n_bombs, true));
        BoardReplaceEvent event(MAIN_REPLACE_BOARD, GetId(), board);
        event.SetEventObject(this);
        ProcessWindowEvent(event);
    }

    void GuiMain::showNewGameWindow(wxCommandEvent &)
    {
        auto *config = new BoardConfigView(this, wxID_ANY);
        // connect(config, &BoardConfigView::onFinish, [this, config](int width, int height, int bombs) {
        //     newGame(width, height, bombs);
        //     config->deleteLater();
        // });
        // connect(config, &BoardConfigView::onCancel, config, &QWidget::deleteLater);
        config->Bind(BOARDCONFIG_FINISH, [this, config](BoardConfigEvent &ev) {
            newGame(ev.width(), ev.height(), ev.bombs());
            config->Destroy();
        });
        config->Bind(BOARDCONFIG_CANCELED, [config](BoardConfigEvent &) { config->Destroy(); });
        config->Show();
    }

    void GuiMain::startAutoSolve(wxCommandEvent &)
    {
        autoSolve(0.2);
    }

    void MainCallback::onWin(minesweeper::BoardView &bv)
    {
        bv.setDiscloseBombs(true);
        wxMessageDialog dialog(&bv, "あなたの勝ちです!", "Finish!", wxOK | wxCENTRE);
        dialog.ShowModal();
    }

    void MainCallback::onLose(minesweeper::BoardView &bv)
    {
        bv.setDiscloseBombs(true);
        wxMessageDialog dialog(&bv, "あなたの負けです!", "...", wxOK | wxCENTRE);
        dialog.ShowModal();
    }

} // namespace minesweeper
