#include "ai.h"
#include "board.h"
#include "boardview.h"
#include "guimain.h"
#include <wx/wx.h>

using namespace minesweeper;

class Main : public wxApp
{
public:
    virtual bool OnInit() override;
};

wxIMPLEMENT_APP(Main);

bool Main::OnInit()
{
    auto *guimain = new GuiMain();
    guimain->Show(true);
    return true;
}
