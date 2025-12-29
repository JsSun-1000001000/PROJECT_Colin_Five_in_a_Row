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
    void clearRoom();
    void playerLeave(int id);

    void closeEvent(QCloseEvent * event);

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
