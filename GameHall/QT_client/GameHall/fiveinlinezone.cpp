#include "fiveinlinezone.h"
#include "ui_fiveinlinezone.h"
#include <QMessageBox>

FiveInLineZone::FiveInLineZone(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FiveInLineZone)
{
    ui->setupUi(this);

    this->setWindowTitle( "五子棋专区" );

    m_layout = new QGridLayout;

    ui->scrollAreaWidgetContents_2->setLayout( m_layout );

    m_vecRoomItem.push_back(nullptr);

    for( int i = 0; i < 120; i++){
        RoomItem * item = new RoomItem;
        item->setInfo( i+1 );
        m_vecRoomItem.push_back( item );
        //roomitem->fiveinlinezone->ckernel
        connect(item, SIGNAL( SIG_JoinRoom(int) ),
                this,SIGNAL( SIG_joinRoom(int) ));
        m_layout->addWidget( item, i/2, i%2);
    }

}

FiveInLineZone::~FiveInLineZone()
{
    delete ui;
}
/*
 * time 2025.12.26
 * finish closeEvent
 */
void FiveInLineZone::closeEvent(QCloseEvent *event)
{
    if( QMessageBox::question( this, "exit?", "exit Five in line zone?")
        ==QMessageBox::Yes ){
        Q_EMIT SIG_close();
        event->accept();
    }
    else{
        event->ignore();
    }
}

std::vector<RoomItem *> &FiveInLineZone::getvecRoomItem()
{
    return m_vecRoomItem;
}
