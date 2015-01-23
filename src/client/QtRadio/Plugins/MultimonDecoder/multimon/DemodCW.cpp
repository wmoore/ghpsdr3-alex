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
        state->l1.demodcw.bp = 0;
        state->l1.demodcw.tt = 0;
        state->l1.demodcw.run1 = 1;

        state->l1.demodcw.nsamp = (SAMP*5) / (EST_WPM*12*SUBSAMP);
        state->l1.demodcw.ditlength = SUBSAMP;
        state->l1.demodcw.range = (8*12*EST_WPM*SUBSAMP) / 5; /* 8 seconds' worth */
        /* gives a rough number of measurements needed to make SUBSAMP samples
         * per dit. apparently there's 5/12 dits per second at 1 WPM */
        state->l1.demodcw.subsamp = (int*)calloc (state->l1.demodcw.range, sizeof(int));
        qDebug()<< state->l1.demodcw.nsamp;
    }

    void DemodCW::demod(float *buffer, int length)
    {
        //OK
        int i;
        int c;
        QString message = "";

        /* main decoding loop */
           while(length > 0) {
                buffer++;
                length--;
                c = *buffer;
                if (c > 32767) c = 65536 - c;
                state->l1.demodcw.s += c;
            }

        while(state->l1.demodcw.s > state->l1.demodcw.nsamp) {
            --state->l1.demodcw.s;

            state->l1.demodcw.tt = state->l1.demodcw.tt + state->l1.demodcw.s - state->l1.demodcw.subsamp[state->l1.demodcw.bp];
            state->l1.demodcw.subsamp[state->l1.demodcw.bp] = state->l1.demodcw.s;
            state->l1.demodcw.bp = (state->l1.demodcw.bp+1) % state->l1.demodcw.range;

            /* wait until we have an idea of the average signal level */
            if (++state->l1.demodcw.pass <= state->l1.demodcw.range)
            {
                qDebug() << "Range pass";
                continue;
            }

            state->l1.demodcw.thres = state->l1.demodcw.tt / state->l1.demodcw.range;
            state->l1.demodcw.bit = (10*state->l1.demodcw.s) > (12*state->l1.demodcw.thres) ? 1 : 0;

            if (state->l1.demodcw.bit == state->l1.demodcw.lbit)
            {
                state->l1.demodcw.run++;
                //qDebug() << "run++";
                continue;
            }

            state->l1.demodcw.d = (10*state->l1.demodcw.run) / state->l1.demodcw.ditlength;

            if (state->l1.demodcw.lbit == 1)
            {
                if ((state->l1.demodcw.d > 5) && (state->l1.demodcw.d < 20))
                {
                    *state->l1.demodcw.lp++ = '.';
                    qDebug() << "Dot";
                }

                if ((state->l1.demodcw.d >= 20) && (state->l1.demodcw.d < 40))
                {
                    *state->l1.demodcw.lp++ = '-';
                    qDebug() << "Dit";
                }
                //*state->l1.demodcw.lp = '\0';
                state->l1.demodcw.run1 = state->l1.demodcw.run;
                if (state->l1.demodcw.lp == &state->l1.demodcw.line[79]) state->l1.demodcw.lp--; /* prevent overflow */
            }

            if ((state->l1.demodcw.lbit == 0) && (state->l1.demodcw.lp != state->l1.demodcw.line))
            {
                if (state->l1.demodcw.d >= 20)
                {
                    for (i=0; *morse[i+1] != '?'; i += 2)
                        if (strcmp (morse[i+1], state->l1.demodcw.line) == 0)
                            break;
                    if (*morse[i+1] == '?')
                    {
                        message.append(state->l1.demodcw.line);
                        qDebug() << message;
                    }
                    else
                    {
                        message.append(*morse[i]);
                        qDebug() << message;
                    }
                    if (state->l1.demodcw.d > 80)
                    {
                        message.append(" ");
                        qDebug() << message;
                        emit newMessage(message);
                    }
                    state->l1.demodcw.lp = state->l1.demodcw.line;
                }
                /* check if a "dah-do" */
                if ((state->l1.demodcw.run1 > 2) && ((state->l1.demodcw.run1) > (2*state->l1.demodcw.run)))
                {
                    state->l1.demodcw.d = (state->l1.demodcw.run+state->l1.demodcw.run1) / 4;
                    if ((state->l1.demodcw.d > (SUBSAMP/2)) &&
                            ((3*state->l1.demodcw.d) > (2*state->l1.demodcw.ditlength)) && ((2*state->l1.demodcw.d) < (3*state->l1.demodcw.ditlength)))
                    {
                        state->l1.demodcw.ditlength = state->l1.demodcw.d;
                        qDebug() << state->l1.demodcw.ditlength;
                    }
                    state->l1.demodcw.run1=0;
                }
            }
            state->l1.demodcw.lbit = state->l1.demodcw.bit;
            state->l1.demodcw.run = 1;
        }

    }
