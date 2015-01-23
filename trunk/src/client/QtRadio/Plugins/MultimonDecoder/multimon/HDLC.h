#ifndef HDLC_H
#define HDLC_H

#include <QObject>

class HDLC : public QObject
{
    Q_OBJECT
public:
    explicit HDLC(QObject *parent = 0);
    ~HDLC();

    void hdlc_init(struct demod_state *state);
    QString hdlc_rxbit(struct demod_state *state, int bit);

signals:

public slots:

private:
    QString ax25_disp_packet(unsigned char *bp, unsigned int len);
    QString aprs_print_ax25call(unsigned char *call, int is_repeater);
    QString aprs_disp_packet(unsigned char *bp, unsigned int len);

};

#endif // HDLC_H
