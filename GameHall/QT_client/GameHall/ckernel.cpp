#include "ckernel.h"
#include "QDebug"
#include "TcpClientMediator.h"

void CKernel::DestroyInstance()
{
    qDebug()<<__func__;
    delete m_mainDialog;
}

//接收处理
void CKernel::slot_ReadyData(unsigned int lSendIP, char *buf, int nlen)
{
    //要记得回收buf
    delete[] buf;
}

CKernel::CKernel(QObject *parent)
    : QObject{parent}
{
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
