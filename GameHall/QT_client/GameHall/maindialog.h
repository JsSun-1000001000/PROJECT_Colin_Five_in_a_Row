#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainDialog;
}
QT_END_NAMESPACE

class MainDialog : public QDialog
{
    Q_OBJECT

signals:
    void SIG_close();

public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();

    void closeEvent( QCloseEvent * e );

private slots:
    void on_pb_fiveinLine_clicked();

private:
    Ui::MainDialog *ui;
};
#endif // MAINDIALOG_H
