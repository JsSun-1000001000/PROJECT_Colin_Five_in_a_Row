#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    setUI();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
//设置背景
void LoginDialog::setUI()
{
    this->setWindowTitle( "Register & Login" );
    //加载照片 资源路径，qrc文件下:/根目录
    QPixmap pixmap(":/images/background.jpg");
    //画板添加背景
    QPalette pal( this->palette() );
    //注意 qt512以后对palette进行了更改 改为了window
    //pal.setBrush( QPalette::Background,  );
    pal.setBrush( QPalette::Window, pixmap );
    this->setPalette( pal );
}

void LoginDialog::on_pb_clear_clicked()
{
    //清空
}


void LoginDialog::on_pb_login_clicked()
{
    //注册信息的过滤

    //发送信号

}


void LoginDialog::on_pb_clear_register_clicked()
{
    //清空

}


void LoginDialog::on_pb_register_clicked()
{
    //注册信息的过滤

    //发送信号

}

