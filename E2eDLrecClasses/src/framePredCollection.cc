#include "E2eDL/E2eDLrecClasses/interface/framePredCollection.h"

framePredCollection::~framePredCollection(){
    frameCollection.clear();
    predCollection.clear();
    IetaSeed=0;
    IphiSeed=0;
  }
