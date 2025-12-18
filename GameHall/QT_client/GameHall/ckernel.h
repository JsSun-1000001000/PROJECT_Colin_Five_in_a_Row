#ifndef CKERNEL_H
#define CKERNEL_H

#include <QObject>
#include "maindialog.h"


//单例 最简单 静态的
class CKernel : public QObject
{
    Q_OBJECT
public:
    static CKernel * GetInstance(){
        static CKernel kernel;
        return &kernel;
    }

public slots:
    void DestroyInstance();

signals:

private:
    explicit CKernel(QObject *parent = nullptr);
    ~CKernel(){}
    CKernel(const CKernel& kernel){};
    CKernel& operator = (const CKernel& kernel){
        return *this;
    }
    //成员属性 网络 ui类对象
    MainDialog * m_mainDialog;

};

#endif // CKERNEL_H
