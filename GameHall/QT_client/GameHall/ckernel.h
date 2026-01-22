#ifndef CKERNEL_H
#define CKERNEL_H

#include <QObject>
#include "INetMediator.h"
#include "packdef.h"
#include <vector>

#include "maindialog.h"
#include "logindialog.h"
#include "fiveinlinezone.h"
#include "roomdialog.h"
#include <QTimer>

class CKernel;
//成员函数指针类型
typedef void (CKernel::*PFUN)( unsigned int lSendIP , char* buf , int nlen );

//单例 最简单 静态的
class CKernel : public QObject
{
    Q_OBJECT
public:
    static CKernel * GetInstance(){
        static CKernel kernel;
        return &kernel;
    }

public slots:
    void DestroyInstance();
    void SendData( char * buf, int nlen );
    /*
     * 窗口处理
     * 2025.12.24: add slot_logincommit and slotregistercommit
     * 2025.12.26: add slot_joinZone
     */
    void slot_loginCommit(QString tel, QString password);
    void slot_registerCommit(QString tel, QString password, QString name);
    void slot_leaveZone();
    void slot_joinZone(int zoneid);
    /*
     * time 2025.12.27
     * slot leave room
     */
    void slot_leaveRoom();
    /*
     * time 2025.12.27
     * definite slot_joinRoom(int)
     */
    void slot_joinRoom(int roomid);

    //获取专区每个房间玩家数
    void slot_roomInfoInZone();

    /*
     * time 2025.12.30
     * 准备和开局
     */
    void slot_fil_gameReady(int zoneid, int roomid, int userid);
    void slot_fil_gameStart(int zoneid, int roomid);
    //落子
    void slot_fil_pieceDown( int blackorwhite, int x, int y);
    //输赢
    void slot_fil_win( int blackorwhite );
    /*------------------托管 人机---------------------------------*/

    void slot_fil_playByCpuBegin(int zoneid, int roomid, int userid);

    void slot_fil_playByCpuEnd(int zoneid, int roomid, int userid);

    /*------------------------------------------------------------*/

    void slot_ReadyData( unsigned int lSendIP , char* buf , int nlen );
    //网络处理
    void slot_dealloginRs( unsigned int lSendIP , char* buf , int nlen );
    void slot_dealregisterRs( unsigned int lSendIP , char* buf , int nlen );
    void slot_dealJoinRoomRs( unsigned int lSendIP , char* buf , int nlen );
    void slot_dealRoomMemberRq( unsigned int lSendIP , char* buf , int nlen );
    void slot_dealLeaveRoomRq(unsigned int lSendIP, char *buf, int nlen);

    void slot_dealFilGameReadyRq(unsigned int lSendIP, char *buf, int nlen);
    void slot_dealFilGameStartRq(unsigned int lSendIP, char *buf, int nlen);
    void slot_dealFilPieceDownRq(unsigned int lSendIP, char *buf, int nlen);
    void slot_dealFilWinRq(unsigned int lSendIP, char *buf, int nlen);
    void slot_dealZoneRoomInfo(unsigned int lSendIP, char *buf, int nlen);

    /*------------------网络处理——托管处理--------------------------------*/
    void slot_dealFilPlayByCpuBegin(unsigned int lSendIP, char *buf, int nlen);
    void slot_dealFilPlayByCpuEnd(unsigned int lSendIP, char *buf, int nlen);
    /*-------------------------------------------------------------------*/

private:
    void setNetPackFunMap();
    void ConfigSet();
    explicit CKernel(QObject *parent = nullptr);
    ~CKernel(){  }
    CKernel(const CKernel& kernel){};
    CKernel& operator = (const CKernel& kernel){
        return *this;
    }
    //成员属性 网络 ui类对象
    MainDialog * m_mainDialog;
    LoginDialog * m_loginDialog;
    FiveInLineZone * m_fiveInLineZone;
    RoomDialog * m_roomDialog;

    INetMediator * m_client;

    //协议映射表 协议头与处理函数的处理关系
    std::vector<PFUN> m_netPackFunMap;

    //个人信息
    int m_id;
    int m_roomid;
    int m_zoneid;
    bool m_isHost;
    QString m_userName;
    char m_serverIP[20];
    //单位时间内获取专区内房间内玩家个数
    QTimer m_rqTimer;
};

#endif // CKERNEL_H
