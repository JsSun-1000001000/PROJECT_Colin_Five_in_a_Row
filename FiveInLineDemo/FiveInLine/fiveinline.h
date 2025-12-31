#ifndef FIVEINLINE_H
#define FIVEINLINE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class FiveInLine;
}
QT_END_NAMESPACE

//界面580*580
//外边距
#define FIL_MARGIN_HEIGHT   (50)
#define FIL_MARGIN_WIDTH    (50)
//行列数 规范 15*15
#define FIL_COLS            (15)
#define FIL_ROWS            (15)
//边和边之间的边距
#define FIL_SPACE           (30)
//棋子大小
#define FIL_PIECE_SIZE      (28)
//棋盘边缘缩进的距离
#define FIL_DISTANCE        (10)

//1.首先绘制棋盘 网格线 棋子
//2.鼠标点击，出现棋子，然后鼠标移动，棋子跟着移动，鼠标释放，棋子落子
//3.需要涉及到鼠标点击，鼠标移动，鼠标释放事件
//4.定时器 定时刷新界面 一秒60帧
//5.落子 出界 要放弃 合法的 发送信号
//落子的槽函数 对更新数组 切换回合
//6.判断输赢

//根据方向对坐标的偏移 每次是一个单位
//1.根据当前点 一次查看左右 上下 左上右下 左下右上 四个方向棋子个数
//2.初始棋子个数为一 然后根据方向换算新的坐标 查看是否出界，出界break
//不然看是否和当前棋子同色 如果count到5 结束 没有到5 继续看其他直线

#include <vector>
#include <QPaintEvent>
#include <QTimer>
/*---------------------赢麻了ai----------------------*/
//time 2025.12.31

#include <vector>
using namespace std;
struct stru_win{
    stru_win():board( FIL_COLS, vector<int>(FIL_ROWS, 0) ),
        playerCount(0),cpuCount(0){

    }
    void clear(){
        playerCount = 0;
        cpuCount = 0;
    }
    vector< vector<int> > board;//胜利的棋子布局
    int playerCount;//该赢法玩家的棋子个数
    int cpuCount;//该赢法电脑的棋子个数
};

/*--------------------------------------------------*/

class FiveInLine : public QWidget
{
    Q_OBJECT

signals://用信号记录落子
    //落子信号
    void SIG_pieceDown( int blackorwhite, int x, int y );

public:
    FiveInLine(QWidget *parent = nullptr);
    ~FiveInLine();

    //重写绘图事件 绘图背景 棋盘 棋子
    void paintEvent(QPaintEvent *event);
    /*------------------------------------------*/
    //鼠标点击 出现棋子
    void mousePressEvent(QMouseEvent *event);
    //鼠标移动 棋子跟着移动
    void mouseMoveEvent(QMouseEvent *event);
    //鼠标释放 棋子落子
    void mouseReleaseEvent(QMouseEvent *event);
    //获取当前是黑白回合
    int getblackOrWhite() const;
    //切换黑白回合
    void changeBlackAndWhite();
    //判断是否出界
    bool isCrossLine( int x, int y );
    //判断输赢
    bool isWin( int x, int y );
    //清空
    void clear();
    /*-----------------------------------------------------------*/
    /*
     * time 2025.12.31
     */
    //初始化所有赢法——赢学
    void InitAiVector();
    //电脑落子
    void pieceDownByCpu();
    /*-----------------------------------------------------------*/

    void setCpuColor(int newCpuColor);

public slots:
    void slot_pieceDown( int blackorwhite, int x, int y );
    void slot_startGame();

private:
    Ui::FiveInLine *ui;
    //记录当前是黑子还是白子回合
    int m_blackOrWhite;
    //鼠标移动中
    QPoint m_movePoint;
    //移动标志
    bool m_moveFlag;
    //黑白子枚举
    enum ENUM_BLACK_OR_WHITEP{ None = 0, Black = 1, White = 2 };
    //方向枚举 判断输赢
    enum enum_direction{ d_z, d_y, d_s, d_x, d_zs, d_yx, d_zx, d_ys, d_count };

    //成员属性 棋盘 二维数组
    std::vector< std::vector<int> > m_board;
    //棋子的颜色数组
    QBrush m_pieceColor[3];
    //定时器
    QTimer m_timer;
    //判断结束标志
    bool m_isOver;
    //根据方向对坐标的偏移 每次是一个单位
    int dx[ d_count ] = {-1, 1, 0, 0, -1, 1, -1, 1 };
    int dy[ d_count ] = { 0, 0, -1, 1, -1, 1, 1, -1 };
    /*-----------------------------------------------------------*/
    /*
     * time 2025.12.31
     * 赢麻了ai
     */
    //赢法数组
    vector<stru_win> m_vecWin;
    //电脑回合
    int m_cpuColor;
    /*-----------------------------------------------------------*/
};
#endif // FIVEINLINE_H
