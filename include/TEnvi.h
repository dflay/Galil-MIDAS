#ifndef TEnvi_h__
#define TEnvi_h__
#include "/home/galil/installation/root/include/TObject.h"

/** This class holds all of the data created by analysis modules.  */

class TEnvi : public TObject{
  public:

  TEnvi();
  virtual ~TEnvi();

 
  Int_t p1val;
  Int_t p2val;
  Int_t p3val;
  Int_t p4val;
  Int_t p5val;
  Int_t p6val;
  


  ClassDef(TEnvi, 1)
};
#endif
