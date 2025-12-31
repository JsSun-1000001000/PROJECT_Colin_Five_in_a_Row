#include "ckernel.h"
#include "FiveInLine/fiveinline.h"
#include "QDebug"
#include "TcpClientMediator.h"
#include <QMessageBox>
#include "md5.h"

#include <QSettings>
#include <QCoreApplication>
#include <QFileInfo>

//获得md5函数
//1_1234
//EA135E06CD37AB7E304E1DC440C93EA2
//结果：ea135e06cd37ab7e304e1dc440c93ea2
//验证
#define MD5_KEY 1234
static std::string getMD5(QString val){
    QString tmp = QString("%1_%2").arg(val).arg(MD5_KEY);
    MD5 md( tmp.toStdString() );
    return md.toString();
}



//宏定义封装 映射偏移
#define NetPackMap( a ) m_netPackFunMap[ (a) - _DEF_PACK_BASE ]

//协议对应关系
void CKernel::setNetPackFunMap()
{
    //太长了 使用宏定义进行封装
    //m_netPackFunMap[_DEF_PACK_LOGIN_RS - _DEF_PACK_BASE ] = &CKernel::slot_dealloginRs;

    NetPackMap(_DEF_PACK_LOGIN_RS) = &CKernel::slot_dealloginRs;
    NetPackMap(_DEF_PACK_REGISTER_RS) = &CKernel::slot_dealregisterRs;
    /*
     * time 2025.12.27
     *
     */
    NetPackMap(DEF_JOIN_ROOM_RS) = &CKernel::slot_dealJoinRoomRs;
    /*
     * time 2025.12.28
     * room member
     */
    NetPackMap(DEF_ROOM_MEMBER) = &CKernel::slot_dealRoomMemberRq;
    /*
     * time 2025.12.29
     *
     */
    NetPackMap(DEF_LEAVE_ROOM_RQ) = &CKernel::slot_dealLeaveRoomRq;
    /*
     * time 2025.12.30
     * game
     */
    NetPackMap(DEF_FIL_ROOM_READY  ) = &CKernel::slot_dealFilGameReadyRq;
    NetPackMap(DEF_FIL_GAME_START  ) = &CKernel::slot_dealFilGameStartRq;
    //NetPackMap(DEF_FIL_AI_BEGIN    ) = &CKernel::slot_deal
    //NetPackMap(DEF_FIL_AI_END      ) = &CKernel::slot_deal
    //NetPackMap(DEF_FIL_DISCARD_THIS) = &CKernel::slot_deal
    //NetPackMap(DEF_FIL_SURREND     ) = &CKernel::slot_deal
    NetPackMap(DEF_FIL_PIECEDOWN   ) = &CKernel::slot_dealFilPieceDownRq;
    NetPackMap(DEF_FIL_WIN         ) = &CKernel::slot_dealFilWinRq;


}

void CKernel::ConfigSet()
{
    //获取配置文件的信息以及设置
    //.ini配置文件
    //[net]组名 groupname
    //key = value

    //ip默认
    strcpy( m_serverIP, _DEF_SERVER_IP );
    //设置和获取配置文件 有还是没有 配置文件在哪里？设置和exe同一级的目录
    QString path = QCoreApplication::applicationDirPath() + "/config.ini";
    QFileInfo info(path);
    if( info.exists() ){
        //存在
        QSettings setting( path, QSettings::IniFormat, nullptr );
        //[net]写入组
        setting.beginGroup("net");
        QVariant var = setting.value( "ip" );
        QString strip = var.toString();
        if( !strip.isEmpty() ){
            strcpy( m_serverIP, strip.toStdString().c_str() );
        }
        setting.endGroup();
    }
    else{
        //不存在
        QSettings setting( path, QSettings::IniFormat, nullptr );
        //[net]写入组
        setting.beginGroup("net");
        setting.setValue( "ip", QString::fromStdString(m_serverIP) );
        setting.endGroup();
    }
    qDebug()<< "ip: " << m_serverIP;
}

void CKernel::DestroyInstance()
{
    qDebug()<<__func__;
    if(m_client){
        m_client->CloseNet();
        delete m_client;
        m_client = nullptr;
    }
    delete m_mainDialog;
    delete m_loginDialog;
}

void CKernel::slot_loginCommit(QString tel, QString password)
{
    //加密——挖坑——完成
    //封包
    STRU_LOGIN_RQ rq;
    strcpy( rq.tel, tel.toStdString().c_str() );

    //qDebug()<<password<<"MD5"<<getMD5( password ).c_str();

    strcpy( rq.password, getMD5( password ).c_str() );
    //发送
    SendData( (char * )&rq, sizeof(rq) );
}

void CKernel::slot_registerCommit(QString tel, QString password, QString name)
{
    //加密——挖坑——完成
    //封包
    STRU_REGISTER_RQ rq;
    strcpy( rq.tel, tel.toStdString().c_str() );
    strcpy( rq.password, getMD5( password ).c_str() );
    //兼容中文
    std::string strName = name.toStdString();
    strcpy( rq.name, strName.c_str() );
    //发送
    SendData( (char *)&rq, sizeof(rq) );
}
/*
 * time 2025.12.27
 * refactor slot leave zone
 */
void CKernel::slot_leaveZone()
{
    //成员属性修改
    m_zoneid = 0;
    //请求
    STRU_LEAVE_ZONE rq;
    rq.userid = m_id;
    SendData( (char *)&rq, sizeof(rq) );
    //ui跳转
    m_mainDialog->show();
}
/*
 * author jssun
 * time 2025.12.26: add a definition of slot_joinZone
 */
 //提交加入分区
void CKernel::slot_joinZone(int zoneid)
{
    /*
     * time 2025.12.27
     * member changes the property
     */
    m_zoneid = zoneid;

    //请求包
    STRU_JOIN_ZONE rq;
    rq.userid = m_id;
    rq.zoneid = zoneid;

    SendData( (char *)&rq, sizeof(rq) );
    /*
     * time 2025.12.27
     * show the five in line zone
     */
    switch( zoneid ){
    case Five_in_Line:
        m_fiveInLineZone->show();
        break;
    }
}
/*
 * time 2025.12.29
 * leave room
 */
void CKernel::slot_leaveRoom()
{
    //这个人主动离开
    STRU_LEAVE_ROOM_RQ rq;
    rq.status = m_isHost?_host:_player;
    rq.userid = m_id;
    rq.roomid = m_roomid;
    SendData((char *)&rq, sizeof(rq));
    //界面
    m_roomDialog->clearRoom();
    m_roomDialog->hide();
    m_fiveInLineZone->show();
    //后台数据
    m_roomid = 0;
    m_isHost = false;
}
/*
 * time 2025.12.27
 * refactor slot_joinRoom
 */
//提交加入房间
void CKernel::slot_joinRoom(int roomid)
{
    /*
     * time 2025.12.27
     * judge if the roomid != 0
     */
    if( m_roomid != 0 ){
        QMessageBox::about( nullptr, "Oops", "已经在房间，无法加入");
        return;
    }
    //加入成功后隐藏
    //m_fiveInLineZone->hide();
    //提交请求
    STRU_JOIN_ROOM_RQ rq;
    rq.userid = m_id;
    rq.roomid = roomid;

    SendData( (char *)&rq, sizeof(rq) );
}

//五子棋准备
void CKernel::slot_fil_gameReady(int zoneid, int roomid, int userid)
{

    STRU_FIL_RQ rq(DEF_FIL_ROOM_READY);
    rq.zoneid = zoneid;
    rq.roomid = roomid;
    rq.userid = userid;

    SendData( (char *)&rq, sizeof(rq) );
}
//五子棋开局
void CKernel::slot_fil_gameStart(int zoneid, int roomid)
{
    STRU_FIL_RQ rq(DEF_FIL_GAME_START);
    rq.zoneid = zoneid;
    rq.roomid = roomid;
    rq.userid = m_id;

    SendData( (char *)&rq, sizeof(rq) );
}

void CKernel::slot_fil_pieceDown(int blackorwhite, int x, int y)
{
    //封包
    STRU_FIL_PIECEDOWN rq;
    rq.color = blackorwhite;
    rq.x = x;
    rq.y = y;
    rq.userid = m_id;
    rq.roomid = m_roomid;
    rq.zoneid = m_zoneid;

    SendData( (char *)&rq, sizeof(rq) );
}

void CKernel::slot_fil_win(int blackorwhite)
{
    //弹窗
    QString res;
    if( (m_isHost && blackorwhite == FiveInLine::Black) ||
        (!m_isHost && blackorwhite == FiveInLine::White)){
        res = QString("you win");
    }
    else{
        res = QString("you lose");
    }
    /*if( !m_isHost && blackorwhite == FiveInLine::White ){
        res = QString("you win");
    }
    else{
        res = QString("you lose");
    }*/
    QMessageBox::about( m_roomDialog, "提示", res );
    //可以点准备开局
    m_roomDialog->resetAllPushButton();
    /*// 构造并发出胜利消息（由服务器转发给房间内所有人）
    STRU_FIL_RQ rq(DEF_FIL_WIN);
    rq.zoneid = m_zoneid;
    rq.roomid = m_roomid;
    rq.userid = m_id; // 把胜者的 userid 放入包里

    SendData( (char *)&rq, sizeof(rq) );

    // 不在这里直接弹窗，等待服务器转发后统一由 slot_dealFilWinRq 处理显示与重置*/
}

//接收处理
void CKernel::slot_ReadyData(unsigned int lSendIP, char *buf, int nlen)
{
    //协议映射表
    PackType type = *(int *)buf;    //*(int *) 按照整型取数据
    if(type >= _DEF_PACK_BASE
        && type < _DEF_PACK_BASE + _DEF_PACK_COUNT )
    {
        //根据协议头跳转到对应函数
        //不推荐switch 每此都要增加，修改带着swich
        //协议映射处理
        PFUN pf = NetPackMap( type );
        //执行函数
        (this->*pf)( lSendIP,buf,nlen );

    }
    //要记得回收buf
    delete[] buf;
}

void CKernel::slot_dealloginRs(unsigned int lSendIP, char *buf, int nlen)
{
    qDebug()<<__func__;
    //拆包
    STRU_LOGIN_RS * rs = (STRU_LOGIN_RS *)buf;
    //根据不同结果显示
    switch( rs->result ){
    case user_not_exist:
        QMessageBox::about( m_loginDialog, "Oops", "user not exist login failed");
        break;
    case password_error:
        QMessageBox::about( m_loginDialog, "Oops", "password error login failed");
        break;
    case login_success:
        //ui切换
        m_loginDialog->hide();
        m_mainDialog->show();
        //存储
        m_id = rs->userid;
        m_userName = QString::fromStdString( rs->name );
        break;
    default:
        QMessageBox::about( m_loginDialog, "Oops", "what the fuck did you fucking did?");
        break;
    }
}

void CKernel::slot_dealregisterRs(unsigned int lSendIP, char *buf, int nlen)
{
    qDebug()<<__func__;
    //解析数据包
    STRU_REGISTER_RS * rs = (STRU_REGISTER_RS * )buf;
    //根据结果弹窗
    switch(rs->result){
    case tel_is_exist:
        QMessageBox::about(this->m_loginDialog, "Register Oops", "tel is exist");
        break;
    case name_is_exist:
        QMessageBox::about(this->m_loginDialog, "Register Oops", "name is exist");
        break;
    case register_success:
        QMessageBox::about(this->m_loginDialog, "Register Yeah", "Register SUCCESSFULLY~~");
        break;
    default:
        QMessageBox::about(this->m_loginDialog, "Register Oops", "What the fuck did you fucking did?");
        break;
    }
}
//加入房间回复处理
void CKernel::slot_dealJoinRoomRs(unsigned int lSendIP, char *buf, int nlen)
{
    //拆包
    STRU_JOIN_ROOM_RS * rs = (STRU_JOIN_ROOM_RS *)buf;
    if( rs->result == 0){
        QMessageBox::about( nullptr, "Oops", "加入房间失败" );//专区要具体
        return;
    }
    if( rs->status == _host ){
        m_isHost = true;
    }
    m_roomid = rs->roomid;
    //成功跳转 成员赋值
    m_fiveInLineZone->hide();
    m_roomDialog->setInfo( m_roomid );
    m_roomDialog->show();
    //问题、未来扩展游戏区域不同怎么显示和隐藏
}
/*
 * time 2025.12.28
 * dealRoomMemberRq
 */
void CKernel::slot_dealRoomMemberRq(unsigned int lSendIP, char *buf, int nlen)
{
    //拆包 别人给你发 自己给自己发
    STRU_ROOM_MEMBER *rq = (STRU_ROOM_MEMBER *)buf;
    //设计的时候要加个身份——加身份了
    if( rq->status == _host ){
        m_roomDialog->setHostInfo( rq->userid,
                                  QString::fromStdString(rq->name));
    }
    if( rq->status == _player ){
        //time 2025.12.29 fix bug: change rq->status into rq->userid
        m_roomDialog->setPlayerInfo( rq->userid,
                                  QString::fromStdString(rq->name));
    }

    m_roomDialog->setUserStatus( m_isHost?_host:_player);
}
/*
 * time 2025.12.29
 */
void CKernel::slot_dealLeaveRoomRq(unsigned int lSendIP, char *buf, int nlen){
    //拆包
    STRU_LEAVE_ROOM_RQ * rq = (STRU_LEAVE_ROOM_RQ *)buf;
    if( rq->status == _host ){
        //界面
        m_roomDialog->clearRoom();
        m_roomDialog->hide();
        m_fiveInLineZone->show();
        //后台数据
        m_roomid = 0;
        m_isHost = false;
    }
    else{
        m_roomDialog->playerLeave(rq->userid);
    }

}

void CKernel::slot_dealFilGameReadyRq(unsigned int lSendIP, char *buf, int nlen)
{
    //拆包
    STRU_FIL_RQ * rq = (STRU_FIL_RQ *)buf;
    //什么专区 什么房间 谁 做了什么事
    if( rq->roomid == m_roomid ){
        m_roomDialog->setPlayerReady( rq->userid );
    }
}

void CKernel::slot_dealFilGameStartRq(unsigned int lSendIP, char *buf, int nlen)
{
    //拆包
    STRU_FIL_RQ * rq = (STRU_FIL_RQ *)buf;
    //什么专区 什么房间 谁 做了什么事
    if( rq->roomid == m_roomid ){
        m_roomDialog->setGameStart();
    }
}
//处理落子
void CKernel::slot_dealFilPieceDownRq(unsigned int lSendIP, char *buf, int nlen)
{
    STRU_FIL_PIECEDOWN* rq = (STRU_FIL_PIECEDOWN*)buf;
    m_roomDialog->slot_pieceDown( rq->color, rq->x, rq->y );
}

void CKernel::slot_dealFilWinRq(unsigned int lSendIP, char *buf, int nlen)
{
    /*STRU_FIL_RQ * rq = (STRU_FIL_RQ *)buf;
    //只处理当前房间里的通知
    if(rq->roomid!=m_roomid) return;
    //log
    qDebug() << "RX FIL_WIN userid=" << rq->userid << " localid=" << m_id << " roomid=" << rq->roomid;

    bool iWin = (rq->userid == m_id);
    if(iWin){
        QMessageBox::about( m_roomDialog, "提示", "you win" );
    }
    else{
        QMessageBox::about( m_roomDialog, "提示", "you lose" );
    }
    // 所有人收到胜利消息后将按钮重置或禁用
    m_roomDialog->resetAllPushButton();*/

}


void CKernel::SendData(char *buf, int nlen)
{
    m_client->SendData( 0, buf, nlen );
}

CKernel::CKernel(QObject *parent)
    : QObject{parent}, m_netPackFunMap(_DEF_PACK_COUNT, 0)
    ,m_id(0), m_roomid(0), m_zoneid(0),m_isHost(false)
{
    ConfigSet();
    setNetPackFunMap();
/*-----------------------------------------------------------------------------*/
    m_mainDialog = new MainDialog;

    connect( m_mainDialog, SIGNAL( SIG_close() ),
            this, SLOT(DestroyInstance()) );
    //p.s.如果析构函数里也写了destroyinstance函数 先调用析构的然后再走到connect这里

    /*
     * time: 2025.12.26
     * connect: maindialog five in row push button
     */
    connect( m_mainDialog, SIGNAL( SIG_joinZone(int) ),
            this, SLOT(slot_joinZone(int)) );
    /*
     * time: 2025.12.26
     */
    connect( m_mainDialog, SIGNAL( SIG_pieceDown(int, int, int) ),
            this, SLOT( slot_fil_gameStart(int, int)) );
    //m_mainDialog->show();
/*-----------------------------------------------------------------------------*/
    //show register & login window
    m_loginDialog = new LoginDialog;

    /*
     * time:2025.12.24
     * connect login, register, close
     */
    connect( m_loginDialog, SIGNAL(SIG_close()),
            this, SLOT(DestroyInstance()) );
    connect( m_loginDialog, SIGNAL(SIG_loginCommit( QString, QString )),
            this, SLOT(slot_loginCommit( QString, QString )) );
    connect( m_loginDialog, SIGNAL(SIG_registerCommit( QString, QString, QString )),
            this, SLOT(slot_registerCommit( QString, QString, QString )) );

    m_loginDialog->show();
/*-----------------------------------------------------------------------------*/

    m_fiveInLineZone = new FiveInLineZone;
    //m_fiveInLineZone->show();
    /*
     * time 2025.12.27
     * connect five in line zone when you click the button
     */
    connect( m_fiveInLineZone, SIGNAL(SIG_joinRoom(int))
            , this, SLOT(slot_joinRoom(int)) );
/*----------------leave zone connection----------------------------------------*/
    connect( m_fiveInLineZone, SIGNAL(SIG_close())
            , this, SLOT(slot_leaveZone()) );


    m_roomDialog = new RoomDialog;
    /*
     * time 2025.12.29
     */
    connect(m_roomDialog, SIGNAL(SIG_close()),
            this, SLOT(slot_leaveRoom()));

    /*
     * time 2025.12.30
     * connect ready and start signals
     */
    connect(m_roomDialog, SIGNAL(SIG_gameReady(int,int,int)),
            this, SLOT(slot_fil_gameReady(int,int,int)));
    connect(m_roomDialog, SIGNAL(SIG_gameStart(int,int)),
            this, SLOT(slot_fil_gameStart(int,int)));

    connect(m_roomDialog, SIGNAL( SIG_pieceDown(int,int,int) ),
            this, SLOT(slot_fil_pieceDown(int,int,int)));
    connect(m_roomDialog, SIGNAL( SIG_playerWin(int) ),
            this, SLOT(slot_fil_win(int)));
    //m_roomDialog->show();

/*-----------------------------------------------------------------------------*/
    m_client = new TcpClientMediator;
    m_client->OpenNet( m_serverIP, _DEF_TCP_PORT );

    connect(m_client, SIGNAL(SIG_ReadyData(uint,char*,int))
            , this, SLOT(slot_ReadyData(uint,char*,int)));

    //模拟连接服务器 发送数据包
    //STRU_LOGIN_RQ rq;//这个位置用的请求不能定义字符串，不可以，因为string用的堆区空间
    //这个senddata相当于copy，拷贝连续空间，给的首地址，拷贝sizeof这么多，
    //故，这个区域一定是连续的，不能定义string，qstring这种
    //m_client->SendData(0,(char*)&rq,sizeof(rq));



}
