#ifndef EQUALIZER_H
#define EQUALIZER_H

#include <QDialog>
#include <QSettings>
#include "Connection.h"

namespace Ui {
class Equalizer;
}

class Equalizer : public QDialog
{
    Q_OBJECT
    
public:
    explicit Equalizer(Connection *pConn, QWidget *parent = 0);
    ~Equalizer();

    Connection *connection;
    
public slots:
    void resetRx(void);
    void resetTx(void);
    void rxSliderChanged(void);
    void txSliderChanged(void);
    void set3BandEqualizer(void);
    void set10BandEqualizer(void);
    void saveSettings(void);
    void saveSettings3Band(void);
    void loadSettings3Band(void);
    void saveSettings10Band(void);
    void loadSettings10Band(void);

private:
    Ui::Equalizer *ui;

};

#endif // EQUALIZER_H
