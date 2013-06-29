#include "RBClient.h"
#include "ui_RBClient.h"
#include "UI.h"
#include <QtCore>
#include <QtGui>
#include <QtWidgets/QWidget>
#include <QTableWidget>
#include <QDebug>

RBClient::RBClient(QWidget *parent) : QWidget(parent), ui(new Ui::RBClient)
{
     //myMainWindow = MWindow;
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
     rbsocket = new QTcpSocket(this);
     connect(rbsocket,SIGNAL(connected()), this, SLOT(connected()));
     connect(rbsocket,SIGNAL(readyRead()), this, SLOT(readyRead()));
     connect(ui->tableWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(row_clicked(QModelIndex)));
     this->initRBClient(1);

}

RBClient::~RBClient()
{
    delete ui;
}

void RBClient::initRBClient(int index)
{
    if (index == 1) initTableWidget();
    else closeRBClient();
}


void RBClient::initTableWidget()
{
//    myMainWindow->disconnectRadio();
//    myMainWindow->radio_disconnected();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(12);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setHorizontalHeaderLabels(QString("DX;de;Spotter;Frequency;DX Call;Mode;db;DB;wpm;WPM;cq;Time").split(";"));
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->hide();
    rbsocket->connectToHost("telnet.reversebeacon.net", 7000);
}

void RBClient::connected()
{
//    myMainWindow->setWindowTitle("QtRadioManager - connected to telnet.reversebeacon.net");
//    //qDebug() << "connected to reversebeacon.net";
    rbsocket->write("dl6kbg\n");
    rbsocket->waitForBytesWritten(100);
    rbsocket->write("acc/spot on 7000/7200\n");
    rbsocket->waitForReadyRead(3000);

}

void RBClient::readyRead()

{
      // read from the server
    rbsocket->setReadBufferSize(2048);
    QString rbSpotShow = rbsocket->read(2048);
    QStringList lines = rbSpotShow.split("\r\n");


    int y = lines.size()-1;
    int x = 0;
     do
    {
        QStringList tokens = rbSpotShow.split(" ", QString::SkipEmptyParts);

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->setRowCount(row + 1);

        for(int column = 0; column < tokens.count(); column++)
        {

            //qDebug() << "row: " << row << "column: " << column;
            ui->tableWidget->setItem(row, column, new QTableWidgetItem(tokens[column]));
            ui->tableWidget->scrollToBottom();
            ui->tableWidget->setAlternatingRowColors(true);
            ui->tableWidget->hideColumn(0);
            ui->tableWidget->hideColumn(1);
            ui->tableWidget->hideColumn(7);
            ui->tableWidget->hideColumn(9);
            ui->tableWidget->hideColumn(10);
            ui->tableWidget->hideColumn(11);
        }

        x++;


    } while (x < y);

}

//// get qrg and call of clicked row
void RBClient::row_clicked(QModelIndex)
{
    // read the call we want to listen to from column 4
    QString call;
    QTableWidgetItem* itm_call = ui->tableWidget->item(ui->tableWidget->currentRow(),4);
    if (itm_call)
        call = itm_call->text();
    //myMainWindow->setWindowTitle("QtRadioManager - connected to telnet.reversebeacon.net - listening to: " +call);

    // read the qrg we want to tune
    QString qrg;
    QTableWidgetItem* itm_qrg = ui->tableWidget->item(ui->tableWidget->currentRow(),3);
    if (itm_qrg)
        qrg = itm_qrg->text();

    // manipulate the dx servers qrg from i.e. 7003.8 (khz) to 7003800 (hz)
    qrg.remove(QChar('.'), Qt::CaseInsensitive);
    qrg.append("00");
    qDebug() << "will send to QtRadio: " << qrg;

    // update vfo
//    myMainWindow->writeVFO(qrg);
//    myMainWindow->ui->labelMode->setText("CW");
//    myMainWindow->ui->labelFilter->setText("500");

    // lets connect to radio
    radiosocket = new QTcpSocket(this);
    QSettings settings("DL6KBG", "QtRadioManager");
    settings.beginGroup("Receiver");
    int HAMLIB_PORT = (settings.value("HAMLIB_PORT", 19090)).toInt();
    settings.endGroup();
    radiosocket->connectToHost("127.0.0.1", HAMLIB_PORT);
    radiosocket->waitForConnected();

        QString s_qrg = "F ";
        QString s_empty = " ";
        QString s_mode = "M ";
        QString s_return = "\n";

        QByteArray array_qrg;
        array_qrg.append(s_qrg);
        array_qrg.append(s_empty);
        array_qrg.append(qrg);
        array_qrg.append(s_return);

        QByteArray array_mode;
        array_mode.append(s_mode);
        array_mode.append(s_empty);
        array_mode.append("CW");
        array_mode.append(s_empty);
        array_mode.append("500");
        array_mode.append(s_return);

        radiosocket->write(array_mode);
        //socket->waitForBytesWritten();
        radiosocket->waitForReadyRead(100);
        radiosocket->write(array_qrg);
        radiosocket->close();

}

void RBClient::closeRBClient()
{
    rbsocket->close();
    //myMainWindow->setWindowTitle("QtRadioManager: Disconnected");

}
