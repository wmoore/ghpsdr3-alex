/* QtRadioManager
 *
 * file: mainwindow.h
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

#ifndef LOGBOOK_H
#define LOGBOOK_H

#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include <QTcpSocket>
#include <QString>
#include <QAudioInput>
#include <QAudioRecorder>
#include <qmediarecorder.h>
#include <QTableView>
#include <QMessageBox>
#include <QTimer>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQueryModel>
#include <QFile>

namespace Ui {
    class LogBook;
}

class UI;
class LogBook : public QWidget
{
    Q_OBJECT

public:
    explicit LogBook(QWidget *parent = 0);
    ~LogBook();

    void queryDB(QString freq);
    void deleteRow(QModelIndex dindex);
    void writeVFO(QString freq);
    Ui::LogBook *ui;
    //int HAMLIB_PORT;

private:
    //Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlQueryModel *model;

    //QSqlTableModel *modell;
    //QTcpSocket *socket;
    QTimer *timer;
    QSqlQuery *query;
    QString line;
    QFile destinationFile;
    QAudioRecorder* audioRecorder;


public slots:
    void getQRG();
    void setQRG();
    void addDB();
    void clearLineEdits();
    void startRecording();
    void stopRecording();
    void recordProgress(qint64 duration);
    void row_clicked(QModelIndex index);
    void row_doubleClicked(QModelIndex dindex);
    //void about();
    //void connectRadio();
    //void disconnectRadio();
    //void radio_connected();
    //void radio_disconnected();
    void timerHandler();
    //void writeSettings();
    //void readSettings();
    
private slots:

    //void on_actionHamlib_Port_triggered();

};

#endif // MAINWINDOW_H
