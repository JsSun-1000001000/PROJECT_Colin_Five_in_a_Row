#include "myworker.h"
#include "fiveinline.h"

MyWorker::MyWorker( FiveInLine * _fil ) {

    m_fil = _fil;

    m_thread = new QThread;
    m_thread->start();

    this->moveToThread(m_thread);
}

MyWorker::~MyWorker()
{
    m_thread->quit();
    m_thread->wait();
    if(m_thread->isRunning()){
        m_thread->terminate();
        m_thread->wait();
    }
    m_thread->deleteLater();
}

//
void MyWorker::slot_getBetterScore(int x, int y, int player)
{
    //遍历所有可能位置

    ///很多不认识的参数需要重新定义
    //上来先是max层
    int bestValue = INT_MIN;
    int bestX = -1;
    int bestY = -1;

    vector<pair<int, int> > copyEveryStep = m_fil->m_everyStepPos;//避免出现问题 使用拷贝的
    vector<vector<int> > copyBoard = m_fil->m_board;//同理棋盘也是

    //这个位置之所以能下子 因为是空位置
    //尝试在该位置下子 如果直接能获得胜利 那么就返回
    copyBoard[x][y] = player;
    if(m_fil->isWin(x,y,copyBoard)){
        bestValue = 200000;
        bestX = x;
        bestY = y;
        emit sig_getScoreFinish(bestX,bestY,bestValue);
        return;
    }
    //尝试在该位置 敌人下子 如果能获得胜利 那么也返回
    copyBoard[x][y] = (player == FiveInLine::Black)?FiveInLine::White:FiveInLine::Black;
    if(m_fil->isWin(x, y, copyBoard)){
        bestValue = 100000;
        bestX = x;
        bestY = y;
        emit sig_getScoreFinish(bestX,bestY,bestValue);
        return;
    }
    //切换回来 开始 搜索
    copyBoard[x][y] = player;
    copyEveryStep.push_back({x,y});
    //使用极大值极小值搜索配合α-β剪枝 先看min层
    int moveValue = m_fil->minmax( copyBoard,
                           copyEveryStep,
                           m_fil->MAX_DEPTH - 1,
                           INT_MIN,
                           INT_MAX,
                           false,
                           player);
    //dfs 撤销该点落子
    copyBoard[x][y] = FiveInLine::None;
    copyEveryStep.pop_back();

    if(moveValue > bestValue){
        bestValue = moveValue;
        bestX = x;
        bestY = y;
    }
    emit sig_getScoreFinish(bestX,bestY,bestValue);
}
