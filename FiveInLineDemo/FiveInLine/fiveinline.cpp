#include "fiveinline.h"
#include "ui_fiveinline.h"
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include <QMessageBox>

FiveInLine::FiveInLine(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FiveInLine)
    , m_blackOrWhite(1)
    , m_movePoint( 0,0 )
    , m_moveFlag( false )
    , m_board( FIL_COLS, std::vector<int>( FIL_ROWS, 0 ) )
    , m_isOver(false)
    , m_colorCounter( DEFAULT_COUNTER )

    //vector 初始化 有参构造 参数 多长 初值多少
{
    ui->setupUi(this);

    InitAiVector();

    m_pieceColor[None] = QBrush( QColor(0,0,0,0) );//最后一个0透明色
    m_pieceColor[Black] = QBrush( QColor(0,0,0) );
    m_pieceColor[White] = QBrush( QColor(255,255,255) );

    //连接信号和槽
    connect( &m_timer, SIGNAL( timeout() ),
             this, SLOT( repaint() ) );
    m_timer.start(1000/60);//60帧

    //连接落子信号和槽
    //网络版本 中间有kernel通过kernel发送给服务器 单机版本直接触发
    connect( this, SIGNAL( SIG_pieceDown( int, int, int ) ),
             this, SLOT( slot_pieceDown( int, int, int ) ) );

    //cpu颜色
    setCpuColor( White );

    //connect( &m_countTimer, SIGNAL( timeout() ),
    //        this, SLOT( slot_countTimer() ) );

    slot_startGame();

    for(int i = 0; i < 16; i++){
        m_worker[i] = new MyWorker(this);
        connect(m_worker[i], &MyWorker::sig_getScoreFinish ,
                this,[&]( int x, int y, int score){          ///lambda表达式处理
                    //todo
                    //好像是单线程？不需要是线程安全的 没有并发？？
                    m_taskCount--;//count 会再分配任务时 重新赋值

                    m_vecScoreRes.push_back({x,y,score});

                    if(m_taskCount == 0){
                        ///将所有结果排序 最高分就是所求
                        sort(m_vecScoreRes.begin(),m_vecScoreRes.end(),[&](vector<int>&a,vector<int>&b){
                            return a[2]>b[2];//比较得分
                        });
                        int x = m_vecScoreRes[0][0];
                        int y = m_vecScoreRes[0][1];
                        if(x!=-1&&y!=-1){
                            emit SIG_pieceDown(getblackOrWhite(),m_vecScoreRes[0][0],m_vecScoreRes[0][1]);
                        }
                        m_vecScoreRes.clear();//没经过一轮 对数组清空
                    }
                });
    }
    connect( this, SIGNAL(sig_getBetterScore0(int,int,int)),m_worker[0],SLOT(slot_getBetterScore(int,int,int)));
    connect( this, SIGNAL(sig_getBetterScore1(int,int,int)),m_worker[1],SLOT(slot_getBetterScore(int,int,int)));
    connect( this, SIGNAL(sig_getBetterScore2(int,int,int)),m_worker[2],SLOT(slot_getBetterScore(int,int,int)));
    connect( this, SIGNAL(sig_getBetterScore3(int,int,int)),m_worker[3],SLOT(slot_getBetterScore(int,int,int)));
    connect( this, SIGNAL(sig_getBetterScore4(int,int,int)),m_worker[4],SLOT(slot_getBetterScore(int,int,int)));
    connect( this, SIGNAL(sig_getBetterScore5(int,int,int)),m_worker[5],SLOT(slot_getBetterScore(int,int,int)));
    connect( this, SIGNAL(sig_getBetterScore6(int,int,int)),m_worker[6],SLOT(slot_getBetterScore(int,int,int)));
    connect( this, SIGNAL(sig_getBetterScore7(int,int,int)),m_worker[7],SLOT(slot_getBetterScore(int,int,int)));
    connect( this, SIGNAL(sig_getBetterScore8(int,int,int)),m_worker[8],SLOT(slot_getBetterScore(int,int,int)));
    connect( this, SIGNAL(sig_getBetterScore9(int,int,int)),m_worker[9],SLOT(slot_getBetterScore(int,int,int)));
    connect( this, SIGNAL(sig_getBetterScore10(int,int,int)),m_worker[10],SLOT(slot_getBetterScore(int,int,int)));
    connect( this, SIGNAL(sig_getBetterScore11(int,int,int)),m_worker[11],SLOT(slot_getBetterScore(int,int,int)));
    connect( this, SIGNAL(sig_getBetterScore12(int,int,int)),m_worker[12],SLOT(slot_getBetterScore(int,int,int)));
    connect( this, SIGNAL(sig_getBetterScore13(int,int,int)),m_worker[13],SLOT(slot_getBetterScore(int,int,int)));
    connect( this, SIGNAL(sig_getBetterScore14(int,int,int)),m_worker[14],SLOT(slot_getBetterScore(int,int,int)));
    connect( this, SIGNAL(sig_getBetterScore15(int,int,int)),m_worker[15],SLOT(slot_getBetterScore(int,int,int)));
}

FiveInLine::~FiveInLine()
{
    m_timer.stop();
    delete ui;
}
//重写绘图事件 绘图背景 棋盘 棋子
void FiveInLine::paintEvent(QPaintEvent *event)
{
    //绘制棋盘
    QPainter painter(this);//QPainter 用于绘图 使用有参构造 传入当前对象 得到当前控件的画布
    painter.drawPixmap(FIL_MARGIN_WIDTH  - FIL_DISTANCE,
                       FIL_MARGIN_HEIGHT - FIL_DISTANCE,
                       ( FIL_COLS+1 ) * FIL_SPACE + 2 * FIL_DISTANCE,
                       ( FIL_ROWS+1 ) * FIL_SPACE + 2 * FIL_DISTANCE,
                       QPixmap(":/plank.jpg"));
    //painter.setBrush( QBrush( QColor( 255, 160, 0 ) ) );//通过画刷设置颜色
    /*
     * 过时了bro 看看我的原木风格棋盘
     *
     * painter.drawRect( FIL_MARGIN_WIDTH  - FIL_DISTANCE,
                      FIL_MARGIN_HEIGHT - FIL_DISTANCE,
                      ( FIL_COLS+1 ) * FIL_SPACE + 2 * FIL_DISTANCE,
                      ( FIL_ROWS+1 ) * FIL_SPACE + 2 * FIL_DISTANCE );*/
    //绘制网格线
    //竖线
    for( int i = 1; i <= FIL_COLS; ++i ){
        painter.setBrush( QBrush( QColor(0,0,0) ) );
        QPoint p1( FIL_MARGIN_WIDTH + FIL_SPACE * i,
                   FIL_MARGIN_HEIGHT + FIL_SPACE );
        QPoint p2( FIL_MARGIN_WIDTH + FIL_SPACE * i,
                  FIL_MARGIN_HEIGHT + (FIL_ROWS) * FIL_SPACE );
        painter.drawLine( p1, p2 );
    }
    //横线
    for( int i = 1; i <= FIL_ROWS; ++i ){
        painter.setBrush( QBrush( QColor(0,0,0) ) );
        QPoint p1( FIL_MARGIN_WIDTH + FIL_SPACE,
                   FIL_MARGIN_HEIGHT + FIL_SPACE * i );
        QPoint p2( FIL_MARGIN_WIDTH + (FIL_COLS) * FIL_SPACE,
                   FIL_MARGIN_HEIGHT + FIL_SPACE * i );
        painter.drawLine( p1, p2 );
    }

    //画中心点
    painter.setBrush( QBrush( QColor(0,0,0) ) );
    painter.drawEllipse( QPoint(290,290), 6 / 2, 6 / 2 );

    //绘制棋子
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

    //绘制移动的棋子 当前回合是谁 画谁
    if( m_moveFlag ){
        //根据当前是黑子还是白子 绘制对应颜色的棋子
        painter.setBrush( m_pieceColor[ getblackOrWhite() ] );
        QPoint center( m_movePoint.x(),
                       m_movePoint.y() );
        painter.drawEllipse( center, FIL_PIECE_SIZE / 2, FIL_PIECE_SIZE / 2 );
    }
    /*-----------------------绘制上一次落子的位置-------------------------------*/
    //time 2026.1.2
    if(!m_lastPos.isNull()){
        painter.setBrush(QBrush(QColor(0,250,0)));//通过画刷设置颜色
        int x1 = FIL_MARGIN_WIDTH + FIL_SPACE + FIL_SPACE * m_lastPos.x() - FIL_PIECE_SIZE / 2;//左上
        int y1 = FIL_MARGIN_HEIGHT + FIL_SPACE + FIL_SPACE * m_lastPos.y() - FIL_PIECE_SIZE / 2;

        int x2 = x1 + FIL_PIECE_SIZE;
        int y2 = y1;

        int x3 = x1;//左下
        int y3 = y1 + FIL_PIECE_SIZE;

        int x4 = x2;//右下
        int y4 = y3;

        painter.drawLine(QPoint(x1,y1),QPoint(x2,y2));
        painter.drawLine(QPoint(x1,y1),QPoint(x3,y3));
        painter.drawLine(QPoint(x2,y2),QPoint(x4,y4));
        painter.drawLine(QPoint(x3,y3),QPoint(x4,y4));
    }
    event->accept();
}

void FiveInLine::mousePressEvent(QMouseEvent *event)
{
    //捕捉点 相对坐标
    m_movePoint = event->pos();
    //位置坐标
    //相对坐标 相对窗口的
    //QMouseEvent :: pos();
    //绝对坐标 相对屏幕的
    //QCursor::pos(); 鼠标位置
    //QMouseEvent::globalPos();

    //加入结束判断
    if( m_isOver ) goto quit;
    /*-------------高级ai--------------------*/
    if( getblackOrWhite() == White){
        goto quit;
    }
    /*---------------------------------------*/
    //点击状态
    m_moveFlag = true;

    quit:
    event->accept();
}

void FiveInLine::mouseMoveEvent(QMouseEvent *event)
{
    //捕捉点 相对坐标
    if(m_moveFlag){
        m_movePoint = event->pos();
    }
    //qDebug() << "mouse move:" << m_movePoint.x() << "," << m_movePoint.y();;
    event->accept();
}

void FiveInLine::mouseReleaseEvent(QMouseEvent *event)
{
    //释放状态
    m_moveFlag = false;
    //计算落子位置 会涉及到取整和四舍五入的问题
    int x = 0;
    int y = 0;
    float fx = (float)event->pos().x();
    float fy = (float)event->pos().y();

    if( m_isOver ) goto quit;
    /*-------------高级ai--------------------*/
    if( getblackOrWhite() == White){
        goto quit;
    }
    /*---------------------------------------*/

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

int FiveInLine::getblackOrWhite() const
{
    return m_blackOrWhite;
}

void FiveInLine::changeBlackAndWhite()
{
    //0,1,2
    m_blackOrWhite = m_blackOrWhite + 1;
    if( m_blackOrWhite == 3 ){
        m_blackOrWhite = Black;
    }
    if( m_blackOrWhite == Black ){
        ui->lb_color->setText("黑子回合");
    }
    else{
        ui->lb_color->setText("白子回合");
    }
}

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

bool FiveInLine::isWin(int x, int y)
{
    //看四条线八个方向 同色棋子个数
    int count = 1;//初始个数为1 包括自己
    //循环看四条线
    for( int dr = d_z; dr < d_count; dr += 2 ){
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
        if( count >= 5 ){//不看其他的了
            break;
        }
        else{
            count = 1;
        }
    }
    if( count >= 5 ){
        m_isOver = true;
        return true;
    }
    return false;
}

void FiveInLine::clear()
{
    //清空赢法棋子个数统计
    for( int i = 0; i < m_vecWin.size(); ++i ){
        m_vecWin[i].clear();
    }
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

    m_colorCounter = DEFAULT_COUNTER ;
    ui->lb_timer->show();
    m_countTimer.start(1000);
}
//初始化所有赢法
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
//电脑落子 根据每种赢法 棋子的个数
//给每一个无子的位置估分 得到所有的最优值 然后得到一个坐标
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

void FiveInLine::slot_pieceDown(int blackorwhite, int x, int y)
{
    //更新数组 切换回合
    if( blackorwhite != getblackOrWhite() ){
        return;
    }
    //落子 没有子才能放
    if( m_board[x][y] == None ){
        m_board[x][y] = blackorwhite;

        m_lastPos = QPoint(x,y);
        m_everyStepPos.push_back({x,y});

        //更新该点棋子颜色后 判断输赢
        if( isWin( x, y ) ){
            m_countTimer.stop();
            ui->lb_timer->hide();
            QString str = ( blackorwhite == Black ) ? "黑子" : "白子";
            ui->lb_winner->setText( str + "赢了！" );

            QMessageBox::information( this, "游戏结束", str + "赢了！" );
        }else{
            /*-------------电脑赢麻了----------------------*/
            //是不是电脑回合
            if( m_cpuColor != getblackOrWhite() ){
                //更新玩家每种赢法 玩家的棋子个数
                for( int i = 0; i < m_vecWin.size(); i++ ){
                    if(m_vecWin[i].board[x][y] == 1){
                        m_vecWin[i].playerCount += 1;
                        m_vecWin[i].cpuCount = 100;
                    }
                }
            }
            else{
                //更新电脑每种赢法 电脑的棋子个数
                for( int k = 0; k < m_vecWin.size(); ++k ){
                    if( m_vecWin[k].board[x][y] == 1 ){
                        m_vecWin[k].cpuCount += 1;
                        m_vecWin[k].playerCount = 100;
                    }
                }
            }
            m_colorCounter = DEFAULT_COUNTER;
            //更换回合
            changeBlackAndWhite();
            //判断是否是电脑回合 电脑下棋
            if( m_cpuColor == getblackOrWhite() ){
#ifndef DEF_ALPHA_BETA
                pieceDownByCpu();
#else
                pieceDownByBetterCpu();
#endif
            }
            /*-----------赢麻了AI-----------------------*/
        }
    }
}

void FiveInLine::slot_startGame()
{
    clear();
    m_isOver = false;
}

//每秒更新一次
void FiveInLine::slot_countTimer()
{
    m_colorCounter--;
    if( m_colorCounter <= 0){
        //文本更新
        ui->lb_timer->setText( QString("%1秒").arg(m_colorCounter, 2, 10, QChar('0')) );
        //切换回合
        changeBlackAndWhite();
        m_colorCounter = DEFAULT_COUNTER;
        return;
    }
    //界面文本更新
    ui->lb_timer->setText( QString("%1秒").arg(m_colorCounter, 2, 10, QChar('0')));

}

//不使用现在的棋盘 使用临时棋盘
bool FiveInLine::isWin(int x, int y, vector<vector<int> > & board)
{
    //看四条线八个方向 同色棋子个数
    int count = 1;//初始个数为1 包括自己
    //循环看四条线
    for( int dr = d_z; dr < d_count; dr += 2 ){
        for(int i = 1; i <= 4; ++i){
            //获取偏移后的棋子坐标
            int ix = dx[ dr ]*i + x;
            int iy = dy[ dr ]*i + y;
            //判断是否出界
            if( isCrossLine( ix, iy ) ){
                break;
            }
            //看是否同色
            if( board[ix][iy] == board[x][y] ){
                count++;
            }else{
                break;
            }
        }
        for(int i = 1; i <= 4; ++i){
            //获取偏移后的棋子坐标
            int ix = dx[ dr + 1 ]*i + x;
            int iy = dy[ dr + 1 ]*i + y;
            //判断是否出界
            if( isCrossLine( ix, iy ) ){
                break;
            }
            //看是否同色
            if( board[ix][iy] == board[x][y] ){
                count++;
            }else{
                break;
            }
        }
        if( count >= 5 ){//不看其他的了
            break;
        }
        else{
            count = 1;
        }
    }
    if( count >= 5 ){
        //结束 //不能结束
        return true;
    }
    return false;
}

void FiveInLine::pieceDownByBetterCpu()
{
    if(m_isOver) return;
    int bestX = -1;
    int bestY = -1;

    findBestMove( bestX, bestY, getblackOrWhite(), MAX_DEPTH );

    if(bestX != -1 && bestY != -1){
        //找到最佳点 落子
        emit SIG_pieceDown( getblackOrWhite(), bestX, bestY );
    }
}

void FiveInLine::findBestMove(int &bestX, int &bestY, int player, int depth)
{
    //上来先是max层
    int bestValue = INT_MIN;
    bestX = -1;
    bestY = -1;

    vector<pair<int, int> > candidates;
    vector<pair<int, int> >& copyEveryStep = m_everyStepPos;//避免出现问题 使用拷贝的
    vector<vector<int> >& copyBoard = m_board;//同理棋盘也是

    //获取要看的点
    getNeedHandlePos(copyEveryStep, candidates, copyBoard );
    /// 并发执行 就不用排序了 都要算 不会剪枝 可以放到minmax函数里面
    /*---------------优化--------------------------*/
    //排序 用深度为1 的评估得分排序 先算好的 这样剪枝的可能性大大增强
    // sort(candidates.begin(), candidates.end(),[&](pair<int,int>&a,pair<int,int>&b){
    //     copyBoard[a.first][a.second] = player;
    //     int scoreA = evalueteBoard(copyBoard);
    //     copyBoard[a.first][a.second] = None;

    //     copyBoard[b.first][b.second] = player;
    //     int scoreB = evalueteBoard(copyBoard);
    //     copyBoard[b.first][b.second] = None;

    //     return scoreA > scoreB;//降序排列 优先看高分
    // });
    /// candidates 根据候选点的情况 分配到多个线程中————我是8核心 打算用8线程<——草泥马和我的24核说去吧
    int count = candidates.size();

    m_taskCount = count;

    for( int i = 0; i < count; ++i){
        switch(i%8){
        case 0: emit sig_getBetterScore0( candidates[i].first,candidates[i].second,player );
            break;
        case 1: emit sig_getBetterScore1( candidates[i].first,candidates[i].second,player );
            break;
        case 2: emit sig_getBetterScore2( candidates[i].first,candidates[i].second,player );
            break;
        case 3: emit sig_getBetterScore3( candidates[i].first,candidates[i].second,player );
            break;
        case 4: emit sig_getBetterScore4( candidates[i].first,candidates[i].second,player );
            break;
        case 5: emit sig_getBetterScore5( candidates[i].first,candidates[i].second,player );
            break;
        case 6: emit sig_getBetterScore6( candidates[i].first,candidates[i].second,player );
            break;
        case 7: emit sig_getBetterScore7( candidates[i].first,candidates[i].second,player );
            break;
        case 8: emit sig_getBetterScore8( candidates[i].first,candidates[i].second,player );
            break;
        case 9: emit sig_getBetterScore9( candidates[i].first,candidates[i].second,player );
            break;
        case 10:    emit sig_getBetterScore10( candidates[i].first,candidates[i].second,player );
            break;
        case 11:    emit sig_getBetterScore11( candidates[i].first,candidates[i].second,player );
            break;
        case 12:    emit sig_getBetterScore12( candidates[i].first,candidates[i].second,player );
            break;
        case 13:    emit sig_getBetterScore13( candidates[i].first,candidates[i].second,player );
            break;
        case 14:    emit sig_getBetterScore14( candidates[i].first,candidates[i].second,player );
            break;
        case 15:    emit sig_getBetterScore15( candidates[i].first,candidates[i].second,player );
            break;
        }
    }



    /*---------------------------------------------*/
    // //遍历所有可能位置
    // for (auto & pos : candidates){
    //     int x = pos.first;
    //     int y = pos.second;
    //     //这个位置之所以能下子 因为是空位置
    //     //尝试在该位置下子 如果直接能获得胜利 那么就返回
    //     copyBoard[x][y] = player;
    //     if(isWin(x,y,copyBoard)){
    //         bestX = x;
    //         bestY = y;
    //         return;
    //     }
    //     //尝试在该位置 敌人下子 如果能获得胜利 那么也返回
    //     copyBoard[x][y] = (player == Black)?White:Black;
    //     if(isWin(x, y, copyBoard)){
    //         bestX = x;
    //         bestY = y;
    //         return;
    //     }
    //     //切换回来 开始 搜索
    //     copyBoard[x][y] = player;
    //     copyEveryStep.push_back({x,y});
    //     //使用极大值极小值搜索配合α-β剪枝 先看min层
    //     int moveValue = minmax( copyBoard,
    //                            copyEveryStep,
    //                            depth - 1,
    //                            INT_MIN,
    //                            INT_MAX,
    //                            false,
    //                            player);
    //     //dfs 撤销该点落子
    //     copyBoard[x][y] = None;
    //     copyEveryStep.pop_back();

    //     if(moveValue > bestValue){
    //         bestValue = moveValue;
    //         bestX = x;
    //         bestY = y;
    //     }
    // }
}
#include <unordered_set>
void FiveInLine::getNeedHandlePos(vector<pair<int, int> > &copyEveryStep,
                                  vector<pair<int, int> > &candidates,
                                  vector<vector<int> > &board)
{
    //根据历史下棋位置 找附近1个位置 也就是3*3的位置
    //去重添加到数组中 最好的方式是最近下的先添加
    unordered_set<int> unique;
    //使用反向迭代器 反向遍历
    for( auto ite = copyEveryStep.rbegin(); ite != copyEveryStep.rend(); ++ite){
        auto & pos = *ite;
        int x = pos.first;
        int y = pos.second;
        for(int i = -1; i <= 1; i++){
            for(int j = -1;j <= 1; ++j){
                if(i == 0 && j == 0){
                    continue;
                }
                int nx = x+i;
                int ny = y+j;
                if(nx<0 || nx>=FIL_COLS || ny<0 || ny>=FIL_ROWS) continue;
                if(board[ nx ][ ny ] != None) continue;//只看空白点

                //去重
                int key = nx * 100 + ny;//nx ny不会超过15
                if(unique.count(key) == 0){//没有
                    unique.insert(key);
                    candidates.push_back({nx,ny});
                }

            }
        }
    }
}

int FiveInLine::minmax(vector<vector<int> > &copyBoard,
                       vector<pair<int, int> > &copyEveryStep,
                       int depth, int alpha, int beta,
                       bool isMaximizing, int player)
{
    //其实本质上是dfs 只是max层 min层 取子节点的min值
    //分别对应 ai 最大化自己的得分 以及玩家最小化 ai得分
    //一会实现剪枝
    //递归终点 评估棋盘
    if(depth == 0){
        //评估棋盘 todo
        return evalueteBoard(copyBoard);
    }
    int bestValue = isMaximizing ? INT_MIN : INT_MAX;
    int opponent = (player == Black) ? White : Black;

    vector<pair<int, int> > candidates;
    getNeedHandlePos(copyEveryStep, candidates, copyBoard );

    if(candidates.empty()) return 0;
    //对于max层才能这样看 min层要升序 不过会崩溃暂时不知道什么原因 先不看

    //这个位置可能是负优化

    if(isMaximizing){
        //排序 用深度为1 的评估得分排序 先算好的 这样剪枝的可能性大大增强
        sort(candidates.begin(), candidates.end(),[&](pair<int,int>&a,pair<int,int>&b){
            copyBoard[a.first][a.second] = player;
            int scoreA = evalueteBoard(copyBoard);
            copyBoard[a.first][a.second] = None;

            copyBoard[b.first][b.second] = player;
            int scoreB = evalueteBoard(copyBoard);
            copyBoard[b.first][b.second] = None;

            return scoreA > scoreB;//降序排列 优先看高分
        });
    }
    // else{
    //     //todo 会崩溃先不看了
    //     //玩家的回合可以考虑 玩家的得分最高来排序 只看黑棋的分数 黑棋分数最大 白棋就最低
    //     sort(candidates.begin(), candidates.end(),[&](pair<int,int>&a,pair<int,int>&b){
    //         copyBoard[a.first][a.second] = player;
    //         int scoreA = evalueteBoard(Black, copyBoard);
    //         copyBoard[a.first][a.second] = None;

    //         copyBoard[b.first][b.second] = player;
    //         int scoreB = evalueteBoard(Black, copyBoard);
    //         copyBoard[b.first][b.second] = None;

    //         return scoreA > scoreB;//降序排列 优先看高分
    //     });
    // }

    //遍历所有可能位置
    for(auto & pos : candidates){
        int x = pos.first;
        int y = pos.second;

        //逻辑和 findBestMove 类似
        //尝试落子
        copyBoard[x][y] = isMaximizing ? player : opponent;
        copyEveryStep.push_back({x,y});

        //检查是否获胜
        bool win = isWin(x, y, copyBoard);
        if(win){
            //返回不需要继续搜索
            copyBoard[x][y] = None;
            copyEveryStep.pop_back();
            return isMaximizing ? 1000000 : -1000000;//其实就是正无穷和负无穷
        }
        //dfs并撤销
        int value = minmax( copyBoard,
                           copyEveryStep,
                           depth-1,
                           alpha,
                           beta,
                           !isMaximizing,
                           player);
        //撤销
        copyBoard[x][y] = None;
        copyEveryStep.pop_back();

        //更新最佳 实现 α-β剪枝
        if(isMaximizing){
            if(value > bestValue){
                bestValue = value;
            }
            if(bestValue > alpha){
                alpha = bestValue;
                if( alpha >= beta ) break;//alpha剪枝
            }
        }
        else{
            if(value < bestValue){
                bestValue = value;
            }
            if(bestValue > alpha){
                alpha = bestValue;
                if( beta <= alpha ) break;//beta剪枝
            }
        }
    }
    return bestValue;
}

int FiveInLine::evalueteBoard(int color, vector<vector<int> > &board)
{
    int values = 0;
    //遍历棋盘所有位置
    //不是想要的颜色跳过
    for( int x = 0; x <FIL_COLS; ++x){
        for( int y = 0; y < FIL_ROWS; ++y){
            if( board[x][y] != color ){
                continue;
            }
            //处理8个方向
            int j = 0;
            while(j < directions.size()){
                int count = 1;
                vector<int> record;//记录两个方向上中止的原因

                //检查一对方向
                for(int a = 0; a < 2; a++){
                    int curX = x;
                    int curY = y;
                    for( int i = 0;i < 4; ++i){
                        //查看延伸的两个方向各4个点 也就是看9个点
                        //检查边界
                        if(curX + directions[j].first < 0 ||
                            curX + directions[j].first >= FIL_COLS ||
                            curY + directions[j].second < 0 ||
                            curY + directions[j].second >= FIL_ROWS){
                            record.push_back(3-color);//认为超出边界也是对方堵死
                            break;//换另一个方向
                        }
                        //移动到下一个位置
                        curX += directions[j].first;
                        curY += directions[j].second;
                        if(board[curX][curY] == color){
                            count++;
                        }
                        else{
                            record.push_back(board[curX][curY]);
                            break;
                        }
                    }
                    j++;//切换方向 应该是一组里面
                }
                //看完一族的个数 可以看棋子得分
                //五连200000 活四10000必赢 死四 1000 活三 900一定要拦补一个就赢
                //死三100
                //活二90
                //死二 10
                if(count >= 5){
                    values += 200000;
                }
                else if(count == 4){
                    if(record[0] == 0 && record[1] == 0){
                        //活4
                        values += 10000;
                    }
                    else if(record[0] == 0 && record[1] == 3-color ||
                            record[1] == 0 && record[0] == 3-color){
                        //死4
                        values += 5000;
                    }
                }
                else if(count == 3){
                    if(record[0] == 0 && record[1] == 0){
                        //活3
                        values += 1000;
                    }
                    else if(record[0] == 0 && record[1] == 3-color ||
                            record[1] == 0 && record[0] == 3-color){
                        //死3
                        values += 500;
                    }
                }
                else if(count == 2){
                    if(record[0] == 0 && record[1] == 0){
                        //活2
                        values += 90;
                    }
                    else if(record[0] == 0 && record[1] == 3-color ||
                            record[1] == 0 && record[0] == 3-color){
                        //死2
                        values += 10;
                    }
                }
            }
        }
    }
    return values;
}

int FiveInLine::evalueteBoard(vector<vector<int> > &board)
{
    //放引用就可以 值传递容易栈溢出
    ///先计算棋盘hash
    /// 查看是否有 有就直接返回
    /// 没有的话 计算并添加到缓存
    /// 先查看是否要淘汰，如果是就先淘汰
    string hash = getBoardHash(board);

    if(evalueteCache.count(hash)>0){
        return evalueteCache[hash];
    }

    int white = evalueteBoard( White, board );
    int black = evalueteBoard( Black, board );

    /// 这里多线程访问 访问共享资源 需要加锁
    {
        lock_guard<mutex> lck(m_mtx);
        if(CacheQueue.size()>1000){
            evalueteCache.erase(CacheQueue.front());/// 根据key值删除，队列中添加的是字符串key值
            CacheQueue.pop_front();
        }

        //缓存结果
        evalueteCache[hash] = white-black;
        CacheQueue.push_back(hash);
    }

    return white - black;
}

string FiveInLine::getBoardHash(vector<vector<int> > &board)
{
    string hash;
    for( int i = 0;i < FIL_COLS; ++i){
        for( int j = 0; j < FIL_ROWS; ++j){
            hash += to_string(board[i][j]);
        }
    }
    return hash;
}

void FiveInLine::setCpuColor(int newCpuColor)
{
    m_cpuColor = newCpuColor;
}
