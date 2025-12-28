#include "roomdialog.h"
#include "ui_roomdialog.h"

RoomDialog::RoomDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RoomDialog), m_roomid(0)
{
    ui->setupUi(this);

    /*
     * time 2025.12.28
     * make push button uncheckable
     */
    ui->pb_start->setEnabled( false );
    //关于开局的按键 可以是kernel判断 然后给room发送 可以点击开局

    //点击开局 返回开局信号



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
/*
 * time 2025.12.28
 * set user status
 */
#include "packdef.h"
void RoomDialog::setUserStatus(int status)
{
    m_status = status;
    //只有房主可以点开局
    if( m_status == _host ){

    }
}
/*
 * time 2025.12.28
 * the definition of setHostInfo
 * 设置房主信息
 */
void RoomDialog::setHostInfo(int id, QString name)
{
    ui->lb_player1_name->setText( name );

    m_roomUserList.push_back( id );
}
/*
 *time 2025.12.28
 *the definition of setplayerinfo
 *设置玩家信息
 */
void RoomDialog::setPlayerInfo(int id, QString name)
{
    ui->lb_player2_name->setText( name );

    m_roomUserList.push_back( id );
}
