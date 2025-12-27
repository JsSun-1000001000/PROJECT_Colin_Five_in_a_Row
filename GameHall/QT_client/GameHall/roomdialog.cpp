#include "roomdialog.h"
#include "ui_roomdialog.h"

RoomDialog::RoomDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RoomDialog), m_roomid(0)
{
    ui->setupUi(this);
}

RoomDialog::~RoomDialog()
{
    delete ui;
}

void RoomDialog::setInfo(int roomid)
{
    m_roomid = roomid;

    QString txt = QString("五子棋-%1").arg(roomid, 2, 10, QChar('0'));
    this->setWindowTitle( txt );
}
