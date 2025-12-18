#include "maindialog.h"

#include <QApplication>
#include "ckernel.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainDialog w;
    //w.show();

    //创建核心类
    CKernel::GetInstance();
    //qt机制：当关闭窗体时，当他没有可以实现的窗体时，程序就会自动结束

    return a.exec();
}
