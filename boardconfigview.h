#ifndef BOARDCONFIGVIEW_H
#define BOARDCONFIGVIEW_H

#include <wx/wx.h>
#include <optional>
#include <tuple>

namespace minesweeper
{

    class BoardConfigEvent : public wxEvent
    {
    public:
        BoardConfigEvent(wxEventType eventType, int winid, int width, int height, int bombs, bool ok)
            : wxEvent(winid, eventType),
              width_(width), height_(height), bombs_(bombs), ok_(ok)
        {
        }
        BoardConfigEvent(wxEventType eventType, int winid) : BoardConfigEvent(eventType, winid, 0, 0, 0, false) {}
        BoardConfigEvent(wxEventType eventType, int winid, int width, int height, int bombs)
            : BoardConfigEvent(eventType, winid, width, height, bombs, true) {}

        int width() const
        {
            return width_;
        }
        int height() const { return height_; }
        int bombs() const { return bombs_; }

        virtual wxEvent *Clone() const { return new BoardConfigEvent(*this); }

    private:
        const int width_;
        const int height_;
        const int bombs_;
        const bool ok_;
    };

    wxDECLARE_EVENT(BOARDCONFIG_FINISH, BoardConfigEvent);
    wxDECLARE_EVENT(BOARDCONFIG_CANCELED, BoardConfigEvent);

    class BoardConfigView : public wxFrame
    {
    public:
        explicit BoardConfigView(wxWindow *parent, wxWindowID id);
        ~BoardConfigView();

    private:
        using Data = std::tuple<int, int, int>;

        std::optional<Data> validate() const;

        wxTextCtrl *widthInput;
        wxTextCtrl *heightInput;
        wxTextCtrl *bombsInput;
        wxButton *okButton;
        wxButton *cancelButton;

        void onFinish(int width, int height, int bombs);
        void onCancel();

        void onOkButton(wxCommandEvent &ev);
        void onCancelButton(wxCommandEvent &ev);
    };

} // namespace minesweeper

#endif // BOARDCONFIGVIEW_H
