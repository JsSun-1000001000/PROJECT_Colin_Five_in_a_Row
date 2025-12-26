#include "ckernel.h"
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

void CKernel::SendData(char *buf, int nlen)
{
    m_client->SendData( 0, buf, nlen );
}

CKernel::CKernel(QObject *parent)
    : QObject{parent}, m_netPackFunMap(_DEF_PACK_COUNT, 0)
    ,m_id(0), m_roomid(0), m_zoneid(0)
{
    ConfigSet();
    setNetPackFunMap();
/*-----------------------------------------------------------------------------*/
    m_mainDialog = new MainDialog;

    connect( m_mainDialog, SIGNAL( SIG_close() ),
            this, SLOT(DestroyInstance()) );
    //p.s.如果析构函数里也写了destroyinstance函数 先调用析构的然后再走到connect这里

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
    m_fiveInLineZone->show();
    m_roomDialog = new RoomDialog;
    m_roomDialog->show();

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
