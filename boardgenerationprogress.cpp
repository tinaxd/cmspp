#include "boardgenerationprogress.h"
#include "ui_boardgenerationprogress.h"

#include <QDialogButtonBox>

using namespace minesweeper;

BoardGenerationProgress::BoardGenerationProgress(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BoardGenerationProgress)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &BoardGenerationProgress::onCancel);
}

BoardGenerationProgress::~BoardGenerationProgress()
{
    delete ui;
}

void BoardGenerationProgress::updateAttempts(int attempts)
{
    ui->lcdNumber->display(attempts);
}
