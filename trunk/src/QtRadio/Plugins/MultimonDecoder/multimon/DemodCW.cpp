#include "DemodCW.h"

    /* wiggle - morse decoder from soundcard input.
     * --
     * Copyright 2005 Chris Baird <cjb@brushtail.apana.org.au>
     * Licenced according to the GPL V2.
     * --
     * gcc -o wiggle wiggle.c
     * --
     * TODO:
     * command line args:
     * -w nnn: word-wrap after the nnnth column
     * -w nnn: expected wpm?
     * -m    : measure mode; print run timings, etc., instead of letters.
     */

    #include <stdio.h>		/* EOF, etc. */
    #include <unistd.h>		/* read() */
    #include <string.h>		/* strcmp() */
    #include <stdlib.h>		/* exit(), calloc() */
    #include "DemodCW.h"
    #include <QDebug>

    const char *morse[] =
      {
        "A", ".-",  "B", "-...", "C", "-.-.", "D", "-..",  "E", ".", "F", "..-.",
        "G", "--.", "H", "....", "I", "..",   "J", ".---", "K", "-.-", "L", ".-..",
        "M", "--",  "N", "-.",   "O", "---",  "P", ".--.", "Q", "--.-", "R", ".-.",
        "S", "...", "T", "-",    "U", "..-",  "V", "...-", "W", ".--", "X", "-..-",
        "Y", "-.--","Z", "--..",
        "0", "-----", "1", ".----", "2", "..---", "3", "...--", "4", "....-",
        "5", ".....", "6", "-....", "7", "--...", "8", "---..", "9", "----.",
        ".", ".-.-.-",  ",", "--..--", "?", "..--..", "(", "-.--.",
        ")", "-.--.-",  ":", "---...", "/", "-..-.",  "'", ".----.",
        "\"", ".-..-.", "-", "-....-", "+", ".-.-.",  "=", "-...-",
        "?", "?"
      };

    DemodCW::DemodCW(QObject *parent) :
        QObject(parent)
    {
        state = (demod_state *) malloc(sizeof(demod_state));
        reset();
    }

    DemodCW::~DemodCW()
    {
        free(state);
    }

    /*! \brief Reset the decoder. */
    void DemodCW::reset()
    {
        memset(&state->l1.demodcw, 0, sizeof(state->l1.demodcw));
        state->l1.demodcw.lbit = 0;
        state->l1.demodcw.pass = 0;
        state->l1.demodcw.run = 1;
    }

    void DemodCW::demod(float *buffer, int length)
    {
      int i, d, bp=0, bit, nsamp, ditlength, range;
      int *subsamp, s, tt=0, thres, run1=1;

      char line[80], *lp;
      QString message;

      lp = line;
      /* gives a rough number of measurements needed to make SUBSAMP samples
       * per dit. apparently there's 5/12 dits per second at 1 WPM */
      nsamp = (SAMP*5) / (EST_WPM*12*SUBSAMP);
      ditlength = SUBSAMP;

      range = (8*12*EST_WPM*SUBSAMP) / 5; /* 8 seconds' worth */
      subsamp = (int*)calloc (range, sizeof(int));

      /* main decoding loop */
      for (; length > 0; length--, buffer++) {
          //s_in = *buffer;
          for (s = 0, i=0; i < nsamp; i++)
            s += *buffer;
          tt = tt + s - subsamp[bp];
          subsamp[bp] = s;
          bp = (bp+1) % range;

          /* wait until we have an idea of the average signal level */
          if (++state->l1.demodcw.pass < range)
        {
          //fprintf (stderr, "%d \r", range-pass);
          qDebug() << "Range pass";
          continue;
        }

          thres = tt / range;
          bit = (10*s) > (12*thres) ? 1 : 0;

          if (bit == state->l1.demodcw.lbit)
        {
          state->l1.demodcw.run++;
          //qDebug() << "run++";
          continue;
        }

          d = (10*state->l1.demodcw.run) / ditlength;

          if (state->l1.demodcw.lbit == 1)
        {
          if ((d > 5) && (d < 20))
            *lp++ = '.';
          if ((d >= 20) && (d < 40))
            *lp++ = '-';
          *lp = '\0';
          run1 = state->l1.demodcw.run;
          if (lp == &line[79]) lp--; /* prevent overflow */
        }

          if ((state->l1.demodcw.lbit == 0) && (lp != line))
        {
          if (d >= 20)
            {
              for (i=0; *morse[i+1] != '?'; i += 2)
            if (strcmp (morse[i+1], line) == 0)
              break;
              if (*morse[i+1] == '?')
               {
                  message.append(line);
                  //qDebug() << message;
              }
              else
              {
                  message.append(*morse[i]);
                  //qDebug() << message;
              }
              if (d > 80)
              {
                  message.append(" ");
                  //qDebug() << message;
                  emit newMessage(message);
                  //fflush (stdout);
              }
              lp = line;
            }
          /* check if a "dah-do" */
          if ((run1 > 2) && ((run1) > (2*state->l1.demodcw.run)))
            {
              d = (state->l1.demodcw.run+run1) / 4;
              if ((d > (SUBSAMP/2)) &&
              ((3*d) > (2*ditlength)) && ((2*d) < (3*ditlength)))
            {
              ditlength = d;
              //fprintf (stderr, "<%d>",ditlength);
              qDebug() << ditlength;
            }
              run1=0;
            }
        }
          state->l1.demodcw.lbit = bit;
          state->l1.demodcw.run = 1;
        }

}
