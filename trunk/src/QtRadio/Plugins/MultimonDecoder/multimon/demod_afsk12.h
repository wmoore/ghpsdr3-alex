/*
 *      cafsk12.h -- AFSK1200 demodulator class
 *
 *      Copyright (C) 1996
 *          Thomas Sailer (sailer@ife.ee.ethz.ch, hb9jnx@hb9w.che.eu)
 *
 *      Copyright (C) 2011 Alexandru Csete (oz9aec at gmail.com)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef CAFSK12_H
#define CAFSK12_H

#include <QObject>
#include "multimon.h"

/*
 * Standard TCM3105 clock frequency: 4.4336MHz
 * Mark frequency: 2200 Hz
 * Space frequency: 1200 Hz
 */
#define FREQ_MARK  1200
#define FREQ_SPACE 2200
#define FREQ_SAMP  22050
#define BAUD       1200
#define SUBSAMP    2

#define CORRLEN ((int)(FREQ_SAMP/BAUD))
#define SPHASEINC (0x10000u*BAUD*SUBSAMP/FREQ_SAMP)

class CAfsk12 : public QObject
{
    Q_OBJECT
public:
    explicit CAfsk12(QObject *parent = 0);
    ~CAfsk12();

    void demod(float *buffer, int length);
    void reset();

signals:
    void newMessage(const QString &message);

public slots:

private:
    float corr_mark_i[CORRLEN];
    float corr_mark_q[CORRLEN];
    float corr_space_i[CORRLEN];
    float corr_space_q[CORRLEN];

    struct demod_state *state;

    /* HDLC functions */
    void hdlc_init(struct demod_state *s);
    void hdlc_rxbit(struct demod_state *s, int bit);
    void verbprintf(int verb_level, const char *fmt, ...);
    void ax25_disp_packet(unsigned char *bp, unsigned int len);
};

#endif // CAFSK12_H
