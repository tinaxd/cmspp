#include "boardgenerationprogress.h"

using namespace minesweeper;

wxDEFINE_EVENT(minesweeper::BGP_CANCELED, wxCommandEvent);

BoardGenerationProgress::BoardGenerationProgress(wxWindow *parent, wxWindowID id) :
    wxFrame(parent, id, "generating board..."),
    lcd(new wxStaticText(this, wxID_ANY, ""))
{
    auto *box = new wxBoxSizer(wxVERTICAL);
    auto *cancel = new wxButton(this, wxID_ANY, "Cancel");
    box->Add(lcd);
    box->Add(cancel);
    SetSizerAndFit(box);

    cancel->Bind(wxEVT_BUTTON, &BoardGenerationProgress::onCancel, this);
}

BoardGenerationProgress::~BoardGenerationProgress()
{
}

void BoardGenerationProgress::onCancel(wxCommandEvent&)
{
    wxCommandEvent cancel(BGP_CANCELED, GetId());
    cancel.SetEventObject(this);
    ProcessWindowEvent(cancel);
}

void BoardGenerationProgress::updateAttempts(int attempts)
{
    lcd->SetLabelText(std::to_string(attempts));
}
