#ifndef DEMODCW_H
#define DEMODCW_H

#include <QObject>
#include "multimon.h"

#define SAMP 22050
#define SUBSAMP 6
#define EST_WPM (10.0)

class DemodCW : public QObject
{
    Q_OBJECT
public:
    explicit DemodCW(QObject *parent = 0);
    ~DemodCW();

    void demod(float *buffer, int length);
    void reset();

signals:
    void newMessage(const QString &message);

public slots:
    
private:
    //int get_sample(float *buffer, int length);
    struct demod_state *state;

};

#endif // DEMODCW_H
