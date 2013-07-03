/*
 *      cafsk12.cpp -- AFSK1200 demodulator class
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
#include <QDebug>
#include <QTime>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include "multimon.h"
#include "filter.h"
#include "DemodAFSK12.h"

DemodAFSK12::DemodAFSK12(QObject *parent) :
    QObject(parent)
{
    state = (demod_state *) malloc(sizeof(demod_state));
    reset();
}

DemodAFSK12::~DemodAFSK12()
{
    free(state);
}

/*! \brief Reset the decoder. */
void DemodAFSK12::reset()
{
    float f;
    int i;

    hdlc_init(state);
    memset(&state->l1.afsk12, 0, sizeof(state->l1.afsk12));
    for (f = 0, i = 0; i < CORRLEN; i++) {
        corr_mark_i[i] = cos(f);
        corr_mark_q[i] = sin(f);
        f += 2.0*M_PI*FREQ_MARK/FREQ_SAMP;
    }
    for (f = 0, i = 0; i < CORRLEN; i++) {
        corr_space_i[i] = cos(f);
        corr_space_q[i] = sin(f);
        f += 2.0*M_PI*FREQ_SPACE/FREQ_SAMP;
    }

}


void DemodAFSK12::demod(float *buffer, int length)
{
    float f;
    unsigned char curbit;

    if (state->l1.afsk12.subsamp) {
        int numfill = SUBSAMP - state->l1.afsk12.subsamp;
        if (length < numfill) {
            state->l1.afsk12.subsamp += length;
            return;
        }
        buffer += numfill;
        length -= numfill;
        state->l1.afsk12.subsamp = 0;
    }
    for (; length >= SUBSAMP; length -= SUBSAMP, buffer += SUBSAMP) {
        f = fsqr(mac(buffer, corr_mark_i, CORRLEN)) +
            fsqr(mac(buffer, corr_mark_q, CORRLEN)) -
            fsqr(mac(buffer, corr_space_i, CORRLEN)) -
            fsqr(mac(buffer, corr_space_q, CORRLEN));
        state->l1.afsk12.dcd_shreg <<= 1;
        state->l1.afsk12.dcd_shreg |= (f > 0);
        //verbprintf(10, "%c", '0'+(state->l1.afsk12.dcd_shreg & 1));
        /*
         * check if transition
         */
        if ((state->l1.afsk12.dcd_shreg ^ (state->l1.afsk12.dcd_shreg >> 1)) & 1) {
            if (state->l1.afsk12.sphase < (0x8000u-(SPHASEINC/2)))
                state->l1.afsk12.sphase += SPHASEINC/8;
            else
                state->l1.afsk12.sphase -= SPHASEINC/8;
        }
        state->l1.afsk12.sphase += SPHASEINC;
        if (state->l1.afsk12.sphase >= 0x10000u) {
            state->l1.afsk12.sphase &= 0xffffu;
            state->l1.afsk12.lasts <<= 1;
            state->l1.afsk12.lasts |= state->l1.afsk12.dcd_shreg & 1;
            curbit = (state->l1.afsk12.lasts ^
                  (state->l1.afsk12.lasts >> 1) ^ 1) & 1;
            //verbprintf(9, " %c ", '0'+curbit);
            hdlc_rxbit(state, curbit);
        }
    }
    state->l1.afsk12.subsamp = length;
}
