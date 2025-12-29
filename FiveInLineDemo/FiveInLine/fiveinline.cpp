#include "fiveinline.h"
#include "ui_fiveinline.h"

FiveInLine::FiveInLine(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FiveInLine)
{
    ui->setupUi(this);
}

FiveInLine::~FiveInLine()
{
    delete ui;
}
