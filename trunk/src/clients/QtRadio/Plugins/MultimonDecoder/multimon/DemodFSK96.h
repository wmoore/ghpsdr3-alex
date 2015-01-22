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
#ifndef DEMOD_FSK96_H
#define DEMOD_FSK96_H

#include <QObject>
#include "multimon.h"
#include "HDLC.h"

/* ---------------------------------------------------------------------- */

#define FREQ_SAMP  22050
#define BAUD       9600

/* ---------------------------------------------------------------------- */

#define DESCRAM_TAP1 0x20000
#define DESCRAM_TAP2 0x01000
#define DESCRAM_TAP3 0x00001

#define DESCRAM_TAPSH1 17
#define DESCRAM_TAPSH2 12
#define DESCRAM_TAPSH3 0

#define SCRAM_TAP1 0x20000 /* X^17 */
#define SCRAM_TAPN 0x00021 /* X^0+X^5 */

/* --------------------------------------------------------------------- */

#define FILTLEN 24
#define UPSAMP 3

static const float inp_filt[3][24] = {
        {  0.000440, -0.001198, -0.000493,  0.003648,
          -0.000630, -0.008433,  0.005567,  0.015557,
          -0.019931, -0.026514,  0.079822,  0.181779,
           0.124956, -0.002471, -0.032062,  0.008024,
           0.012568, -0.006559, -0.004235,  0.003711,
           0.000909, -0.001520, -0.000018,  0.000709},
        {  0.000686, -0.000618, -0.001332,  0.002494,
           0.002258, -0.007308, -0.001538,  0.016708,
          -0.004897, -0.035748,  0.034724,  0.161417,
           0.161417,  0.034724, -0.035748, -0.004897,
           0.016708, -0.001538, -0.007308,  0.002258,
           0.002494, -0.001332, -0.000618,  0.000686},
        {  0.000709, -0.000018, -0.001520,  0.000909,
           0.003711, -0.004235, -0.006559,  0.012568,
           0.008024, -0.032062, -0.002471,  0.124956,
           0.181779,  0.079822, -0.026514, -0.019931,
           0.015557,  0.005567, -0.008433, -0.000630,
           0.003648, -0.000493, -0.001198,  0.000440}
};

#define SPHASEINC (0x10000u*BAUD/FREQ_SAMP/UPSAMP)

/* ---------------------------------------------------------------------- */

class DemodFSK96 : public QObject
{
    Q_OBJECT
public:
    explicit DemodFSK96(QObject *parent = 0);
    ~DemodFSK96();

    void demod(float *buffer, int length);
    void reset();

signals:
    void newMessage(const QString &message);

public slots:

private:
    struct demod_state *state;
    HDLC hdlc;
};

#endif // DEMOD_FSK96_H
