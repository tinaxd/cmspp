#ifndef BOARDGENERATIONPROGRESS_H
#define BOARDGENERATIONPROGRESS_H

#include <QDialog>

namespace Ui {
class BoardGenerationProgress;
}

namespace minesweeper {

class BoardGenerationProgress : public QDialog
{
    Q_OBJECT

public:
    explicit BoardGenerationProgress(QWidget *parent = nullptr);
    ~BoardGenerationProgress();

private:
    Ui::BoardGenerationProgress* ui;

signals:
    void onCancel();

public slots:
    void updateAttempts(int attempts);
};

}

#endif // BOARDGENERATIONPROGRESS_H
