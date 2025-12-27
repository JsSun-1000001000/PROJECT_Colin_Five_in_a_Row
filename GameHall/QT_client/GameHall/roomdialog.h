#ifndef ROOMDIALOG_H
#define ROOMDIALOG_H

#include <QDialog>

namespace Ui {
class RoomDialog;
}

class RoomDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RoomDialog(QWidget *parent = nullptr);
    ~RoomDialog();

    void setInfo( int roomid );
private:
    Ui::RoomDialog *ui;

    int m_roomid;
};

#endif // ROOMDIALOG_H
