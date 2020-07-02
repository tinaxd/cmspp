#include "boardconfigview.h"

using namespace minesweeper;

wxDEFINE_EVENT(minesweeper::BOARDCONFIG_FINISH, BoardConfigEvent);
wxDEFINE_EVENT(minesweeper::BOARDCONFIG_CANCELED, BoardConfigEvent);

BoardConfigView::BoardConfigView(wxWindow *parent, wxWindowID id)
    : wxFrame(parent, id, "New game"),
      widthInput(new wxTextCtrl(this, wxID_ANY)),
      heightInput(new wxTextCtrl(this, wxID_ANY)),
      bombsInput(new wxTextCtrl(this, wxID_ANY)),
      okButton(new wxButton(this, wxID_ANY, "OK")),
      cancelButton(new wxButton(this, wxID_ANY, "Cancel"))
{
    auto *box = new wxBoxSizer(wxVERTICAL);
    const auto flags = wxSizerFlags().Expand().Proportion(1);
    auto *widthBox = new wxBoxSizer(wxHORIZONTAL);
    widthBox->Add(new wxStaticText(this, wxID_ANY, "Width"));
    widthBox->Add(widthInput);
    auto *heightBox = new wxBoxSizer(wxHORIZONTAL);
    heightBox->Add(new wxStaticText(this, wxID_ANY, "Height"));
    heightBox->Add(heightInput);
    auto *bombBox = new wxBoxSizer(wxHORIZONTAL);
    bombBox->Add(new wxStaticText(this, wxID_ANY, "Bombs"));
    bombBox->Add(bombsInput);
    auto *btnBox = new wxBoxSizer(wxHORIZONTAL);
    btnBox->Add(okButton);
    btnBox->Add(cancelButton);

    box->Add(widthBox, flags);
    box->Add(heightBox, flags);
    box->Add(bombBox, flags);
    box->Add(btnBox, flags);
    SetSizerAndFit(box);

    okButton->Bind(wxEVT_BUTTON, &BoardConfigView::onOkButton, this);
    cancelButton->Bind(wxEVT_BUTTON, &BoardConfigView::onCancelButton, this);
}

BoardConfigView::~BoardConfigView()
{
}

std::optional<BoardConfigView::Data> BoardConfigView::validate() const
{
    const auto &widthText = widthInput->GetLineText(0);
    const auto &heightText = heightInput->GetLineText(0);
    const auto &bombsText = bombsInput->GetLineText(0);
    try
    {
        int width = std::stoi(widthText.ToStdString());
        int height = std::stoi(heightText.ToStdString());
        int bombs = std::stoi(bombsText.ToStdString());
        return std::make_optional(Data(width, height, bombs));
    }
    catch (const std::invalid_argument &e)
    {
        return std::optional<Data>();
    }
    catch (const std::out_of_range &e2)
    {
        return std::optional<Data>();
    }
}

void BoardConfigView::onOkButton(wxCommandEvent &)
{
    const auto &data = validate();
    if (data.has_value())
    {
        const auto &d = data.value();
        int width = std::get<0>(d);
        int height = std::get<1>(d);
        int bombs = std::get<2>(d);
        std::cout << "validate success " << width << " " << height << " " << bombs << std::endl;

        BoardConfigEvent event(BOARDCONFIG_FINISH, GetId(), width, height, bombs);
        event.SetEventObject(this);
        ProcessWindowEvent(event);
    }
    else
    {
        std::cerr << "invalid values" << std::endl;
    }
}

void BoardConfigView::onCancelButton(wxCommandEvent &)
{
    BoardConfigEvent event(BOARDCONFIG_CANCELED, GetId());
    event.SetEventObject(this);
    ProcessWindowEvent(event);
}
