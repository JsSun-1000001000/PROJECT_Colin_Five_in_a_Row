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

    //五子棋类 发送的信号 转发给kernel
    connect( ui->wdg_play, SIGNAL(SIG_pieceDown(int,int,int)),
            this, SIGNAL(SIG_pieceDown(int,int,int)) );
    connect( ui->wdg_play, SIGNAL(SIG_playerWin(int)),
            this, SIGNAL(SIG_playerWin(int)) );

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
    ui->wdg_play->setSelfStatus( m_status == _host?FiveInLine::Black:FiveInLine::White );
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
    ui->pb_start->setEnabled( false );
    ui->pb_player1_ready->setEnabled( true );
    ui->pb_player1_ready->setChecked( false );
    ui->pb_player2_ready->setEnabled( true );
    ui->pb_player2_ready->setChecked( false );
    ui->pb_player1_ready->setText("待准备");
    ui->pb_player2_ready->setText("待准备");
    //聊天窗口清空

    //后台数据
    m_roomid = 0;
    m_roomUserList.clear();
    m_status = _player;
}

void RoomDialog::resetAllPushButton()
{
    ui->pb_start->setEnabled( false );
    ui->pb_player1_ready->setEnabled( true );
    ui->pb_player1_ready->setChecked( false );
    ui->pb_player2_ready->setEnabled( true );
    ui->pb_player2_ready->setChecked( false );
    ui->pb_player1_ready->setText("待准备");
    ui->pb_player2_ready->setText("待准备");
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

void RoomDialog::setPlayerReady(int id)
{
    /*if( m_roomUserList.size() == id ){
        ui->pb_player1_ready->setChecked( true );
        ui->pb_player1_ready->setText( "已准备" );
    }
    if( m_roomUserList.back() == id ){
        ui->pb_player2_ready->setChecked( true );
        ui->pb_player2_ready->setText( "已准备" );
    }
    if( ui->pb_player1_ready->isChecked() &&
        ui->pb_player2_ready->isChecked() ){
        //都准备可以开始游戏
        if( m_status == _host ){
            ui->pb_start->setEnabled( true );
        }
    }*/
    // 在 m_roomUserList 中找到 id，确定它是第几个槽位（0 为 host/slot1, 1 为 player/slot2），并设置对应 ready 状态
    int pos = 0;
    bool found = false;
    for(auto ite = m_roomUserList.begin(); ite != m_roomUserList.end(); ++ite, ++pos){
        if(id == *ite){
            found = true;
            if(pos == 0){
                ui->pb_player1_ready->setChecked(true);
                ui->pb_player1_ready->setText("已准备");
            } else if(pos == 1){
                ui->pb_player2_ready->setChecked(true);
                ui->pb_player2_ready->setText("已准备");
            }
            break;
        }
    }

    if(!found){
        // 兜底：按 front/back 判断
        if(!m_roomUserList.empty() && m_roomUserList.front() == id){
            ui->pb_player1_ready->setChecked(true);
            ui->pb_player1_ready->setText("已准备");
        } else if(!m_roomUserList.empty() && m_roomUserList.back() == id){
            ui->pb_player2_ready->setChecked(true);
            ui->pb_player2_ready->setText("已准备");
        }
    }

    // 检查是否两边都 ready，若是并且本端是房主则启用 start
    if( ui->pb_player1_ready->isChecked() && ui->pb_player2_ready->isChecked() ){
        if( m_status == _host ){
            ui->pb_start->setEnabled(true);
        }
    } else {
        if( m_status == _host ){
            ui->pb_start->setEnabled(false);
        }
    }

}

void RoomDialog::setGameStart()
{
    //开始准备都不好使
    ui->pb_player1_ready->setEnabled( false );
    ui->pb_player2_ready->setEnabled( false );
    ui->pb_start->setEnabled( false );
    //五子棋开始操作
    ui->wdg_play->slot_startGame();
}

void RoomDialog::on_pb_player1_ready_clicked(bool checked)
{
    //点击准备按钮
    //验证是不是自己点击
    if( m_status != _host ){
        QMessageBox::warning( this, "warning", "only host can ready!" );
        // 恢复 checkbox 状态（因为 click 已切换）
        ui->pb_player1_ready->setChecked(!checked);
        return;
    }
    //改变按钮状态
    //是 切换状态
    if(ui->pb_player1_ready->isChecked()){
        ui->pb_player1_ready->setText( "已准备" );
        //发送准备信号
        Q_EMIT SIG_gameReady( 0x10, m_roomid, m_roomUserList.front() );
    }
    else{
        ui->pb_player1_ready->setText( "待准备" );
    }
}


void RoomDialog::on_pb_player2_ready_clicked(bool checked)
{
    //点击准备按钮
    //验证是不是自己点击
    if( m_status != _player ){
        QMessageBox::warning( this, "warning", "only player can ready!" );
        // 恢复 checkbox 状态
        ui->pb_player2_ready->setChecked(!checked);
        return;
    }
    //改变按钮状态
    //是 切换状态
    if(ui->pb_player2_ready->isChecked()){
        ui->pb_player2_ready->setText( "已准备" );
        //发送准备信号
        Q_EMIT SIG_gameReady( 0x10, m_roomid, m_roomUserList.back() );
    }
    else{
        ui->pb_player2_ready->setText( "待准备" );
    }
}


void RoomDialog::on_pb_start_clicked()
{
    if( m_status != _host ){
        QMessageBox::warning( this, "warning", "only host can start game!" );
        return;
    }
    //发送开始信号
    Q_EMIT SIG_gameStart( 0x10, m_roomid );
}

void RoomDialog::slot_pieceDown(int blackorwhite, int x, int y)
{
    ui->wdg_play->slot_pieceDown( blackorwhite, x, y );
}

