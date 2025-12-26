#ifndef FIVEINLINEZONE_H
#define FIVEINLINEZONE_H

#include <QDialog>
#include "roomitem.h"
#include "QGridLayout"

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

    QGridLayout * m_layout;
};

#endif // FIVEINLINEZONE_H
