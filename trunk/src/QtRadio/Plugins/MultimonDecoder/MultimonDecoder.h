/*
 *      mainwindow.h -- Main application window for AFSK1200 demodulator
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
#ifndef MultimonDecoder_H
#define MultimonDecoder_H

#include <QtWidgets>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QAudioInput>
#include <QList>
#include "AudioBuffer.h"

#include "multimon/DemodAFSK12.h"
#include "multimon/DemodDTMF.h"
#include "multimon/DemodFSK96.h"


namespace Ui {
    class MultimonDecoder;
}

class MultimonDecoder : public QWidget
{
    Q_OBJECT

public:
    explicit MultimonDecoder(QWidget *parent = 0);
    ~MultimonDecoder();

private slots:
    void samplesReceived(float *buffer, const int length);
    void audioStateChanged(QAudio::State state);
    void inputSelectionChanged(int index);
    void updateLevel(qreal);  // updates ui level indicator
    void on_actionDecode_toggled(bool enabled);
    void on_actionClear_clicked();
    void on_actionSave_clicked();

    void formatMessage(QString message);


    void on_selectDecoder_currentIndexChanged(int index);

private:
    Ui::MultimonDecoder *ui;

    int currentDecoder;

    QLabel     *inputLabel;
    QComboBox  *inputSelector;  /*! Audio input delector. */

    QList<QAudioDeviceInfo> inputDevices;  /*! List of available audio input devices. */
    QAudioInput  *audioInput;               /*! Audio input object. */
    QAudioFormat  audioFormat;              /*! Audio format info. */
    CAudioBuffer *audioBuffer;              /*! Audio buffer. */

    DemodAFSK12      *afsk12;
    DemodDTMF         *dtmf;
    DemodFSK96      *fsk96;

    QVarLengthArray<float, 8192> tmpbuf;   /*! Needed to remember "overlap" smples. */

    void createDeviceSelector();
    void initializeAudio();
    void process(QByteArray buff);

    qreal m_alpha;
    void setAlpha(qreal value);
};

#endif // MultimonDecoder_H
