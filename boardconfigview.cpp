#include "boardconfigview.h"
#include "ui_boardconfigview.h"
#include <QDialogButtonBox>

using namespace minesweeper;

BoardConfigView::BoardConfigView(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::BoardConfigView)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &BoardConfigView::onOkButton);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &BoardConfigView::onCancelButton);
}

BoardConfigView::~BoardConfigView()
{
    delete ui;
}

std::optional<BoardConfigView::Data> BoardConfigView::validate() const
{
    const auto& widthText = ui->widthInput->text();
    const auto& heightText = ui->heightInput->text();
    const auto& bombsText = ui->bombsInput->text();
    try {
        int width = std::stoi(widthText.toStdString());
        int height = std::stoi(heightText.toStdString());
        int bombs = std::stoi(bombsText.toStdString());
        return std::make_optional(Data(width, height, bombs));
    } catch (const std::invalid_argument& e) {
        return std::optional<Data>();
    } catch (const std::out_of_range& e2) {
        return std::optional<Data>();
    }
}

void BoardConfigView::onOkButton()
{
    const auto& data = validate();
    if (data.has_value()) {
        const auto& d = data.value();
        int width = std::get<0>(d);
        int height = std::get<1>(d);
        int bombs = std::get<2>(d);
        qDebug("validate success %d %d %d", width, height, bombs);
        emit onFinish(width, height, bombs);
    } else {
        qDebug("invalid values");
    }
}

void BoardConfigView::onCancelButton()
{
    emit onCancel();
}