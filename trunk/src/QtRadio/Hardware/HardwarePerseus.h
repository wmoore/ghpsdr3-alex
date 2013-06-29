#if !defined __HARDWARE_PERSEUS_H__
#define      __HARDWARE_PERSEUS_H__

#include "Hardware.h"

class HardwarePerseus: public Hardware
{ 
   Q_OBJECT

public:
   HardwarePerseus (Connection *pC, QWidget *p);
   ~HardwarePerseus ();
private:
   QSignalMapper *attMapper;
private slots:
   void attClicked(int state);
   void ditherChanged(int);
   void preampChanged(int);

   void processAnswer (QStringList);
};

#endif

