#ifndef FIVEINLINE_H
#define FIVEINLINE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class FiveInLine;
}
QT_END_NAMESPACE

class FiveInLine : public QWidget
{
    Q_OBJECT

public:
    FiveInLine(QWidget *parent = nullptr);
    ~FiveInLine();

private:
    Ui::FiveInLine *ui;
};
#endif // FIVEINLINE_H
