#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QCheckBox>
#endif

#include "HardwarePerseus.h"

HardwarePerseus :: HardwarePerseus (Connection *pC, QWidget *pW): Hardware (pC, pW)
{

    // Attenuator
    QGroupBox *attGroupBox = new QGroupBox(tr("Attenuator"));
    attGroupBox->setFlat ( true );

    QRadioButton *att0Db  = new QRadioButton(tr("&0 dB"));
    QRadioButton *att10Db = new QRadioButton(tr("-&10 dB"));
    QRadioButton *att20Db = new QRadioButton(tr("-&20 dB"));
    QRadioButton *att30Db = new QRadioButton(tr("-&30 dB"));
    
    QVBoxLayout *vbox = new QVBoxLayout;

    vbox->addWidget(att0Db);
    vbox->addWidget(att10Db);
    vbox->addWidget(att20Db);
    vbox->addWidget(att30Db);
    vbox->addStretch(1);

    attGroupBox->setLayout(vbox);

    // att slider
    QSlider *pAttSlider = new QSlider(this);
    pAttSlider->setMaximum ( 0 );
    pAttSlider->setMinimum ( -40 );
    pAttSlider->setPageStep( 10 );

    // Dither
    QCheckBox *ditherCB = new QCheckBox(tr("Di&ther"));

    // Preamp
    QCheckBox *preampCB = new QCheckBox(tr("Pre&amplifier"));

    // Main layout of dialog
    QHBoxLayout *grid = new QHBoxLayout;

    // add objects
    grid->addWidget (attGroupBox);
    grid->addWidget (pAttSlider);
    grid->addWidget (ditherCB);
    grid->addWidget (preampCB);


    // use grid obecjt as main dialog's layout 
    setLayout(grid);

    // general features
    setWindowTitle(tr("Perseus"));
    //resize(240, 160);

    // interconnects
    attMapper = new QSignalMapper(this);
    connect(att0Db, SIGNAL(toggled(bool)), attMapper, SLOT(map()));
    attMapper->setMapping(att0Db, 0);

    connect(att10Db, SIGNAL(toggled(bool)), attMapper, SLOT(map()));
    attMapper->setMapping(att10Db, 10);

    connect(att20Db, SIGNAL(toggled(bool)), attMapper, SLOT(map()));
    attMapper->setMapping(att20Db, 20);

    connect(att30Db, SIGNAL(toggled(bool)), attMapper, SLOT(map()));
    attMapper->setMapping(att30Db, 30);

    connect(attMapper, SIGNAL(mapped(int)), this, SLOT(attClicked(int)));

    connect(pAttSlider, SIGNAL(valueChanged(int)), this, SLOT(attClicked(int)));

    connect(ditherCB, SIGNAL(stateChanged(int)),  this, SLOT(ditherChanged(int)));
    connect(preampCB, SIGNAL(stateChanged(int)),  this, SLOT(preampChanged(int)));

    // update the serial number in title bar
    QString command;
    command.clear(); QTextStream(&command) << "*getserial?";
    pConn->sendCommand (command);

    // defaults
    ditherCB->setCheckState(Qt::Checked);  // dither ON
    preampCB->setCheckState(Qt::Checked);  // preamp ON
    att0Db->setChecked(true);              // attenuator 0 dB
}

void HardwarePerseus :: attClicked(int state)
{
   if (state < 0) state = -state;
   qDebug() << "Attenuator: " << state << "dB";
   QString command;
   command.clear(); QTextStream(&command) << "*setattenuator " << state;
   pConn->sendCommand (command);
}

void HardwarePerseus :: ditherChanged(int state)
{
   qDebug() << "Dither: " << state;
   QString command;
   command.clear(); QTextStream(&command) << "*dither " << ((state==Qt::Checked) ? "on" : "off") ;
   pConn->sendCommand (command);
}
void HardwarePerseus :: preampChanged(int state)
{   
   qDebug() << "Preamp: " << state;
   QString command;
   command.clear(); QTextStream(&command) << "*preamp " << ((state==Qt::Checked) ? "on" : "off") ;
   pConn->sendCommand (command);
}


void HardwarePerseus :: processAnswer (QStringList list)
{
    if (list[0] == "*getserial?") {
       // try to set the serial
       qDebug() << Q_FUNC_INFO<<list[2];
       // change the title bar
       QString x;
       x.clear(); 
       QTextStream(&x) << windowTitle() << " - SN: " << list[2];

       setWindowTitle(x) ;
    }

}

HardwarePerseus :: ~HardwarePerseus ()
{

}

namespace {
   struct RegisterHw<HardwarePerseus> r("Perseus");
}
