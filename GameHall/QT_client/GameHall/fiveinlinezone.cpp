#include "fiveinlinezone.h"
#include "ui_fiveinlinezone.h"

FiveInLineZone::FiveInLineZone(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FiveInLineZone)
{
    ui->setupUi(this);
}

FiveInLineZone::~FiveInLineZone()
{
    delete ui;
}
