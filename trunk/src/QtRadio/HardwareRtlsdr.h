#if !defined __HARDWARE_RTLSDR_H__
#define      __HARDWARE_RTLSDR_H__

#include "Hardware.h"

class HardwareRtlsdr: public Hardware
{ 
   Q_OBJECT

public:
   HardwareRtlsdr (Connection *pC, QWidget *p);
   ~HardwareRtlsdr ();

private:
   QSignalMapper *attMapper;
   int attenuatorVal;

private slots:
   void attClicked(int state);
   void processAnswer (QStringList);
};
            

#endif

