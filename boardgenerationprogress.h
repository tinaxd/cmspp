#ifndef BOARDGENERATIONPROGRESS_H
#define BOARDGENERATIONPROGRESS_H

#include <wx/wx.h>

namespace minesweeper {

wxDECLARE_EVENT(BGP_CANCELED, wxCommandEvent);

class BoardGenerationProgress : public wxFrame
{
public:
    explicit BoardGenerationProgress(wxWindow *parent, wxWindowID id);
    ~BoardGenerationProgress();

    void onCancel(wxCommandEvent &ev);
    void updateAttempts(int attempts);

private:
    wxStaticText *lcd;
};

}

#endif // BOARDGENERATIONPROGRESS_H
