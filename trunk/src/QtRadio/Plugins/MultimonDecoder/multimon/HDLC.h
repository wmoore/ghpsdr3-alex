#ifndef HDLC_H
#define HDLC_H

#include <QObject>

class HDLC : public QObject
{
    Q_OBJECT
public:
    explicit HDLC(QObject *parent = 0);
    ~HDLC();

    //void demod(float *buffer, int length);
    //void reset();

    void hdlc_init(struct demod_state *state);
    void hdlc_rxbit(struct demod_state *state, int bit);

signals:
    void newMessage(const QString &message);

public slots:

private:
    //struct demod_state *state;

};

#endif // HDLC_H
