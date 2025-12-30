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
    , m_status( Black ) //有参构造

    //vector 初始化 有参构造 参数 多长 初值多少
{
    ui->setupUi(this);

    m_pieceColor[None] = QBrush( QColor(0,0,0,0) );//最后一个0透明色
    m_pieceColor[Black] = QBrush( QColor(0,0,0) );
    m_pieceColor[White] = QBrush( QColor(255,255,255) );

    //连接信号和槽
    connect( &m_timer, SIGNAL( timeout() ),
             this, SLOT( repaint() ) );
    m_timer.start(1000/60);//60帧

    //连接落子信号和槽
    //网络版本 中间有kernel通过kernel发送给服务器 单机版本直接触发
    //本地版本是在这里触发connect
    /*connect( this, SIGNAL( SIG_pieceDown( int, int, int ) ),
             this, SLOT( slot_pieceDown( int, int, int ) ) );*/

    clear();
    //slot_startGame();
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
    if( m_status != getblackOrWhite() ) goto quit;
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
    if( m_status != getblackOrWhite() ) goto quit;

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
}

void FiveInLine::setSelfStatus(int _status)
{
    m_status = _status;
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

        //更新该点棋子颜色后 判断输赢
        if( isWin( x, y ) ){
            QString str = ( blackorwhite == Black ) ? "黑子" : "白子";
            ui->lb_winner->setText( str + "赢了！" );

            Q_EMIT SIG_playerWin( getblackOrWhite() );
            QMessageBox::information( this, "游戏结束", str + "赢了！" );
        }else{
            changeBlackAndWhite();
        }
    }
}

void FiveInLine::slot_startGame()
{
    clear();
    m_isOver = false;
}
