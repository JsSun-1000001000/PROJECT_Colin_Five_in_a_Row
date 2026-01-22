/**
 * @file fiveinline.h
 * @brief Five in line game (gomoku) header file
 * @author jssun
 * @date 2026-01-02
 * @version 0.0.1
 */

/**
 * @note
 * 1.首先绘制棋盘 网格线 棋子 \n
 * 2.鼠标点击，出现棋子，然后鼠标移动，棋子跟着移动，鼠标释放，棋子落子\n
 * 3.需要涉及到鼠标点击，鼠标移动，鼠标释放事件\n
 * 4.定时器 定时刷新界面 一秒60帧\n
 * 5.落子 出界 要放弃 合法的 发送信号 落子的槽函数 对更新数组 切换回合\n
 * 6.判断输赢\n
 *
 */
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
/**
 * @brief 定义棋盘外边距高 50 界面为580*580
 */
#define FIL_MARGIN_HEIGHT   (50)
/**
 * @brief 定义棋盘外边距宽 50
 */
#define FIL_MARGIN_WIDTH    (50)
/**
 * @brief 定义棋盘列数 15 规范 15*15
 */
#define FIL_COLS            (15)
/**
 * @brief 定义棋盘行数 15 规范 15*15
 */
#define FIL_ROWS            (15)
//边和边之间的边距
/**
 * @brief 定义棋盘边与边之间的边距 30
 */
#define FIL_SPACE           (30)
//棋子大小
/**
 * @brief 定义棋子大小 28
 */
#define FIL_PIECE_SIZE      (28)
//棋盘边缘缩进的距离
/**
 * @brief 定义棋盘边缘缩进的距离 10
 */
#define FIL_DISTANCE        (10)


#include <vector>
#include <QPaintEvent>
#include <QTimer>

/*---------------------赢麻了ai----------------------*/
//time 2026.1.1 happy new year!!

using namespace std;
/**
 * @struct stru_win
 * @brief 五子棋赢法结构体\n
 * 用于存储胜利的棋子分布
 * @details 所有的赢法都存储在一个容器里，随着玩家和电脑下子，应该不断更新各种赢麻了结构体里的playerCount和cpuCount
 */
struct stru_win{
    stru_win():board( FIL_COLS, vector<int>(FIL_ROWS, 0) ),
        playerCount(0),cpuCount(0){

    }
    void clear(){                   /**< 清空该玩法玩家和电脑的棋子个数*/
        playerCount = 0;
        cpuCount = 0;
    }
    vector< vector<int> > board;    /**< 胜利的棋子布局*/
    int playerCount;                /**< 该赢法玩家的棋子个数，一旦玩家落子该赢法cpu无法取胜*/
    int cpuCount;                   /**< 该赢法电脑的棋子个数，一旦cpu落子该赢法玩家无法取胜*/
};
/**
 * @brief 棋盘每轮的倒计时 31
 */
#define DEFAULT_COUNTER 31

/*--------------------------------------------------*/

class FiveInLine : public QWidget
{
    Q_OBJECT

signals://用信号记录落子
    //落子信号
    /**
     * @brief  落子信号
     *
     * @param[in]  blackorwhite  黑棋还是白棋
     * @param[in]  x  第二个整数
     * @param[in]  y  第二个整数
     *
     */
    void SIG_pieceDown( int blackorwhite, int x, int y );
    /**
     * @brief 在落子槽函数slot_pieceDown中使用
     *
     * @param[in]  blackorwhite
     *
     */
    void SIG_playerWin( int blackorwhite );

public:
    FiveInLine(QWidget *parent = nullptr);
    ~FiveInLine();

    /**
     * @brief 重写绘图事件 绘图背景 棋盘 棋子
     * @param[in] *event 重写绘图事件用的event指针
     */
    void paintEvent(QPaintEvent *event);

    /*------------------------------------------*/
    /**
     * @brief 鼠标点击 出现棋子
     * @param[in] *event 重写绘图事件用的event指针
     */
    void mousePressEvent(QMouseEvent *event);

    /**
     * @brief 鼠标移动 棋子跟着移动
     * @param[in] *event 重写绘图事件用的event指针
     */
    void mouseMoveEvent(QMouseEvent *event);

    /**
     * @brief 鼠标释放 棋子落子
     * @param[in] *event 重写绘图事件用的event指针
     */
    void mouseReleaseEvent(QMouseEvent *event);

    /**
     * @brief 获取当前是黑白回合
     * @param[in] *event 重写绘图事件用的event指针
     */
    int getblackOrWhite() const;

    /**
     * @brief 切换黑白回合
     */
    void changeBlackAndWhite();

    /**
     * @brief 判断是否出界
     * @param[in] x    鼠标落点x坐标
     * @param[in] y    鼠标落点y坐标
     */
    bool isCrossLine( int x, int y );

    /**
     * @brief 判断输赢
     * @param[in] x     鼠标落点x坐标
     * @param[in] y     鼠标落点y坐标
     */
    bool isWin( int x, int y );

    /**
     * @brief 清空
     */
    void clear();

    /*-----------------------------------------------------------*/
    /*
     * time 2025.12.31
     */

    /**
     * @brief 初始化所有赢法——赢学
     */
    void InitAiVector();

    /**
     * @brief 电脑落子函数
     */
    void pieceDownByCpu();

    /**
     * @brief 设置电脑身份
     * @param[in] newCpuColor   人机阵营默认是None
     */
    void setCpuColor(int newCpuColor);

    /*-----------------------------------------------------------*/

    /**
     * @brief 设置自己身份
     */
    void setSelfStatus(int _status);

    /**
     * @enum ENUM_BLACK_OR_WHITEP
     * @brief 黑白子枚举\n
     * 存储五子棋阵营
     */
    enum ENUM_BLACK_OR_WHITEP{
        None = 0,       /**< 无阵营0代表默认关闭 */
        Black = 1,      /**< 黑子1 */
        White = 2       /**< 白子2 */
    };

public slots:
    /**
     * @brief 落子槽函数
     * @param[in] blackorwhite
     * @param[in] x
     * @param[in] y
     */
    void slot_pieceDown( int blackorwhite, int x, int y );
    /**
     * @brief 开始游戏槽函数
     */
    void slot_startGame();
    /**
     * @brief 计时器槽函数
     */
    void slot_countTimer();

private:
    Ui::FiveInLine *ui;

    int m_blackOrWhite;     ///< 记录当前是黑子还是白子回合

    QPoint m_movePoint;     ///< 鼠标移动中

    bool m_moveFlag;        ///< 移动标志

    //
    /**
     * @enum enum_direction
     * @brief 方向枚举 判断输赢\n
     * 在一条线的一组相邻，方便写循环
     * @note 根据方向对坐标的偏移 每次是一个单位\n
     * 1.根据当前点 一次查看左右 上下 左上右下 左下右上 四个方向棋子个数\n
     * 2.初始棋子个数为一 然后根据方向换算新的坐标 查看是否出界，出界break\n
     * 不然看是否和当前棋子同色 如果count到5 结束 没有到5 继续看其他直线\n
     */
    enum enum_direction{
        d_z,                ///< 左方向
        d_y,                ///< 右方向
        d_s,                ///< 上方向
        d_x,                ///< 下方向
        d_zs,               ///< 左上方向
        d_yx,               ///< 右下方向
        d_zx,               ///< 左下方向
        d_ys,               ///< 右上方向
        d_count             ///< 方向计数count
    };

    std::vector< std::vector<int> > m_board;       ///< 成员属性 棋盘 二维数组

    QBrush m_pieceColor[3];         ///< 棋子的颜色数组

    QTimer m_timer;                 ///< 定时器

    bool m_isOver;                  ///< 判断结束标志
    /**
     * @note dx,dy = -1,0 代表左方向，以此类推，左右 上下——注意这里x向右y向下
     */
    int dx[ d_count ] = {-1, 1, 0, 0, -1, 1, -1, 1 };   ///< 根据方向对坐标的偏移 每次是一个单位
    int dy[ d_count ] = { 0, 0, -1, 1, -1, 1, 1, -1 };  ///< 根据方向对坐标的偏移 每次是一个单位

    int m_status;                   ///< 网络版本中的玩家身份，不是自己回合是不能动的
    /*-----------------------------------------------------------*/
    /*
     * time 2026.1.1 Happy new year!!
     * 赢麻了ai
     */

    vector<stru_win> m_vecWin;      ///< 赢法数组

    int m_cpuColor;                 ///< 电脑回合

    QTimer m_countTimer;            ///< 每秒定时

    int m_colorCounter;             ///< 剩余时间计数器
    /*-----------------------------------------------------------*/

};
#endif // FIVEINLINE_H
