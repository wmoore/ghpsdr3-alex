/* QtRadioManager
 *
 * file: mainwindow.cpp
 *
 * original software QtRadioManager:
 * Author: Oliver Goldenstein, DL6KBG
 * http://github.com/dl6kbg/qtradiomanager.git
 * http://www.oliver-goldenstein.de/qtradiomanager
 *
 * Created on 02 November 2012, 13:00
 *
 * Copyright, Oliver Goldenstein, 2012,2013
 *
 */

/*
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/


#include "ui_LogBook.h"
#include <QtCore>
#include <QtGui>
#include <QtSql>
#include <QAudioInput>
#include <QPushButton>
#include <qmediarecorder.h>
#include <QTableView>
#include <QMessageBox>
#include <QTimer>
#include <QtWidgets>
#include <QTabWidget>
#include "RBClient.h"
#include "LogBook.h"

#define HAMLIB_ADDR     "192.168.1.230"


LogBook::LogBook(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogBook)
{

   ui->setupUi(this);
   ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

   // open or create sqlite db file
   db = QSqlDatabase::addDatabase("QSQLITE");

   // FIXME choose different filename for WINDOWS
   // FIXME include directory in source tree/install
   db.setDatabaseName("db/qrg_db.sqlite");
   db.open();

   // create basic table if not exists
   QSqlQuery query;
   query.exec("CREATE TABLE IF NOT EXISTS frequencies (id INTEGER PRIMARY KEY, qrg NUMERIC, name VAR, mode VAR, filter VAR, remarks VAR, date VAR, day VAR, time VAR)");

   // connect pushbuttons
   connect(ui->pbClear, SIGNAL(clicked()), this, SLOT(clearLineEdits()));
   connect(ui->pbRecord, SIGNAL(clicked()), this, SLOT(startRecording()));

   /*
    *  FIXED
    *
    *  from QTimer Reference
    *  [...] you must start and stop the timer in its thread;
    *  it is not possible to start a timer from another thread.
    *
    */

   // we start the timer here and call the timerHandler
   timer = new QTimer(this);
   timer->start(1500);
   //readSettings();
   //radio_disconnected();


}

LogBook::~LogBook()
{
    //socket->close();
    db.close();
    delete ui;
//    writeSettings();
}



//void LogBook::connectRadio()
//{

//    QSettings settings("DL6KBG", "QtRadioManager");
//    settings.beginGroup("Receiver");
//    int HAMLIB_PORT = (settings.value("HAMLIB_PORT", 19090)).toInt();
//    settings.endGroup();
//    socket->connectToHost(HAMLIB_ADDR, HAMLIB_PORT);
//    qDebug() << "Connected to LogBook";

//}

/*
void LogBook::disconnectRadio()
{
    //socket->close();
    //qDebug() << "socket closed: Radio disconnected";
}
*/

void LogBook::timerHandler()
{
    qDebug() << "timerHandler(): called from QTimer";

    ui->labelTime->setText(QTime::currentTime().toString("hh:mm:ss"));
    ui->labelDate->setText(QDate::currentDate().toString("yyyy-MM-dd"));
    ui->labelDay->setText(QDate::currentDate().toString("dddd"));

    getQRG();
}

/*
void LogBook::radio_connected()
{
    qDebug() << "We are connected";
//    ui->actionConnect->setEnabled(false);
//    ui->actionDisconnect->setEnabled(true);
//    ui->tabWidget->setCurrentIndex(0);
    //setWindowTitle("QtRadioManager: Connected");

    connect(timer, SIGNAL(timeout()), this, SLOT(timerHandler())); // we are connected
    disconnect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(row_clicked(QModelIndex)));
    disconnect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(row_doubleClicked(QModelIndex)));
    connect(ui->pbAdd, SIGNAL(clicked()), this, SLOT(addDB()));

}

void LogBook::radio_disconnected()
{

//    QString freq = " 0 "; // we set this to fetch the whole db list later
    //qDebug() << "radio_disconnected: freq =" << freq;

    //qDebug() << "we are disconnected";
//    ui->actionConnect->setEnabled(true);
//    ui->actionDisconnect->setEnabled(false);

    //setWindowTitle("QtRadioManager: Disconnected");

//    disconnect(timer, SIGNAL(timeout()), this, SLOT(timerHandler())); // we are connected
//    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(row_clicked(QModelIndex)));
//    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(row_doubleClicked(QModelIndex)));
//    disconnect(ui->pbAdd, SIGNAL(clicked()), this, SLOT(addDB()));
//    queryDB(freq);

}
*/

void LogBook::getQRG()
{
/*
    // fetch frequency from radio
    socket->write("f\n");
    socket->waitForBytesWritten();
    socket->waitForReadyRead();

    QByteArray freq = socket->readAll();
    //fetch mode from radio
    socket->write("m\n");
    socket->waitForBytesWritten();
    socket->waitForReadyRead();

    QByteArray getQtRadio = socket->readAll();
    QStringList lines = QString(getQtRadio).split("\n");
    QString mode = lines.takeFirst();
    QString filter = lines.takeFirst();
*/

    //TODO - PULL FROM QtRadio Direct, Hamlib internal isn't working right and isn't right for an internal widget anyway
    QByteArray freq = "14253530";
    QString mode = "DSB";
    QString filter = "6600";

    qDebug()  << mode << filter;

    // update lineedits with freq, mode and filter
    QString freq_s = freq.simplified();
    ui->labelQRG->setText(freq_s);
    ui->labelMode->setText(mode);
    ui->labelFilter->setText(filter);
    writeVFO(freq);
    queryDB(freq);
}

// search for qrg in sqlite database
void LogBook::queryDB(QString freq)
{
    //QString limit = "LIMIT 30";
    QString order = "ORDER BY QRG ASC ";

    model = new QSqlQueryModel(this); // QSqlTabelModel ?
    model->setQuery("SELECT ID, QRG, MODE, FILTER, NAME, REMARKS, DATE, DAY, TIME FROM FREQUENCIES WHERE QRG>=" + freq + order);

    if (model->lastError().isValid())
           qDebug() << model->lastError();

    ui->tableView->setModel(model);

    // changed from setResizeMode which was uncompatible with qt5 edgers
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    //
    ui->tableView->setAlternatingRowColors(true);
    //ui->tableView->setStyleSheet("alternate-background-color: lightblue;background-color: grey;");
    ui->tableView->verticalHeader()->hide();
    ui->tableView->hideColumn(0);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void LogBook::setQRG()
{
    qDebug() << "setQRG called:";

}

void LogBook::addDB()
{
    QString qrg, name, mode, filter, remarks, date, day, time;

    qrg = (ui->labelQRG->text());  //fetch qrg from QtRadio
    name = (ui->lineEditName->text());
    mode = (ui->labelMode->text()); //fetch from QtRadio
    filter = (ui->labelFilter->text()); //fetch from QtRadio
    remarks = (ui->lineEditRemarks->text());
    date = (ui->labelDate->text()); //get actual date
    day = (ui->labelDay->text()); //get from actual day
    time = (ui->labelTime->text()); //get actual time

    //qDebug() << "qrg =" << qrg << "name =" << name << "mode =" << mode << "remarks =" << remarks;

    QSqlQuery query;
         query.prepare("INSERT INTO FREQUENCIES (qrg, name, mode, filter, remarks, date, day, time) "
                       "VALUES (?,?,?,?,?,?,?,?)");

         query.bindValue(0, qrg);
         query.bindValue(1, name);
         query.bindValue(2, mode);
         query.bindValue(3, filter);
         query.bindValue(4, remarks);
         query.bindValue(5, date);
         query.bindValue(6, day);
         query.bindValue(7, time);
         query.exec();
         //ui->statusBar->showMessage("entry added", 5000);
         clearLineEdits();
}

void LogBook::clearLineEdits()
{
    ui->lineEditName->clear();
    ui->lineEditRemarks->clear();
}

// start audio recording
void LogBook::startRecording()

{

    //FIXME - Records blank audio?

    QString name, mode, filter, remarks, date, day, time, file;

    QString qrg = (ui->labelQRG->text());  //fetch qrg from QtRadio
    qrg = qrg.simplified();

    mode = (ui->labelMode->text()); //fetch from QtRadio
    date = (QDate::currentDate().toString("yyyyMMdd"));
    time = (QTime::currentTime().toString("hhmmss"));

    audioRecorder = new QAudioRecorder(this);

    connect(audioRecorder, SIGNAL(durationChanged(qint64)), this, SLOT(recordProgress(qint64)));

    ui->pbRecord->setText(tr("stop"));
    ui->pbRecord->setStyleSheet("QPushButton { background-color: red }");
    disconnect(ui->pbRecord, SIGNAL(clicked()), this, SLOT(startRecording()));
    connect(ui->pbRecord, SIGNAL(clicked()), this, SLOT(stopRecording()));

    QAudioEncoderSettings settings;
    settings.setCodec("audio/PCM");
    settings.setSampleRate(8000);
    settings.setChannelCount(1);

    QString container = "wav";

    audioRecorder->setEncodingSettings(settings, QVideoEncoderSettings(), container);
    audioRecorder->setOutputLocation(QUrl("recordings/" +date+"_" +time +"_" +qrg +"_" +mode +".wav"));
    audioRecorder->record();

    //qDebug() << "recording";
    //ui->statusBar->showMessage("recording to: recordings/" +date +"_" +time +"_" +qrg +"_" +mode +".wav",5000);

}

// stop audio recording

void LogBook::stopRecording()
{
    //qDebug() << "stopRecording called";
    audioRecorder->stop();
    destinationFile.close();
    delete audioRecorder;
    disconnect(ui->pbRecord, SIGNAL(clicked()), this, SLOT(stopRecording()));
    connect(ui->pbRecord, SIGNAL(clicked()), this, SLOT(startRecording()));
    
    ui->pbRecord->setStyleSheet("QPushButton:disabled""{ color: gray }");
    ui->pbRecord->setText(tr("record"));
}


void LogBook::recordProgress(qint64 duration)
{
    if (audioRecorder->error() != QMediaRecorder::NoError || duration < 2000)
        return;

    //ui->statusBar->showMessage(tr("Recorded %1 sec").arg(duration / 1000));
}


void LogBook::row_clicked(QModelIndex index)
{

    //qDebug() << "row clicked";

    // get qrg from column one

    QString db_qrg = ui->tableView->model()->data(
                ui->tableView->model()->index(index.row() , 1 ) )
                .toString();

    //qDebug() << db_qrg;

    // get mode from column two

    QString db_mode = ui->tableView->model()->data(
                ui->tableView->model()->index(index.row() , 2 ) )
                .toString();

    //qDebug() << db_mode;

    // get filter from column three

    QString db_filter = ui->tableView->model()->data(
                ui->tableView->model()->index(index.row() , 3 ) )
                .toString();

    //qDebug() << db_filter;

    // lets update the labels
    ui->labelQRG->setText(db_qrg);
    ui->labelMode->setText(db_mode);
    ui->labelFilter->setText(db_filter);

    // connect to radio hamlib port
    QSettings settings("DL6KBG", "QtRadioManager");
    settings.beginGroup("Receiver");
    int HAMLIB_PORT = (settings.value("HAMLIB_PORT", 19090)).toInt();
    settings.endGroup();
  //  socket->connectToHost(HAMLIB_ADDR, HAMLIB_PORT);
  //  socket->waitForConnected();

    // qDebug() << "connected to radio";

    //FIXME add error message
    QString s_qrg = "F ";
    QString s_empty = " ";
    QString s_mode = "M ";
    QString s_return = "\n";

    QByteArray array_qrg;
    array_qrg.append(s_qrg);
    array_qrg.append(db_qrg);
    array_qrg.append(s_return);

    QByteArray array_mode;
    array_mode.append(s_mode);
    array_mode.append(db_mode);
    array_mode.append(s_empty);
    array_mode.append(db_filter);
    array_mode.append(s_return);

    //array_mode = array_mode.simplified();

    //socket->write(array_mode);
    //qDebug() << array_qrg;
    //socket->waitForBytesWritten();
    //socket->waitForReadyRead();
    //socket->write(array_qrg);
    //qDebug() << array_mode;
    //socket->close();
    writeVFO(db_qrg);
}

void LogBook::row_doubleClicked(QModelIndex dindex)
{

    QMessageBox msgBox;
    msgBox.setText("<b>Delete row from Database</b>.");
    msgBox.setInformativeText("Do you really want to delete the selected row ?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    switch (ret) {
      case QMessageBox::Ok:
          // ok was clicked
          deleteRow(dindex);
          break;
      case QMessageBox::Cancel:
          // Cancel was clicked
      //    radio_disconnected();
          break;
      default:
          // should never be reached
          break;
    }

}

void LogBook::deleteRow(QModelIndex dindex)
{
    QString myRow = ui->tableView->model()->data(
                ui->tableView->model()->index(dindex.row() , 0 ) )
                .toString();
    // QMessagebox DELETE or EDIT
    QSqlQuery query;
    query.prepare("DELETE FROM frequencies WHERE id = ?"); // or SELECT * FROM
    query.addBindValue(myRow);
    query.exec();
    //radio_disconnected();
}

/*
void LogBook::readSettings()
{
    QSettings settings("DL6KBG", "QtRadioManager");
    QRect rect = settings.value("geometry", QRect(68,491,1211,306)).toRect();
    move(rect.topLeft());
    resize(rect.size());
}


void LogBook::writeSettings()
{
    QSettings settings("DL6KBG", "QtRadioManager");
    settings.setValue("geometry", geometry());
}

void LogBook::about()
{
    QMessageBox::about(this, tr("About QtRadioManager"),
    tr("<p><h2>QtRadioManager by DL6KBG</h2></p>"
       "<p>QtRadioManager is a frequency and recordings manager for shortwave receivers. It currently supports Software Defined Receivers (SDR) like QtRadio and Quisk</a>.</p>"
       "<p>Copyright &copy; 2012,2013 Oliver Goldenstein, DL6KBG</p>"
       "<p>QtRadioManager is Open Source Software, licensed under the GNU General Public License (GPL).</p>"
       "<p>Sourcecode may be found at:</p>"
       "<p><i><pre>https://github.com/dl6kbg/qtradiomanager</i></pre></p>"
       "<p>Addtional information may be found at:</p>"
       "<p><i><pre>http://www.oliver-goldenstein.de/qtradiomanager</i></pre></p>"
       ));
}
*/

/*
void LogBook::on_actionHamlib_Port_triggered()
{
    QStringList items;
    items << "19090" << "4575" << "4532";
    bool ok;
    QString item = QInputDialog::getItem(this, "Hamlib Port", "<p><i>Receiver:</i></p>"
                                                              "<b>QtRadio</b> 19090 "
                                                              "<b>Quisk</b> 4575 "
                                                              "<b>Rigctld</b> 4532 ", items, 0, false, &ok);
    if (ok && !item.isEmpty())
    {
    };

    int port = item.toInt();
    QSettings settings("DL6KBG", "QtRadioManager");
    settings.beginGroup("Receiver");
    settings.setValue("HAMLIB_PORT", port);
    settings.endGroup();

}
*/

// taken from ghpsdr3-alex - QtRadio vfo.cpp 
void LogBook::writeVFO(QString freq)
{
    freq = freq.simplified();
    QString uOn;
    QString uOff;
    int cnt = 0;
    int stgChrs;
    stgChrs = freq.size() -1;
    ui->labelHz->setText("");
    ui->labelKHz->setText("");
    ui->labelMHz->setText("");
    for (cnt = stgChrs; cnt > -1; cnt--) {
        if (stgChrs - cnt < 3) ui->labelHz->setText(uOn+freq.at(cnt)+uOff+ui->labelHz->text());
        else if (stgChrs - cnt < 6) ui->labelKHz->setText(uOn+freq.at(cnt)+uOff+ui->labelKHz->text());
        else ui->labelMHz->setText(uOn+freq.at(cnt)+uOff+ui->labelMHz->text());
    }
}


