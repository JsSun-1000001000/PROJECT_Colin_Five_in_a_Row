#ifndef MYWORKER_H
#define MYWORKER_H

#include <QObject>
#include <QThread>

class FiveInLine;

class MyWorker : public QObject
{
    Q_OBJECT
signals:
    void sig_getScoreFinish( int x, int y, int score );
public:
    MyWorker( FiveInLine * _fil );
    ~MyWorker();

private:
    FiveInLine * m_fil;//因为要用五子棋类的方法，所以要加成员

    QThread * m_thread;
public slots:
    void slot_getBetterScore( int x, int y, int player );
};

#endif // MYWORKER_H

///需要实现的方法和信号
///方法有 主线程发送信号 处理 什么点
