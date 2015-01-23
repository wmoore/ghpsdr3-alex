#ifndef DEMOD_DTMF_H
#define DEMOD_DTMF_H

#define SAMPLE_RATE 22050
#define BLOCKLEN (SAMPLE_RATE/100)  /* 10ms blocks */
#define BLOCKNUM 4    /* must match numbers in multimon.h */

#define PHINC(x) ((x)*0x10000/SAMPLE_RATE)

static const char *dtmf_transl = "123A456B789C*0#D";

static const unsigned int dtmf_phinc[8] = {
    PHINC(1209), PHINC(1336), PHINC(1477), PHINC(1633),
    PHINC(697), PHINC(770), PHINC(852), PHINC(941)
};

#include <QObject>
#include "multimon.h"

class DemodDTMF : public QObject
{
    Q_OBJECT
public:
    explicit DemodDTMF(QObject *parent = 0);
    ~DemodDTMF();

    void demod(float *buffer, int length);
    void reset();

signals:
    void newMessage(const QString &message);

public slots:

private:

    struct demod_state *state;

};

#endif // DEMOD_DTMF_H
