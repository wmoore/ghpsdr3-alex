#ifndef SERVERS_H
#define SERVERS_H
#include <QNetworkReply>
#include <QtCore>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
#else
#include <Qwidget>
#endif

namespace Ui {
    class Servers;
}
class UI;
class Servers : public QWidget
{
    Q_OBJECT

public:
    explicit Servers(QWidget *parent = 0 );

    ~Servers();
    void refreshList();
signals:
    void disconnectNow();
    void connectNow(QString IP);
    void dialogClosed ();

private slots:
    void on_closebutton_clicked();
    void finishedSlot(QNetworkReply* reply);
    void on_refreshButton_clicked();
    void on_QuickDisconnect_clicked();
    void on_QuickConnect_clicked();
    void TimerFired();
    void closeEvent(QCloseEvent *event);

private:
    Ui::Servers *ui;
    void populateList(QStringList list);
    void addLine(QString line);

};

#endif // SERVERS_H


