#ifndef DEMODIALOG_H
#define DEMODIALOG_H

#include <QDialog>

#include "INetMediator.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class DemoDialog;
}
QT_END_NAMESPACE

class DemoDialog : public QDialog
{
    Q_OBJECT

public:
    DemoDialog(QWidget *parent = nullptr);
    ~DemoDialog();

//槽函数
public slots:
    void slot_clientReadyData( unsigned int lSendIP , char* buf , int nlen );
    void slot_serverReadyData( unsigned int lSendIP , char* buf , int nlen );


private slots:
    void on_pb_commit_clicked();

private:
    Ui::DemoDialog *ui;

    //client
    INetMediator * m_client;
    //server
    INetMediator * m_server;


};
#endif // DEMODIALOG_H
