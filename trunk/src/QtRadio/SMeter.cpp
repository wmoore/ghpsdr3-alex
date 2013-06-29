#include "SMeter.h"
#include "Panadapter.h"
#include "UI.h"


SMeter::SMeter(QWidget* parent) : QFrame(parent) {
    SMeterMain=new Meter("Main Rx");
    SMeterSub=new Meter("Sub Rx");
    meter_dbm = -121;
    sub_meter_dbm = -121;
    subRx = FALSE;
}

SMeter::~SMeter() {

}

void SMeter::setSubRxState(bool state)
{
    subRx=state;
}

void SMeter::paintEvent(QPaintEvent*)
{
//qDebug() << "SMeter.cpp - Meter value is equal to " << meter_dbm;
//return;
    // Draw the Main Rx S-Meter
    QPainter painter(this);
    QImage image=SMeterMain->getImage(meter_dbm);
    painter.drawImage(4,0,image);

    // Draw the Sub Rx S-Meter
    if(subRx) {
        image=SMeterSub->getImage(sub_meter_dbm);
        painter.drawImage(4,image.height()+1,image);
    }
}
