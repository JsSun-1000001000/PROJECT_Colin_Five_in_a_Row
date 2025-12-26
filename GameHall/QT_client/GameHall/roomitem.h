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

private:
    Ui::RoomItem *ui;
};

#endif // ROOMITEM_H
