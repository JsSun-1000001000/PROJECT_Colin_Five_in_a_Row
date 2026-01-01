#ifndef ROOMDIALOG_H
#define ROOMDIALOG_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class RoomDialog;
}

class RoomDialog : public QDialog
{
    Q_OBJECT
signals:
    void SIG_close();
    //准备和开局
    void SIG_gameReady(int zoneid, int roomid, int userid);
    void SIG_gameStart(int zoneid, int roomid);
    //落子信号
    void SIG_pieceDown( int blackorwhite, int x, int y);
    void SIG_playerWin( int blackorwhite );

    /*-----------------赢麻了-----------------------*/
    //time 2026.1.1
    void SIG_playByCpuBegin(int zoneid, int roomid, int userid);
    void SIG_playByCpuEnd(int zoneid, int roomid, int userid);

    /*-----------------赢麻了-----------------------*/

public:
    explicit RoomDialog(QWidget *parent = nullptr);
    ~RoomDialog();

    void setInfo( int roomid );
    /*
     * time 2025.12.28
     * sethostinfo setplayerinfo
     */
    void setUserStatus( int status );
    void setHostInfo( int id, QString name );
    void setPlayerInfo( int id, QString name);
    /*------------------托管--赢麻了----------------------*/
    //time 2026.1.1
    //用于kernel去调用
    void setHostPlayByCpu( bool yes );
    void setPlayerPlayByCpu( bool yes );

    /*---------------------------------------------------*/
    void clearRoom();
    void resetAllPushButton(  );
    void playerLeave(int id);

    void closeEvent(QCloseEvent * event);

    //ckernel调用
    void setPlayerReady( int id );
    //ckernel
    void setGameStart();

private slots:
    void on_pb_player1_ready_clicked(bool checked);

    void on_pb_player2_ready_clicked(bool checked);

    void on_pb_start_clicked();

    void on_pb_player1_cpu_clicked(bool checked);

    void on_pb_player2_cpu_clicked(bool checked);

public slots:
    void slot_pieceDown( int blackorwhite, int x, int y );

private:
    Ui::RoomDialog *ui;

    int m_roomid;
    /*
     * time 2025.12.28
     * list<int> m_roomUserList
     */
    std::list<int> m_roomUserList;
    //状态
    int m_status;
};

#endif // ROOMDIALOG_H
