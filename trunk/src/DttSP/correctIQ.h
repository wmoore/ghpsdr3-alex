#ifndef _correctIQ_h
#define _correctIQ_h

#include <bufvec.h>

typedef enum _wbir_state
{
  FastAdapt,
  SlowAdapt,
  NoAdapt,
  JustSayNo
} WBIR_STATE;

typedef struct _iqstate
{
  REAL phase, gain, mu, _mu;
  COMPLEX w;
  WBIR_STATE wbir_state;
} *IQ, iqstate;

extern IQ newCorrectIQ (REAL phase, REAL gain, REAL mu);
extern void delCorrectIQ (IQ iq);
extern void correctIQ (CXB sigbuf, IQ iq, BOOLEAN isTX, int subchan);
#define IQFIRlength 1
#endif
