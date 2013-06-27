#if !defined __HARDWARE_SDRIQ_H__
#define      __HARDWARE_SDRIQ_H__

#include "Hardware.h"

class HardwareSdriq: public Hardware
{ 
   Q_OBJECT

public:
   HardwareSdriq (Connection *pC, QWidget *p);
   ~HardwareSdriq ();
private:
   QSignalMapper *attMapper;
private slots:
   void attClicked(int state);
   void processAnswer (QStringList);
};

#endif

