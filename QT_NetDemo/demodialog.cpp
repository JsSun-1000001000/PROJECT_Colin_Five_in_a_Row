#include "demodialog.h"
#include "ui_demodialog.h"

#include "QDebug"
#include <QMessageBox>

#include "TcpClientMediator.h"
#include "TcpServerMediator.h"

DemoDialog::DemoDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DemoDialog)
{
    ui->setupUi(this);

    m_server = new TcpServerMediator;
    m_server->OpenNet();

    m_client = new TcpClientMediator;
    m_client->OpenNet( "192.168.61.204" );

    //建立连接
    connect(m_server, SIGNAL(SIG_ReadyData(uint,char*,int))
            , this, SLOT(slot_serverReadyData(uint,char*,int)));

    connect(m_client, SIGNAL(SIG_ReadyData(uint,char*,int))
            , this, SLOT(slot_clientReadyData(uint,char*,int)));
}

DemoDialog::~DemoDialog()
{
    delete ui;
}

//客户端接受信息的处理
void DemoDialog::slot_clientReadyData(unsigned int lSendIP, char *buf, int nlen)
{
    qDebug() << "客户端接收" << buf;
    QString txt = QString::fromStdString( buf );
    QMessageBox::about(this, "客户端接收", QString("来自服务端的信息: %1")
                        .arg(txt) );
}

//服务器接受信息的处理
void DemoDialog::slot_serverReadyData(unsigned int lSendIP, char *buf, int nlen)
{
    qDebug() << "服务器接收" << buf;
    QString txt = QString::fromStdString( buf );
    QMessageBox::about(this, "服务器接收", QString("来自客户端的信息: %1")
                       .arg(txt) );

    //模态窗口 产生后会阻塞 不点ok不返回
    /*
     * question()
     * information() 有声音
     * warning()
     * critical()
     */

    m_server->SendData(lSendIP, buf, nlen);
}

//点击提交的处理
void DemoDialog::on_pb_commit_clicked()
{
    QString name = ui->le_name->text();
    qDebug() << "name: " << name <<"commit";

    //中文支持 怎么去的怎么回来 不会乱码QString默认使用utf-8编码
    std::string strName = name.toStdString();
    //发送
    m_client->SendData(0, (char *)strName.c_str()
                       , strlen(strName.c_str())+1);

}

