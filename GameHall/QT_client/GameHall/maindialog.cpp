#include "maindialog.h"
#include "ui_maindialog.h"

#include "QMessageBox"

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainDialog)
{
    ui->setupUi(this);
}

MainDialog::~MainDialog()
{
    delete ui;
}

void MainDialog::closeEvent(QCloseEvent *e)
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

//main界面点击五子棋转到五子棋专区
void MainDialog::on_pb_fiveinLine_clicked()
{
    //跳转

    //发信号

}

