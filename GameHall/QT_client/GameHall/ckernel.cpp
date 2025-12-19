#include "ckernel.h"
#include "QDebug"
#include "TcpClientMediator.h"

//宏定义封装 映射偏移
#define NetPackMap( a ) m_netPackFunMap[ (a) - _DEF_PACK_BASE ]

//协议对应关系
void CKernel::setNetPackFunMap()
{
    //太长了 使用宏定义进行封装
    //m_netPackFunMap[_DEF_PACK_LOGIN_RS - _DEF_PACK_BASE ] = &CKernel::slot_dealloginRs;

    NetPackMap(_DEF_PACK_LOGIN_RS) = &CKernel::slot_dealloginRs;

}

void CKernel::DestroyInstance()
{
    qDebug()<<__func__;
    delete m_mainDialog;
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

}

CKernel::CKernel(QObject *parent)
    : QObject{parent}, m_netPackFunMap(_DEF_PACK_COUNT, 0)
{
    setNetPackFunMap();

    m_mainDialog = new MainDialog;

    connect( m_mainDialog, SIGNAL( SIG_close() ),
            this, SLOT(DestroyInstance()) );
    //p.s.如果析构函数里也写了destroyinstance函数 先调用析构的然后再走到connect这里

    m_mainDialog->show();

    m_client = new TcpClientMediator;
    m_client->OpenNet( _DEF_SERVER_IP, _DEF_TCP_PORT );

    connect(m_client, SIGNAL(SIG_ReadyData(uint,char*,int))
            , this, SLOT(slot_ReadyData(uint,char*,int)));
}
