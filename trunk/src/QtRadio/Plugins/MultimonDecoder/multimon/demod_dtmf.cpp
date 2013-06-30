/*
 *      demod_dtmf.c -- DTMF signalling demodulator/decoder
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
#include <QDebug>
#include "multimon.h"
#include "demod_dtmf.h"
#include "filter.h"
#include <math.h>
#include <string.h>

/* ---------------------------------------------------------------------- */

/*
 *
 * DTMF frequencies
 *
 *      1209 1336 1477 1633
 *  697   1    2    3    A
 *  770   4    5    6    B
 *  852   7    8    9    C
 *  941   *    0    #    D
 * 
 */

/* ---------------------------------------------------------------------- */
	
DTMF::DTMF(QObject *parent) :
    QObject(parent)
{
    state = (demod_state *) malloc(sizeof(demod_state));
    reset();
}

DTMF::~DTMF()
{
    free(state);
}

/*! \brief Reset the decoder. */
void DTMF::reset()
{
    memset(&state->l1.dtmf, 0, sizeof(state->l1.dtmf));
}

/* ---------------------------------------------------------------------- */

static int find_max_idx(const float *f)
{
	float en = 0;
	int idx = -1, i;

	for (i = 0; i < 4; i++)
		if (f[i] > en) {
			en = f[i];
			idx = i;
		}
	if (idx < 0)
		return -1;
	en *= 0.1;
	for (i = 0; i < 4; i++)
		if (idx != i && f[i] > en)
			return -1;
	return idx;
}

/* ---------------------------------------------------------------------- */

static inline int process_block(struct demod_state *s)
{
	float tote;
	float totte[16];
	int i, j;

	tote = 0;
	for (i = 0; i < BLOCKNUM; i++)
		tote += s->l1.dtmf.energy[i];
	for (i = 0; i < 16; i++) {
		totte[i] = 0;
		for (j = 0; j < BLOCKNUM; j++)
			totte[i] += s->l1.dtmf.tenergy[j][i];
	}
	for (i = 0; i < 8; i++)
		totte[i] = fsqr(totte[i]) + fsqr(totte[i+8]);
	memmove(s->l1.dtmf.energy+1, s->l1.dtmf.energy, 
		sizeof(s->l1.dtmf.energy) - sizeof(s->l1.dtmf.energy[0]));
	s->l1.dtmf.energy[0] = 0;
	memmove(s->l1.dtmf.tenergy+1, s->l1.dtmf.tenergy, 
		sizeof(s->l1.dtmf.tenergy) - sizeof(s->l1.dtmf.tenergy[0]));
	memset(s->l1.dtmf.tenergy, 0, sizeof(s->l1.dtmf.tenergy[0]));
    tote *= (BLOCKNUM*BLOCKLEN*0.5);  /* adjust for block lengths */
//	verbprintf(10, "DTMF: Energies: %8.5f  %8.5f %8.5f %8.5f %8.5f  %8.5f %8.5f %8.5f %8.5f\n",
//		   tote, totte[0], totte[1], totte[2], totte[3], totte[4], totte[5], totte[6], totte[7]);
    //qDebug() << tote;
	if ((i = find_max_idx(totte)) < 0)
		return -1;
	if ((j = find_max_idx(totte+4)) < 0)
		return -1;
	if ((tote * 0.4) > (totte[i] + totte[j+4]))
		return -1;
	return (i & 3) | ((j << 2) & 0xc);
}

/* ---------------------------------------------------------------------- */

void DTMF::demod(float *buffer, int length)
{
	float s_in;
	int i;
    QString message;

	for (; length > 0; length--, buffer++) {
		s_in = *buffer;
        state->l1.dtmf.energy[0] += fsqr(s_in);
		for (i = 0; i < 8; i++) {
            state->l1.dtmf.tenergy[0][i] += COS(state->l1.dtmf.ph[i]) * s_in;
            state->l1.dtmf.tenergy[0][i+8] += SIN(state->l1.dtmf.ph[i]) * s_in;
            state->l1.dtmf.ph[i] += dtmf_phinc[i];
		}
        if ((state->l1.dtmf.blkcount--) <= 0) {
            state->l1.dtmf.blkcount = BLOCKLEN;
            i = process_block(state);
            if (i != state->l1.dtmf.lastch && i >= 0)
            {
                //verbprintf(0, "DTMF: %c\n", dtmf_transl[i]);
                message = "DTMF: ";
                message.append(QChar(dtmf_transl[i]));
                //qDebug() << message;
                emit newMessage(message);
            }
            state->l1.dtmf.lastch = i;
		}
	}
}
				
/* ---------------------------------------------------------------------- */
