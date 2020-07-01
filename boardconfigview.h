#ifndef BOARDCONFIGVIEW_H
#define BOARDCONFIGVIEW_H

#include <wx/wx.h>
#include <optional>
#include <tuple>

namespace minesweeper
{

    class BoardReplaceEvent : public wxEvent
    {
    public:
        BoardReplaceEvent(wxEventType eventType, int winid, int width, int height, int bombs, bool ok)
            : wxEvent(winid, eventType),
              width_(width), height_(height), bombs_(bombs), ok_(ok)
        {
        }
        BoardReplaceEvent(wxEventType eventType, int winid) : BoardReplaceEvent(eventType, winid, 0, 0, 0, false) {}
        BoardReplaceEvent(wxEventType eventType, int winid, int width, int height, int bombs)
            : BoardReplaceEvent(eventType, winid, width, height, bombs, true) {}

        int width() const
        {
            return width_;
        }
        int height() const { return height_; }
        int bombs() const { return bombs_; }

        virtual wxEvent *Clone() const { return new BoardReplaceEvent(*this); }

    private:
        const int width_;
        const int height_;
        const int bombs_;
        const bool ok_;
    };

    class BoardConfigView : public wxWindow
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
