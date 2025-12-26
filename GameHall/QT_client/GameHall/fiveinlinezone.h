#ifndef FIVEINLINEZONE_H
#define FIVEINLINEZONE_H

#include <QDialog>

namespace Ui {
class FiveInLineZone;
}

class FiveInLineZone : public QDialog
{
    Q_OBJECT

public:
    explicit FiveInLineZone(QWidget *parent = nullptr);
    ~FiveInLineZone();

private:
    Ui::FiveInLineZone *ui;
};

#endif // FIVEINLINEZONE_H
