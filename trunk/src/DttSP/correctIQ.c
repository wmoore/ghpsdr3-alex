/* correctIQ.c

This routine restores quadrature between arms of an analytic signal
possibly distorted by ADC hardware.

This file is part of a program that implements a Software-Defined Radio.

Copyright (C) 2004, 2005, 2006 by Frank Brickle, AB2KT and Bob McGwier, N4HY

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

The authors can be reached by email at

ab2kt@arrl.net
or
rwmcgwier@comcast.net

or by paper mail at

The DTTS Microwave Society
6 Kathleen Place
Bridgewater, NJ 08807
*/

#include <common.h>

IQ newCorrectIQ (REAL phase, REAL gain, REAL mu) {
  IQ iq = (IQ) safealloc (1, sizeof (iqstate), "IQ state");
  iq->phase = phase;
  iq->gain = gain;
  iq->mu = mu;
  iq->_mu = 1.0;
  iq->w = (COMPLEX){0.0, 0.0};
  return iq;
}

void delCorrectIQ (IQ iq) {
  safefree ((char *) iq);
}

int IQdoit = 1;

void correctIQ (CXB sigbuf, IQ iq, BOOLEAN isTX, int subchan) {
  int i;
  REAL mu;
  
  if (IQdoit == 0) return;
  if (subchan == 0) mu = iq->_mu * iq->mu;
  else mu = 0;
  if(!isTX) {
    // test for blow up
    if (iq->w.re != iq->w.re ||	abs(iq->w.re) > 1 ||
	iq->w.im != iq->w.im || abs(iq->w.im) > 1)
      iq->w = (COMPLEX){0.0, 0.0};
    if (mu == 0) {
      // the sub channels somehow share the value of w
      // with the actively correcting channel,
      // so we simply apply the correction
      for (i = 0; i < CXBhave (sigbuf); i++) {
	// output = input + w * input * conj(input);
	CXBdata(sigbuf, i) = Cadd(CXBdata(sigbuf, i), Cmul(iq->w, Cmul(CXBdata(sigbuf, i), Conjg(CXBdata(sigbuf, i)))));
      }
    } else {
      // one pass to establish average error signal at current w
      COMPLEX sum_error = (COMPLEX){0.0, 0.0};
      for (i = 0; i < CXBhave (sigbuf); i++) {
	// y = input + w * input * conj(input);
	COMPLEX y = Cadd(CXBdata(sigbuf, i), Cmul(iq->w, Cmul(CXBdata(sigbuf, i), Conjg(CXBdata(sigbuf, i)))));
	// sum_error += y * y;
	sum_error = Cadd(sum_error, Cmul(y, y));
      }
      COMPLEX avg_error = Cscl(sum_error, 1.0/CXBhave (sigbuf));
      REAL max_abs = max(abs(avg_error.re), abs(avg_error.im));
      // test for learning rate too large
      // defined as moving more than 1 millionth of 
      if (max_abs * iq->_mu > 1e-6) {
	iq->_mu = 1e-6 / max_abs;
	mu = iq->_mu * iq->mu;
      }
      // another pass to train the filter and produce the output
      for (i = 0; i < CXBhave (sigbuf); i++) {
	// y = input + w * input * conj(input);
	COMPLEX y = Cadd(CXBdata(sigbuf, i), Cmul(iq->w, Cmul(CXBdata(sigbuf, i), Conjg(CXBdata(sigbuf, i)))));
	// w -= mm * y * y;
	iq->w = Csub(iq->w, Cscl(Cmul(y, y), mu));
	// output = y;
	CXBdata(sigbuf, i) = y;
      }
      //fprintf(stderr, "w1 real: %g, w1 imag: %g\n", iq->w.re, iq->w.im); fflush(stderr); 
    }
  } else {
    // apply a fixed phase and gain correction for transmit
    for (i = 0; i < CXBhave (sigbuf); i++) {
      CXBimag (sigbuf, i) += iq->phase * CXBreal (sigbuf, i);
      CXBreal (sigbuf, i) *= iq->gain;
    }
  }
}
