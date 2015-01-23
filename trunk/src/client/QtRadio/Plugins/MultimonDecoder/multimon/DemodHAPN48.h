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
#ifndef DEMOD_HAPN48_H
#define DEMOD_HAPN48_H

#include <QObject>
#include "multimon.h"
#include "HDLC.h"

#define FREQ_SAMP  22050
#define BAUD       4800

#define SPHASEINC (0x10000u*BAUD/FREQ_SAMP)

class DemodHAPN48 : public QObject
{
    Q_OBJECT
public:
    explicit DemodHAPN48(QObject *parent = 0);
    ~DemodHAPN48();

    void demod(float *buffer, int length);
    void reset();

signals:
    void newMessage(const QString &message);

public slots:

private:
    struct demod_state *state;
    HDLC hdlc;
};

#endif // DEMOD_HAPN48_H
