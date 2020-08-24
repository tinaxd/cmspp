#ifndef BOARDCONFIGVIEW_H
#define BOARDCONFIGVIEW_H

#include <QDialog>
#include <optional>
#include <tuple>

namespace Ui {
class BoardConfigView;
}

namespace minesweeper {

class BoardConfigView : public QDialog {
    Q_OBJECT

public:
    explicit BoardConfigView(QWidget *parent = nullptr);
    explicit BoardConfigView(std::optional<int> width, std::optional<int> height, std::optional<int> bombs, QWidget *parent = nullptr);
    ~BoardConfigView();

private:
    using Data = std::tuple<int, int, int>;

    Ui::BoardConfigView* ui;

    std::optional<Data> validate() const;

signals:
    void onFinish(int width, int height, int bombs);
    void onCancel();

private slots:
    void onOkButton();
    void onCancelButton();
};

}

#endif // BOARDCONFIGVIEW_H
