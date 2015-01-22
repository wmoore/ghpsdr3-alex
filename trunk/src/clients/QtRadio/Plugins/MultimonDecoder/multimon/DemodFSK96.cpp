/*
 *      demod_fsk96.c -- FSK 9600 baud demodulator (G3RUH)
 *
 *      Copyright (C) 1996  
 *          Thomas Sailer (sailer@ife.ee.ethz.ch, hb9jnx@hb9w.che.eu)
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

/* ---------------------------------------------------------------------- */

#include "multimon.h"
#include "filter.h"
#include <math.h>
#include <string.h>
#include "HDLC.h"
#include "DemodFSK96.h"
#include <QDebug>

/* ---------------------------------------------------------------------- */

DemodFSK96::DemodFSK96(QObject *parent) :
    QObject(parent)
{
    state = (demod_state *) malloc(sizeof(demod_state));
    reset();
}

DemodFSK96::~DemodFSK96()
{
    free(state);
}

void DemodFSK96::reset()
{
    hdlc.hdlc_init(state);
    memset(&state->l1.fsk96, 0, sizeof(state->l1.fsk96));
}

/* ---------------------------------------------------------------------- */

void DemodFSK96::demod(float *buffer, int length)
{
	float f;
	unsigned char curbit;
	int i;
    unsigned int descx;

	for (; length > 0; length--, buffer++) {
		for (i = 0; i < UPSAMP; i++) {
			f = mac(buffer, inp_filt[i], FILTLEN);
            state->l1.fsk96.dcd_shreg <<= 1;
            state->l1.fsk96.dcd_shreg |= (f > 0);
            //verbprintf(10, "%c", '0'+(state->l1.fsk96.dcd_shreg & 1));
			/*
			 * check if transition
			 */
            if ((state->l1.fsk96.dcd_shreg ^ (state->l1.fsk96.dcd_shreg >> 1)) & 1) {
                if (state->l1.fsk96.sphase < (0x8000u-(SPHASEINC/2)))
                    state->l1.fsk96.sphase += SPHASEINC/8;
				else
                    state->l1.fsk96.sphase -= SPHASEINC/8;
			}
            state->l1.fsk96.sphase += SPHASEINC;
            if (state->l1.fsk96.sphase >= 0x10000u) {
                state->l1.fsk96.sphase &= 0xffffu;
                state->l1.fsk96.descram <<= 1;
                state->l1.fsk96.descram |= state->l1.fsk96.dcd_shreg & 1;
                descx = state->l1.fsk96.descram ^ (state->l1.fsk96.descram >> 1);
				curbit = ((descx >> DESCRAM_TAPSH1) ^ (descx >> DESCRAM_TAPSH2) ^
					  (descx >> DESCRAM_TAPSH3) ^ 1) & 1;
                //verbprintf(9, " %c ", '0'+curbit);
                //qDebug() << QChar(0+curbit);
                QString message =  hdlc.hdlc_rxbit(state, curbit);
                if(message.length() > 0) {
                    qDebug() << message;
                    emit newMessage(message);
                }
			}
		}
	}
}
