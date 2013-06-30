/*
 *      mainwindow.cpp -- Main application window for AFSK1200 demodulator
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
#include <QAudio>
#include <QtWidgets/QMessageBox>
#include <QTextStream>
#include <QtWidgets/QFileDialog>
#include <QDir>
#include "MultimonDecoder.h"
#include "ui_MultimonDecoder.h"


MultimonDecoder::MultimonDecoder(QWidget *parent) : QWidget(parent), ui(new Ui::MultimonDecoder)
{
    currentDecoder = 0;
    ui->setupUi(this);

    /* select font for text viewer */
#ifdef Q_OS_MAC
    ui->textView->setFont(QFont("Monaco", 12));
#else
    ui->textView->setFont(QFont("Monospace", 9));
#endif

    initializeAudio();
    createDeviceSelector();

    /* create SSI and to toolbar */
/*
    ssiSpacer = new QWidget();
    ssiSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->mainToolBar->addWidget(ssiSpacer);
    ssi = new CSsi(this);
    ui->mainToolBar->addWidget(ssi);

    connect(audioBuffer, SIGNAL(update(qreal)), ssi, SLOT(setLevel(qreal)));
*/
    connect(audioBuffer, SIGNAL(newData(float*,int)), this, SLOT(samplesReceived(float*,int)));

    /** TODO Support multiple decoders here */

    formatMessage("Multimon Initialized...");

    /* initialize decoders */

    afsk12 = new CAfsk12();
    formatMessage("Decoder: AFSK1200 / APRS / AX.25");

    dtmf = new DTMF();
    formatMessage("Decoder: DTMF Decoder");

    // Set Decoder
    currentDecoder = 1;

    //TODO Choose Decoder via dropdown
    if (currentDecoder == 1)
    {
        formatMessage("Mode Selected: APSK1200 / APRS / AX.25");
        connect(afsk12, SIGNAL(newMessage(QString)), this, SLOT(formatMessage(QString)));
    }

    if (currentDecoder == 2)
    {
        formatMessage("Mode Selected: DTMF");
        connect(dtmf, SIGNAL(newMessage(QString)), this, SLOT(formatMessage(QString)));
    }

}

MultimonDecoder::~MultimonDecoder()
{
    if (ui->actionDecode->isChecked()) {
        audioBuffer->stop();
        audioInput->stop();
        delete audioInput;
    }
    delete audioBuffer;

    delete inputLabel;
    delete inputSelector;
    delete ssi;

    /** TODO support multiple decoders here */

    delete afsk12;
    delete dtmf;

    delete ui;

}

void MultimonDecoder::formatMessage(QString message)
{
    //TODO format messages, timestamp, frequency, mode, etc...  LogBook connection?
    /* get current time that will be prepended to packet display */
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateString = dateTime.toString("yyyy.MM.dd hh:mm:ss:zzz");
    message.prepend(" ");
    message.prepend(AVAILABLE_DECODERS[currentDecoder]);
    message.prepend(" ");
    message.prepend(dateString);
    ui->textView->appendPlainText(message);
}

/*! \brief Create device selector widget.
 *
 * The device selector widget is a combo box on the main toolbar
 * listing all available input devices.
 */
void MultimonDecoder::createDeviceSelector()
{

    /* Add audio device selector */
    inputSelector = new QComboBox(this);
    inputSelector->setToolTip(tr("Select audio input device"));
    connect(inputSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(inputSelectionChanged(int)));

    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
    {
         inputSelector->addItem(deviceInfo.deviceName());

         /* store deviceInfo */
         inputDevices.append(deviceInfo);
    }

    inputLabel = new QLabel(tr(" Input:"), this);
    ui->mainToolBar->insertWidget(ui->actionDecode, inputLabel);
    ui->mainToolBar->insertWidget(ui->actionDecode, inputSelector);

    ui->actionDecode->setToolTip(tr("Start decoder"));

}


/*! \brief Initialise audio related data. */
void MultimonDecoder::initializeAudio()
{
    audioFormat.setSampleRate(22050);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleSize(16);
    audioFormat.setSampleType(QAudioFormat::SignedInt);
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setCodec("audio/pcm");

    audioBuffer  = new CAudioBuffer(audioFormat, this);

}


/*! \brief Input selection has changed.
 *  \param index The index to the new selection in the input selector combo box.
 *
 * This slot is activated when the user selects a new input device. The selection can be made
 * both when the decoder is active and when it is idle. If the decoder is active, firs we
 * disable the decoder by toggling the "Decode" action, then we simply enable it by toggling
 * the action again.
 */
void MultimonDecoder::inputSelectionChanged(int index)
{
    Q_UNUSED(index);

    /* check whether decoder is running */
    if (ui->actionDecode->isChecked()) {
        ui->actionDecode->toggle();        // stop decoder
        ui->actionDecode->toggle();        // start decoder
    }

    /* if decoder is not runnign there is nothing to do */

}

/*! \brief Decoder status changed
 *  \param enabled True if the decoder has been enabled, false if it has been disabled.
 */
void MultimonDecoder::on_actionDecode_toggled(bool enabled)
{
    if (enabled) {

        /* check that selected input device supports desired format, if not try nearest */
        QAudioDeviceInfo info(inputDevices.at(inputSelector->currentIndex()));
        if (!info.isFormatSupported(audioFormat)) {
            qWarning() << "Default format not supported - trying to use nearest";
            audioFormat = info.nearestFormat(audioFormat);
        }
/*
        qDebug() << "----------------------------------------------------";
        qDebug() << "Input device: " << inputDevices.at(inputSelector->currentIndex()).deviceName();
        qDebug() << "      Codecs: " << inputDevices.at(inputSelector->currentIndex()).supportedCodecs();
        qDebug() << "    Channels: " << inputDevices.at(inputSelector->currentIndex()).supportedChannelCounts();
        qDebug() << "Sample rates: " << inputDevices.at(inputSelector->currentIndex()).supportedSampleRates();
        qDebug() << "Sample types: " << inputDevices.at(inputSelector->currentIndex()).supportedSampleTypes();
        qDebug() << "Sample sizes: " << inputDevices.at(inputSelector->currentIndex()).supportedSampleSizes();
        qDebug() << " Byte orders: " << inputDevices.at(inputSelector->currentIndex()).supportedByteOrders();
        qDebug() << "----------------------------------------------------";
        qDebug() << "----------------------------------------------------";
        qDebug() << "      Codec: " << audioFormat.codec();
        qDebug() << " Byte order: " << audioFormat.byteOrder();
        qDebug() << "Sample rate: " << audioFormat.sampleRate();
        qDebug() << "Sample size: " << audioFormat.sampleSize();
        qDebug() << "Sample type: " << audioFormat.sampleType();
        qDebug() << "   Channels: " << audioFormat.channelCount();
        qDebug() << "----------------------------------------------------";
*/
        //TODO support multiple widgets here
        /* initialise decoder; looks weird but dmeods were organised in array in multimon */
        afsk12->reset();
        dtmf->reset();

        audioInput = new QAudioInput(inputDevices.at(inputSelector->currentIndex()), audioFormat, this);

        connect(audioInput, SIGNAL(stateChanged(QAudio::State)), SLOT(audioStateChanged(QAudio::State)));
        audioBuffer->start();
        audioInput->start(audioBuffer);

        ui->actionDecode->setToolTip(tr("Stop decoder"));
    }
    else {

        /* stop audio processing */
        audioBuffer->stop();
        audioInput->stop();
        delete audioInput;

        ui->actionDecode->setToolTip(tr("Start decoder"));

        //TODO use regular widget instead of ssi
        /* reset input level indicator */
        ssi->setLevel(0.0);

        currentDecoder = 0;

    }
}


/*! \brief Slot for receiveing new audio samples.
 *  \param data The sample buffer.
 *  \param length The number of samples in the buffer.
 *
 * Calls the afsk1200 decoder.
 */
void MultimonDecoder::samplesReceived(float *buffer, const int length)
{
    int overlap = 18;
    int i;

    for (i = 0; i < length/*-overlap*/; i++) {
        tmpbuf.append(buffer[i]);
    }

    //TODO Support multiple decoders here
    if (currentDecoder == 1)
    {
        //afsk12->demod(tmpbuf.data(), length);
        afsk12->demod(tmpbuf.data(), length);
    }

    if (currentDecoder == 2)
    {
        //afsk12->demod(tmpbuf.data(), length);
        dtmf->demod(tmpbuf.data(), length);
    }

    //TODO Overlap?  Was not working left buffer with junk?
    /* clear tmpbuf and store "overlap" */
    tmpbuf.clear();

    for (i = length-overlap; i < length; i++) {
        tmpbuf.append(buffer[i]);
    }

}


/*! \brief Slot for audio input state changes.
 *  \param state The new state of the audio input.
 */
void MultimonDecoder::audioStateChanged(QAudio::State state)
{
    qDebug() << "Audio state change: " << state << " ERROR: " << audioInput->error();
#ifdef Q_OS_MAC
    /* On OSX audio stops due to underrun when window is minimized */
    if (state == 3) {
        audioBuffer->stop();
        audioBuffer->start();
        audioInput->start(audioBuffer);
    }
#endif
}


/*! \brief Action: Clear the text view.
 *
 * This slot is activated when the user clicks on the Clear button
 * on the main toolbar.
 */
void MultimonDecoder::on_actionClear_triggered()
{
    ui->textView->clear();
}

/*! \brief Action: Save
 *
 * This slot is called when the user activates the File|Save menu item.
 * It asks for a file name, then saves the contents of the text viewer
 * to a plain text file.
 */
void MultimonDecoder::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    QDir::homePath(),
                                                    tr("Text Files (*.txt)"));

    if (fileName.isEmpty()) {
        qDebug() << "Save as cancelled by user";
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error creating file: " << fileName;
        return;
    }

    QTextStream out(&file);
    out << ui->textView->toPlainText();
    file.close();
}


    /*! \brief Action: About Qt
     *
     * This slot is called when the user activates the
     * Help|About Qt menu item (or AFSK Decoder|About Qt on Mac)
     */
/*    void MultimonDecoder::on_actionAboutQt_triggered()
    {
        QMessageBox::aboutQt(this, tr("About Qt"));
    }
*/
