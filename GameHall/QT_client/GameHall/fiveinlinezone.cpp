#include "fiveinlinezone.h"
#include "ui_fiveinlinezone.h"

FiveInLineZone::FiveInLineZone(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FiveInLineZone)
{
    ui->setupUi(this);

    m_layout = new QGridLayout;

    ui->scrollAreaWidgetContents_2->setLayout( m_layout );

    for( int i = 0; i < 120; i++){
        RoomItem * item = new RoomItem;
        m_layout->addWidget( item, i/2, i%2);
    }

}

FiveInLineZone::~FiveInLineZone()
{
    delete ui;
}
