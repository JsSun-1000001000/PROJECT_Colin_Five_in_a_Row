#include "roomitem.h"
#include "ui_roomitem.h"

RoomItem::RoomItem(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RoomItem)
{
    ui->setupUi(this);
}

RoomItem::~RoomItem()
{
    delete ui;
}
