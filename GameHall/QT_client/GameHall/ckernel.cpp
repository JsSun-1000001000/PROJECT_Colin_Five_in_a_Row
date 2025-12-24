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
    //加密——挖坑

    //封包
    STRU_LOGIN_RQ rq;
    strcpy( rq.tel, tel.toStdString().c_str() );
    strcpy( rq.password, password.toStdString().c_str() );
    //发送
    SendData( (char * )&rq, sizeof(rq) );
}

void CKernel::slot_registerCommit(QString tel, QString password, QString name)
{
    //加密——挖坑

    //封包
    STRU_REGISTER_RQ rq;
    strcpy( rq.tel, tel.toStdString().c_str() );
    strcpy( rq.password, password.toStdString().c_str() );
    //兼容中文
    std::string strName = name.toStdString();
    strcpy( rq.name, strName.c_str() );
    //发送
    SendData( (char *)&rq, sizeof(rq) );
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
}

void CKernel::SendData(char *buf, int nlen)
{
    m_client->SendData( 0, buf, nlen );
}

CKernel::CKernel(QObject *parent)
    : QObject{parent}, m_netPackFunMap(_DEF_PACK_COUNT, 0)
{
    setNetPackFunMap();

    m_mainDialog = new MainDialog;

    connect( m_mainDialog, SIGNAL( SIG_close() ),
            this, SLOT(DestroyInstance()) );
    //p.s.如果析构函数里也写了destroyinstance函数 先调用析构的然后再走到connect这里

    //m_mainDialog->show();

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


    m_client = new TcpClientMediator;
    //m_client->OpenNet( _DEF_SERVER_IP, _DEF_TCP_PORT );

    connect(m_client, SIGNAL(SIG_ReadyData(uint,char*,int))
            , this, SLOT(slot_ReadyData(uint,char*,int)));

    //模拟连接服务器 发送数据包
    //STRU_LOGIN_RQ rq;//这个位置用的请求不能定义字符串，不可以，因为string用的堆区空间
    //这个senddata相当于copy，拷贝连续空间，给的首地址，拷贝sizeof这么多，
    //故，这个区域一定是连续的，不能定义string，qstring这种
    //m_client->SendData(0,(char*)&rq,sizeof(rq));



}
