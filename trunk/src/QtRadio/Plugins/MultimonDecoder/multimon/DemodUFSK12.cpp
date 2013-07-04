/*
 *      demod_ufsk12.c -- 1200 baud FSK demodulator
 *
 *      Copyright (C) 2007
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
#include "DemodUFSK12.h"

DemodUFSK12::DemodUFSK12(QObject *parent) :
    QObject(parent)
{
    state = (demod_state *) malloc(sizeof(demod_state));
    reset();
}

DemodUFSK12::~DemodUFSK12()
{
    free(state);
}

/*! \brief Reset the decoder. */
void DemodUFSK12::reset()
{
    float f;
    int i;

    // TODO uart_init(s);
    memset(&state->l1.ufsk12, 0, sizeof(state->l1.ufsk12));
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

/* ---------------------------------------------------------------------- */

void DemodUFSK12::demod(float *buffer, int length)
{
	float f;
	unsigned char curbit;

    if (state->l1.ufsk12.subsamp) {
        int numfill = SUBSAMP - state->l1.ufsk12.subsamp;
		if (length < numfill) {
            state->l1.ufsk12.subsamp += length;
			return;
		}
		buffer += numfill;
		length -= numfill;
        state->l1.ufsk12.subsamp = 0;
	}
	for (; length >= SUBSAMP; length -= SUBSAMP, buffer += SUBSAMP) {
		f = 	fsqr(mac(buffer, corr_mark_i, CORRLEN)) +
			fsqr(mac(buffer, corr_mark_q, CORRLEN)) -
			fsqr(mac(buffer, corr_space_i, CORRLEN)) -
			fsqr(mac(buffer, corr_space_q, CORRLEN));
        state->l1.ufsk12.dcd_shreg <<= 1;
        state->l1.ufsk12.dcd_shreg |= (f > 0);
        //verbprintf(10, "%c", '0'+(state->l1.ufsk12.dcd_shreg & 1));
		/*
		 * check if transition
		 */
        if ((state->l1.ufsk12.dcd_shreg ^ (state->l1.ufsk12.dcd_shreg >> 1)) & 1) {
            if (state->l1.ufsk12.sphase < (0x8000u-(SPHASEINC/2)))
                state->l1.ufsk12.sphase += SPHASEINC/8;
			else
                state->l1.ufsk12.sphase -= SPHASEINC/8;
		}
        state->l1.ufsk12.sphase += SPHASEINC;
        if (state->l1.ufsk12.sphase >= 0x10000u) {
            state->l1.ufsk12.sphase &= 0xffffu;
            curbit = state->l1.ufsk12.dcd_shreg & 1;
            //verbprintf(9, " %c ", '0'+curbit);
            //TODO uart_rxbit(s, curbit);
		}
	}
    state->l1.ufsk12.subsamp = length;
}
