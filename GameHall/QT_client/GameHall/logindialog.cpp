#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QCloseEvent>

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
    //默认登录界面
    ui->tw_page->setCurrentIndex(0);

}
//清空
void LoginDialog::on_pb_clear_clicked()
{
    //清空
    ui->le_tel->setText("");
    ui->le_password->setText("");
}

//登录按钮
void LoginDialog::on_pb_login_clicked()
{
    //登录信息的过滤
    QString tel = ui->le_tel->text();
    QString password = ui->le_password->text();
    //每一个不能为空 QString isEmpty()->""
    if(tel.isEmpty()||password.isEmpty()){
        QMessageBox::about( this, "提示", "内容不能为空哦" );
        return;
    }
    //手机号要合法 正则表达式 1 3-8 0-9 九位 ^开头$结尾
    //题外话：为什么不做手机验证码？
    //创建正则对象 注意qt6不支持QRegExp了 要么去包含qt5 要么去用qt6
    //以下为qt6对于正则表达式判断电话号的方法
    QRegularExpression exp("^[1][3,4,5,6,7,8][0-9]{9}$");//传参
    QRegularExpressionMatch match = exp.match( tel );
    if( !match.hasMatch() ){
        QMessageBox::about(this,"Oops","illegal telephone number");
        return;
    }
    //密码不能过长
    if( password.length()>20 ){
        QMessageBox::about(this,"Oops","password too long, less than 20 pls");
        return;
    }
    //验证密码
    //发送信号
    Q_EMIT SIG_loginCommit( tel, password );
}
//注册清空
void LoginDialog::on_pb_clear_register_clicked()
{
    //清空
    ui->le_tel_register->setText("");
    ui->le_password_register->setText("");
    ui->le_confirm_register->setText("");
    ui->le_name_register->setText("");
}
//注册按钮
void LoginDialog::on_pb_register_clicked()
{
    //注册信息的过滤
    QString name = ui->le_name_register->text();
    QString tel = ui->le_tel_register->text();
    QString password = ui->le_password_register->text();
    QString confirm = ui->le_confirm_register->text();
    //每一个不能为空 QString isEmpty()->""
    if(name.isEmpty()||tel.isEmpty()||password.isEmpty()||confirm.isEmpty()){
        QMessageBox::about( this, "提示", "内容不能为空哦" );
        return;
    }
    //手机号要合法 正则表达式 1 3-8 0-9 九位 ^开头$结尾
    //题外话：为什么不做手机验证码？
    //创建正则对象 注意qt6不支持QRegExp了 要么去包含qt5 要么去用qt6
    //以下为qt6对于正则表达式判断电话号的方法
    QRegularExpression exp("^[1][3,4,5,6,7,8][0-9]{9}$");//传参
    QRegularExpressionMatch match = exp.match( tel );
    if( !match.hasMatch() ){
        QMessageBox::about(this,"Oops","illegal telephone number");
        return;
    }
    //密码不能过长
    if( password.length()>20 ){
        QMessageBox::about(this,"Oops","password too long, less than 20 pls");
        return;
    }
    //昵称不能全空格 不能过长
    //题外话：为什么不添加一个显示 输入字符/总长 的控件，过长了会暂停输入并且控件变红？
    QString tmpName= name;
    tmpName.remove(' ');//remove operation can change the original string
    if( name.length()>20 ){
        QMessageBox::about(this,"Oops","name is null or too long, less than 20 pls");
        return;
    }
    //twice input password should be same
    if( password != confirm ){
        QMessageBox::about( this, "Oops", "the password and the confirm password are not the same");
        return;
    }
    //发送信号
    Q_EMIT SIG_registerCommit( tel, password, name );
    //
}

void LoginDialog::closeEvent(QCloseEvent *e)
{
    if( QMessageBox::question(this, "退出", "是否退出")
        == QMessageBox::Yes ){
        //发信号
        Q_EMIT SIG_close();
        //同意关闭事件
        e->accept();
    }
    else{
        //忽略关闭事件
        e->ignore();
    }
}

