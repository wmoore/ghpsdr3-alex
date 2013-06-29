#ifndef KEYPAD_H
#define KEYPAD_H

#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QDialog>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QDialogButtonBox>
#else
#include <QDialog>
#include <QAbstractButton>
#include <QDialogButtonBox>
#endif


#include <QKeyEvent>


namespace Ui {
    class Keypad;
}

class Keypad : public QDialog
{
    Q_OBJECT

public:
    explicit Keypad(QWidget *parent = 0);
    ~Keypad();
    long long getFrequency();
    void clear();
	void commitFrequency();

public slots:
    void key_0();
    void key_1();
    void key_2();
    void key_3();
    void key_4();
    void key_5();
    void key_6();
    void key_7();
    void key_8();
    void key_9();
    void key_period();

    void clicked(QAbstractButton*);

signals:
    void setKeypadFrequency(long long);

protected:
	void keyPressEvent(QKeyEvent *event);

private:
    Ui::Keypad *ui;

    void showFrequency();

    //long long frequency;
    QString frequency;
};

#endif // KEYPADD_H
