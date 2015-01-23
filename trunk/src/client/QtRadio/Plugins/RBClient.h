#ifndef RBCLIENT_H
#define RBCLIENT_H

//#include <QMainWindow>
//#include "UI.h"
//#include "UI_mainwindow.h"
//#include <QtGui>

#include <QtCore>
#include <QtWidgets/QWidget>
#include <QTcpSocket>


namespace Ui {
    class RBClient;
}

class UI;
class RBClient : public QWidget
{
    Q_OBJECT

public:
    explicit RBClient(QWidget *parent = 0);
    ~RBClient();
signals:

public slots:
    void initRBClient(int index);
    void initTableWidget();
    void connected();
    void readyRead();
    void row_clicked(QModelIndex);
    void closeRBClient();

private slots:

private:
    Ui::RBClient *ui;
    QTcpSocket *rbsocket;
    QTcpSocket *radiosocket;

};


#endif // RBCLIENT_H
