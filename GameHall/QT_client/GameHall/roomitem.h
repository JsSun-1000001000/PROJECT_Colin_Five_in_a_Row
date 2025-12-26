#ifndef ROOMITEM_H
#define ROOMITEM_H

#include <QDialog>

namespace Ui {
class RoomItem;
}

class RoomItem : public QDialog
{
    Q_OBJECT

public:
    explicit RoomItem(QWidget *parent = nullptr);
    ~RoomItem();

    /*
     * time 2025.12.26
     * setInfo
     */
    void setInfo( int roomid );
signals:
    void SIG_JoinRoom(int roomid);

private slots:
    void on_pb_join_clicked();

private:
    Ui::RoomItem *ui;
    int m_roomid;

};

#endif // ROOMITEM_H
