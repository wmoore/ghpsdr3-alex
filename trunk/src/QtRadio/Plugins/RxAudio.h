#ifndef RXAUDIO_H
#define RXAUDIO_H

#include <QWidget>

namespace Ui {
class RxAudio;
}

class RxAudio : public QWidget
{
    Q_OBJECT
    
public:
    explicit RxAudio(QWidget *parent = 0);
    ~RxAudio();
    
private:
    Ui::RxAudio *ui;
};

#endif // RXAUDIO_H
