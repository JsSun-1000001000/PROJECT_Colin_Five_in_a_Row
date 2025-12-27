#include "roomitem.h"
#include "ui_roomitem.h"

RoomItem::RoomItem(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RoomItem)
    , m_roomid(0)
{
    ui->setupUi(this);
}

RoomItem::~RoomItem()
{
    delete ui;
}

/*
 * time 2025.12.26
 * setInfo
 */
void RoomItem::setInfo(int roomid)
{
    m_roomid = roomid;
    QString txt = QString("五子棋——%1房").arg(roomid, 2, 10, QChar('0'));
    /*
     * time 2025.12.27
     * fix the bug: lb_title cant change
     */
    ui->lb_title->setText( txt );
}

void RoomItem::on_pb_join_clicked()
{
    Q_EMIT SIG_JoinRoom( m_roomid );
}

