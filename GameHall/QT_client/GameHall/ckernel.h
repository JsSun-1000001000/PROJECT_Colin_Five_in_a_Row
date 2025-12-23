#ifndef CKERNEL_H
#define CKERNEL_H

#include <QObject>
#include "INetMediator.h"
#include "packdef.h"
#include <vector>

#include "maindialog.h"
#include "logindialog.h"

//成员函数指针类型
class CKernel;
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
    void slot_ReadyData( unsigned int lSendIP , char* buf , int nlen );

    //网络处理
    void slot_dealloginRs( unsigned int lSendIP , char* buf , int nlen );

signals:

private:
    void setNetPackFunMap();
    explicit CKernel(QObject *parent = nullptr);
    ~CKernel(){  }
    CKernel(const CKernel& kernel){};
    CKernel& operator = (const CKernel& kernel){
        return *this;
    }
    //成员属性 网络 ui类对象
    MainDialog * m_mainDialog;
    LoginDialog * m_loginDialog;
    INetMediator * m_client;

    //协议映射表 协议头与处理函数的处理关系
    std::vector<PFUN> m_netPackFunMap;
};

#endif // CKERNEL_H
