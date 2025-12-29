#include "roomdialog.h"
#include "ui_roomdialog.h"
#include <QMessageBox>
#include "packdef.h"

RoomDialog::RoomDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RoomDialog), m_roomid(0),m_status(_player)
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
//#include "packdef.h"
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
    ui->lb_icon_player1->setPixmap(QPixmap(":/images/icon_rui"));
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
    ui->lb_icon_player2->setPixmap(QPixmap(":/images/icon_rui"));
    m_roomUserList.push_back( id );
}

void RoomDialog::clearRoom()
{
    //ui
    ui->lb_player1_name->setText( "等待加入" );
    ui->lb_player2_name->setText( "等待加入" );
    ui->lb_icon_player1->setPixmap(QPixmap(":/Resource2/icon/slotwait.jpg"));
    ui->lb_icon_player2->setPixmap(QPixmap(":/Resource2/icon/slotwait.jpg"));

    //游戏界面清空

    //聊天窗口清空

    //后台数据
    m_roomid = 0;
    m_roomUserList.clear();
    m_status = _player;
}

void RoomDialog::playerLeave(int id)
{
    //ui
    ui->lb_player2_name->setText("等待加入");
    ui->lb_icon_player2->setPixmap(QPixmap(":/Resource2/icon/slotwait.jpg"));
    //data
    for(auto ite = m_roomUserList.begin();ite!=m_roomUserList.end();++ite){
        if(id == *ite){
            ite = m_roomUserList.erase(ite);
            break;
        }
    }
}
//离开房间
void RoomDialog::closeEvent(QCloseEvent *event)
{
    if(QMessageBox::question( this, "exit", "are you sure?")
        == QMessageBox::Yes){
        Q_EMIT SIG_close();
        event->accept();
    }
    else{
        event->ignore();
    }
}
