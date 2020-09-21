#ifndef RecoE2E_FrameCropping_h
#define RecoE2E_FrameCropping_h

#include <iostream>
#include <vector>
#include <cassert>

#include "E2eDL/DataFormats/interface/FrameCollections.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

namespace e2e {

  void getFrame( e2e::Frame2D&, const e2e::seed&, const e2e::Frame1D*, int, int );

}
#endif
