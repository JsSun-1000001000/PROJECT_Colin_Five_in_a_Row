/**
 * @file fiveinline.cpp
 * @brief Five in line game (gomoku) cpp file
 * @author jssun
 * @date 2026-01-03
 * @version 0.0.1
 */
#include "fiveinline.h"
#include "ui_fiveinline.h"
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include <QMessageBox>

/**
 * @brief fiveinline构造函数
 * @details 初始化各个属性：\n
 * 黑子回合 鼠标移动0,0 移动标志置false\n
 * 初始化棋盘二维数组 棋局结束置false\n
 * 网络版本中的玩家身份默认黑\n
 * 剩余时间置为DEFAULT_COUNTER 31
 */
FiveInLine::FiveInLine(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FiveInLine)
    , m_blackOrWhite(1)
    , m_movePoint( 0,0 )
    , m_moveFlag( false )
    , m_board( FIL_COLS, std::vector<int>( FIL_ROWS, 0 ) )
    , m_isOver(false)
    , m_status( Black ) //有参构造
    , m_colorCounter( DEFAULT_COUNTER )

    //vector 初始化 有参构造 参数 多长 初值多少
{
    ui->setupUi(this);

    InitAiVector();     ///< 赢学人机初始化调用

    m_pieceColor[None] = QBrush( QColor(0,0,0,0) );         ///< 最后一个0透明色
    m_pieceColor[Black] = QBrush( QColor(0,0,0) );          ///< 初始化黑棋黑色
    m_pieceColor[White] = QBrush( QColor(255,255,255) );    ///< 初始化白棋白色

    /**
     * @brief 连接计时器的信号和槽
     * @details 信号timeout 槽函数repaint 到时间重绘窗口
     * @note 当检测到timeout信号时候repaint函数触发，qt会立即执行paintEvent，即立即重绘
     * 因此repaint是最快的。区别于update，update调用后不会立即重绘，而是发送重绘事件，放入事件队列
     * 等到下次事件循环的时候，再处理重绘事件。此外，update 还做了一些优化，
     * 如果 update 在一个事件循环内被调用多次，这些 update 会被合并成一个大的重绘事件加入到事件队列
     * 下次刷新的时候，只需要执行一次重绘。
     * @warning 在paintEvent()中不能使用repaint会陷入死循环
     * @see https://www.cnblogs.com/linuxAndMcu/p/17145322.html
     */
    connect( &m_timer, SIGNAL( timeout() ),
             this, SLOT( repaint() ) );                     ///< 连接计时器的信号和槽

    m_timer.start(1000/60);                                 ///< 一秒刷新60次 也就是60帧
    /**
     * @brief 连接落子信号和槽——本地版本
     * @details 本地版本是在这里触发connect，网络版本 中间有kernel通过kernel发送给服务器 单机版本直接触发
     * @code
     * connect( this, SIGNAL( SIG_pieceDown( int, int, int ) ),
             this, SLOT( slot_pieceDown( int, int, int ) ) );
     * @endcode
     */

    clear(); ///< 清空

    //slot_startGame();
    /**
     * @brief 链接计时器的信号和槽
     * @details 信号timeout 槽slot_countTimer 到时间更新文本切换回和
     */
    connect( &m_countTimer, SIGNAL( timeout() ),
            this, SLOT( slot_countTimer() ) );
}
/**
 * @brief fiveinline析构函数
 * @details 计时器停止
 */
FiveInLine::~FiveInLine()
{
    m_timer.stop();
    delete ui;
}

/**
 * @brief 重写绘图事件，绘制背景，棋盘，棋子
 */
void FiveInLine::paintEvent(QPaintEvent *event)
{
    ///< 绘制棋盘
    QPainter painter(this);///< QPainter 用于绘图 使用有参构造 传入当前对象 得到当前控件的画布
    painter.drawPixmap(FIL_MARGIN_WIDTH  - FIL_DISTANCE,
                       FIL_MARGIN_HEIGHT - FIL_DISTANCE,
                       ( FIL_COLS+1 ) * FIL_SPACE + 2 * FIL_DISTANCE,
                       ( FIL_ROWS+1 ) * FIL_SPACE + 2 * FIL_DISTANCE,
                       QPixmap(":/plank.jpg")); ///< 绘制棋盘背景照片
    /**
     * @details 过时了bro 看看我的原木风格棋盘
     * @code
     * painter.setBrush( QBrush( QColor( 255, 160, 0 ) ) );//通过画刷设置颜色
     * painter.drawRect( FIL_MARGIN_WIDTH  - FIL_DISTANCE,
     *                   FIL_MARGIN_HEIGHT - FIL_DISTANCE,
     *                   ( FIL_COLS+1 ) * FIL_SPACE + 2 * FIL_DISTANCE,
     *                   ( FIL_ROWS+1 ) * FIL_SPACE + 2 * FIL_DISTANCE );
     * @endcode
     *
     */

    /// 绘制网格线
    ///< 竖线
    for( int i = 1; i <= FIL_COLS; ++i ){
        painter.setBrush( QBrush( QColor(0,0,0) ) );
        QPoint p1( FIL_MARGIN_WIDTH + FIL_SPACE * i,
                   FIL_MARGIN_HEIGHT + FIL_SPACE );
        QPoint p2( FIL_MARGIN_WIDTH + FIL_SPACE * i,
                  FIL_MARGIN_HEIGHT + (FIL_ROWS) * FIL_SPACE );
        painter.drawLine( p1, p2 );
    }
    ///< 横线
    for( int i = 1; i <= FIL_ROWS; ++i ){
        painter.setBrush( QBrush( QColor(0,0,0) ) );
        QPoint p1( FIL_MARGIN_WIDTH + FIL_SPACE,
                   FIL_MARGIN_HEIGHT + FIL_SPACE * i );
        QPoint p2( FIL_MARGIN_WIDTH + (FIL_COLS) * FIL_SPACE,
                   FIL_MARGIN_HEIGHT + FIL_SPACE * i );
        painter.drawLine( p1, p2 );
    }

    ///< 画中心点
    painter.setBrush( QBrush( QColor(0,0,0) ) );
    painter.drawEllipse( QPoint(290,290), 6 / 2, 6 / 2 );

    ///< 绘制棋子
    for( int x = 0; x < FIL_COLS; ++x ){
        for( int y = 0; y < FIL_ROWS; ++y ){

            if( m_board[x][y] != None ){
                painter.setBrush( m_pieceColor[ m_board[x][y] ] );
                QPoint center( FIL_MARGIN_WIDTH + FIL_SPACE + FIL_SPACE * x ,
                               FIL_MARGIN_HEIGHT + FIL_SPACE + FIL_SPACE * y );
                painter.drawEllipse( center, FIL_PIECE_SIZE / 2, FIL_PIECE_SIZE / 2 );
            }
        }
    }
    //x y点对应的颜色 m_pieceColor[ m_board[x][y] ]

    ///< 绘制移动的棋子 当前回合是谁 画谁
    if( m_moveFlag ){
        //根据当前是黑子还是白子 绘制对应颜色的棋子
        painter.setBrush( m_pieceColor[ getblackOrWhite() ] );
        QPoint center( m_movePoint.x(),
                       m_movePoint.y() );
        painter.drawEllipse( center, FIL_PIECE_SIZE / 2, FIL_PIECE_SIZE / 2 );
    }
    event->accept();
}

/**
 * @brief 重写鼠标按下事件
 * @details 处理鼠标按下逻辑，记录点击位置并开启移动预览
 * @param[in] event 鼠标事件对象
 */
void FiveInLine::mousePressEvent(QMouseEvent *event)
{
    //捕捉点 相对坐标
    m_movePoint = event->pos();     ///< 记录鼠标按下位置
    /**
     * @note
     * 位置坐标
     * 相对坐标 相对窗口的
     * QMouseEvent :: pos();
     * 绝对坐标 相对屏幕的
     * QCursor::pos(); 鼠标位置
     * QMouseEvent::globalPos();
     */

    //加入结束判断
    if( m_isOver ) goto quit;
    if( m_status != getblackOrWhite() ) goto quit;
    //点击状态
    m_moveFlag = true;      ///< 开启移动预览标志

    quit:
    event->accept();
}

/**
 * @brief 重写鼠标移动事件
 * @details 处理鼠标移动逻辑，更新预览棋子位置
 * @param event 鼠标事件对象
 */
void FiveInLine::mouseMoveEvent(QMouseEvent *event)
{
    if(m_moveFlag){
        m_movePoint = event->pos();     ///< 捕捉点 相对坐标
    }
    //qDebug() << "mouse move:" << m_movePoint.x() << "," << m_movePoint.y();;
    event->accept();    ///< 表示事件已被处理 不再传播
}

/**
 * @brief 重写鼠标释放事件
 * @details 处理鼠标释放逻辑，计算落子坐标并发送落子信号
 * @param[in] *event 鼠标事件对象
 */
void FiveInLine::mouseReleaseEvent(QMouseEvent *event)
{
    //释放状态
    m_moveFlag = false;     ///< 关闭移动预览标志
    //计算落子位置 会涉及到取整和四舍五入的问题
    int x = 0;
    int y = 0;
    float fx = (float)event->pos().x();
    float fy = (float)event->pos().y();

    //游戏结束或非当前玩家回合则不处理
    if( m_isOver ) goto quit;
    if( m_status != getblackOrWhite() ) goto quit;

    //计算落子网格坐标
    fx = ( fx - FIL_MARGIN_WIDTH - FIL_SPACE ) / FIL_SPACE;
    fy = ( fy - FIL_MARGIN_HEIGHT - FIL_SPACE ) / FIL_SPACE;
    //类似4.4 4.5 4.6
    x = ( fx - (int)fx ) > 0.5 ? (int)fx + 1 : (int)fx;
    y = ( fy - (int)fy ) > 0.5 ? (int)fy + 1 : (int)fy;
    //是否越界
    if( isCrossLine( x, y ) ){
        return;
    }
    //发送落子信号
    Q_EMIT SIG_pieceDown( getblackOrWhite(), x, y );

    //落子
    quit:
    event->accept();
}

/**
 * @brief 获取当前回合的棋子颜色
 * @return 当前回合颜色（Black/White）
 */
int FiveInLine::getblackOrWhite() const
{
    return m_blackOrWhite;
}

/**
 * @brief 切换当前回合（黑→白/白→黑）
 * @details 切换回合标志并更新UI显示
 */
void FiveInLine::changeBlackAndWhite()
{
    //0,1,2
    m_blackOrWhite = m_blackOrWhite + 1;
    if( m_blackOrWhite == 3 ){
        m_blackOrWhite = Black;     ///< 循环切换121
    }
    //更新ui显示
    if( m_blackOrWhite == Black ){
        ui->lb_color->setText("黑子回合");
    }
    else{
        ui->lb_color->setText("白子回合");
    }
}

/**
 * @brief 判断坐标是否超出棋盘范围
 * @param[in] x 网格x坐标
 * @param[in] y 网格y坐标
 * @return 超出范围返回true，否则返回false
 */
bool FiveInLine::isCrossLine(int x, int y)
{
    if( x < 0   ||
        x >= 15 ||
        y < 0   ||
        y >= 15 ){
        return true;
    }
    return false;
}

/**
 * @brief 判断当前落子是否导致获胜
 * @details 检查落子位置在横竖斜四个方向是否有5个连续同色棋子
 * @param[in] x 落子网格x坐标
 * @param[in] y 落子网格y坐标
 * @return 获胜返回true 否则返回false
 */
bool FiveInLine::isWin(int x, int y)
{
    //看四条线八个方向 同色棋子个数
    int count = 1;///< 连续同色棋子个数 初始个数为1 包括自己
    //循环看四条线
    for( int dr = d_z; dr < d_count; dr += 2 ){
        ///< 检查4个方向
        for(int i = 1; i <= 4; ++i){
            //获取偏移后的棋子坐标
            int ix = dx[ dr ]*i + x;
            int iy = dy[ dr ]*i + y;
            //判断是否出界
            if( isCrossLine( ix, iy ) ){
                break;
            }
            //看是否同色
            if( m_board[ix][iy] == m_board[x][y] ){
                count++;
            }else{
                break;
            }
        }
        ///< 检查反方向
        for(int i = 1; i <= 4; ++i){
            //获取偏移后的棋子坐标
            int ix = dx[ dr + 1 ]*i + x;
            int iy = dy[ dr + 1 ]*i + y;
            //判断是否出界
            if( isCrossLine( ix, iy ) ){
                break;
            }
            //看是否同色
            if( m_board[ix][iy] == m_board[x][y] ){
                count++;
            }else{
                break;
            }
        }
        if( count >= 5 ){   ///< 达到5个则判定获胜 不看其他的了
            break;
        }
        else{
            count = 1;      ///< 重置计数检查下一个方向
        }
    }
    if( count >= 5 ){
        m_isOver = true;    ///< 设置游戏结束标志
        return true;
    }
    return false;
}

/**
 * @brief 清空棋盘并重置游戏状态
 * @details 充值棋盘数组、回合状态、ui显示和人机赢法统计
 */
void FiveInLine::clear()
{
    /*----------------------赢麻了人机----------------------------*/
    //time 2026.1.1 happy new year!!
    //清空赢法棋子个数统计
    setCpuColor( None );///< 默认不开人机 开就black或white
    for( int i = 0; i < m_vecWin.size(); ++i ){
        m_vecWin[i].clear();
    }

    /*----------------------赢麻了人机----------------------------*/
    //状态位
    m_isOver = true;
    m_blackOrWhite = Black;
    m_moveFlag = false;
    //界面
    for( int x = 0; x < FIL_COLS; ++x ){
        for( int y = 0; y < FIL_ROWS; ++y ){
            m_board[x][y] = None;
        }
    }
    ui->lb_winner->setText("");
    ui->lb_color->setText("黑子回合");

    ui->lb_timer->hide();
}

/**
 * @brief 设置玩家身份，用于网络对战
 * @param[in] _status 玩家身份 black 或 white
 */
void FiveInLine::setSelfStatus(int _status)
{
    m_status = _status;
}

/**
 * @brief 落子处理槽函数
 * @details 处理落子逻辑，更新棋盘状态，判断胜负，切换回和以及触发人机落子
 * @param[in] blackorwhite 落子颜色
 * @param[in] x 落子网格x坐标
 * @param[in] y 落子网格y坐标
 */
void FiveInLine::slot_pieceDown(int blackorwhite, int x, int y)
{
    //更新数组 切换回合
    if( blackorwhite != getblackOrWhite() ){
        return;
    }
    //落子 没有子才能放
    if( m_board[x][y] == None ){
        m_board[x][y] = blackorwhite;   ///< 更新棋盘状态

        //更新该点棋子颜色后 判断输赢
        if( isWin( x, y ) ){
            m_countTimer.stop();
            ui->lb_timer->hide();
            QString str = ( blackorwhite == Black ) ? "黑子" : "白子";
            ui->lb_winner->setText( str + "赢了！" );

            Q_EMIT SIG_playerWin( getblackOrWhite() );
            QMessageBox::information( this, "游戏结束", str + "赢了！" );
        }else{
            /*-------------电脑赢麻了----------------------*/
            //是不是电脑回合
            if( m_cpuColor != getblackOrWhite() ){
                //更新玩家每种赢法 玩家的棋子个数
                for( int i = 0; i < m_vecWin.size(); i++ ){
                    if(m_vecWin[i].board[x][y] == 1){
                        m_vecWin[i].playerCount += 1;
                        m_vecWin[i].cpuCount = 100;     ///< 该赢法人机失效
                    }
                }
            }
            else{
                //更新电脑每种赢法 电脑的棋子个数
                for( int k = 0; k < m_vecWin.size(); ++k ){
                    if( m_vecWin[k].board[x][y] == 1 ){
                        m_vecWin[k].cpuCount += 1;
                        m_vecWin[k].playerCount = 100;  ///< 该赢法玩家失效
                    }
                }
            }
            m_colorCounter = DEFAULT_COUNTER;
            //更换回合
            changeBlackAndWhite();
            //判断是否是电脑回合 电脑下棋
            if( m_cpuColor == getblackOrWhite() ){
                pieceDownByCpu();
            }
            /*-----------赢麻了AI-----------------------*/
            //changeBlackAndWhite();
        }
    }
}
/**
 * @brief 开始游戏槽函数
 * @details 充值游戏状态并启动倒计时
 */
void FiveInLine::slot_startGame()
{
    clear();
    m_isOver = false;

    m_colorCounter = DEFAULT_COUNTER ;
    ui->lb_timer->show();
    m_countTimer.start(1000);   ///< 启动倒计时定时器1秒
}

/**
 * @brief 倒计时更新槽函数
 * @details 每秒更新倒计时显示，超时则自动切换回合
 */
void FiveInLine::slot_countTimer()
{
    m_colorCounter--;
    if( m_colorCounter <= 0){
        //文本更新
        ui->lb_timer->setText( QString("%1秒").arg(m_colorCounter, 2, 10, QChar('0')) );
        //切换回合
        changeBlackAndWhite();      ///< 超时切换回合
        m_colorCounter = DEFAULT_COUNTER;
        return;
    }
    //界面文本更新
    ui->lb_timer->setText( QString("%1秒").arg(m_colorCounter, 2, 10, QChar('0')));
}
/*---------------------五子棋赢麻了人机ai--------------------------------*/
//time 2026.1.1
/**
 * @date 2026-01-01
 * @brief 初始化所有赢法数组
 * @details 生成所有可能的五子棋赢法布局，包括四个方向
 */
void FiveInLine::InitAiVector()
{
    //躺着赢
    for( int y = 0; y < FIL_ROWS; y++ ){
        for( int x = 0; x < FIL_COLS - 4; x++ ){
            stru_win w;
            for( int k = 0; k < 5; k++ ){
                //x+y y
                w.board[x+k][y] = 1;
            }
            m_vecWin.push_back(w);
        }
    }
    //站着赢
    for( int x = 0; x < FIL_COLS; x++ ){
        for( int y = 0; y < FIL_ROWS - 4; y++ ){
            stru_win w;
            for( int k = 0; k < 5; k++ ){
                //x+y y
                w.board[x][y+k] = 1;
            }
            m_vecWin.push_back(w);
        }
    }
    //右斜着赢/
    for( int x = FIL_COLS - 1; x >= 4; x-- ){
        for( int y = 0; y < FIL_ROWS - 4; y++ ){
            stru_win w;
            for( int k = 0; k < 5; k++ ){
                //x+y y
                w.board[x-k][y+k] = 1;
            }
            m_vecWin.push_back(w);
        }
    }
    //\左斜着赢
    for( int x = 0; x < FIL_COLS - 4; x++ ){
        for( int y = 0; y < FIL_ROWS - 4; y++ ){
            stru_win w;
            for( int k = 0; k < 5; k++ ){
                //x+y y
                w.board[x+k][y+k] = 1;
            }
            m_vecWin.push_back(w);
        }
    }
    qDebug()<<__func__;
}

/**
 * @brief 人机自动落子函数
 * @details 电脑落子 根据每种赢法 棋子的个数，
 * 给每一个无子的位置估分 得到所有的最优值 然后得到一个坐标
 */
void FiveInLine::pieceDownByCpu()
{
    if(m_isOver) return;
    int x = 0, y = 0, k = 0;
    int u = 0, v = 0;//最优目标坐标
    int MyScore[FIL_COLS][FIL_ROWS] = {};//记录玩家所有位置的分数
    int CpuScore[FIL_COLS][FIL_ROWS] = {};//记录电脑所有位置的分数

    int max = 0;//最高分数
    //估分 找到没有子的点 看每种赢法对应棋子的个数 进行这个点的分数计算
    for( x = 0; x < FIL_COLS; ++x){
        for( y = 0; y < FIL_ROWS; ++y ){
            if( m_board[x][y] == None ){
                //遍历所有赢法
                for( k = 0; k < m_vecWin.size(); ++k ){
                    //评估玩家在x y 分数
                    if( m_vecWin[k].board[x][y] == 1 ){
                        //根据该赢法 棋子个数
                        switch( m_vecWin[k].playerCount ){
                        case 1: MyScore[x][y] += 200;
                            break;
                        case 2: MyScore[x][y] += 400;
                            break;
                        case 3: MyScore[x][y] += 2000;
                            break;
                        case 4: MyScore[x][y] += 10000;
                            break;
                        }
                    }
                    //评估电脑在x y分数
                    if( m_vecWin[k].board[x][y] == 1 ){
                        //根据该赢法 棋子个数
                        switch( m_vecWin[k].cpuCount ){
                        case 1: CpuScore[x][y] += 220;
                            break;
                        case 2: CpuScore[x][y] += 420;
                            break;
                        case 3: CpuScore[x][y] += 4100;
                            break;
                        case 4: CpuScore[x][y] += 200000;
                            break;
                        }
                    }
                }
            }
        }
    }
    //第二种方法 和第一种没什么区别
    //int a = 0, b = 0;
    //int maxCpu;
    //估分之后找最优点
    for( x = 0; x < FIL_COLS; ++x){
        for( y = 0; y < FIL_ROWS; ++y){
            if( m_board[x][y] == None ){
                //先看玩家 MyScore看作是防守 cpu看作是进攻
                if( MyScore[x][y] > max ){
                    max = MyScore[x][y];
                    u = x;
                    v = y;
                }
                else if( MyScore[x][y] == max){
                    if( CpuScore[x][y] > CpuScore[u][v] ){
                        u = x;
                        v = y;
                    }
                }
                //再看电脑
                if( CpuScore[x][y] > max ){
                    max = CpuScore[x][y];
                    u = x;
                    v = y;
                }
                else if( CpuScore[x][y] == max){
                    if( MyScore[x][y] > MyScore[u][v] ){
                        u = x;
                        v = y;
                    }
                }
                /*第二种方法 和第一种没什么区别
                 * if(MyScore[x][y]>max){
                    max = MyScore[x][y];
                    u = x;
                    v = y;
                }
                if(CpuScore[x][y]>maxCpu){
                    maxCpu = CpuScore[x][y];
                    a = x;
                    b = y;
                }*/
            }
        }
    }
    /*第二种方法 和第一种没什么区别
     * if(max <= maxCpu){
        u = a;
        v = b;
    }*/
    //见更新电脑的每种赢法

    //得到最优目标坐标
    Q_EMIT SIG_pieceDown( getblackOrWhite(), u, v );
}

/**
 * @brief 设置人机棋子颜色
 * @param[in] newCpuColor (None/Black/White)
 */
void FiveInLine::setCpuColor(int newCpuColor)
{
    m_cpuColor = newCpuColor;
}
