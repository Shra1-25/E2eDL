#ifndef framePredCollection_h
#define framePredCollection_h
# include <iostream>
# include <vector>
# include <cmath>
#include "DataFormats/Common/interface/SortedCollection.h"
using namespace std;
class framePredCollection {
  
  std::vector<std::vector<float>> frameCollection;
  std::vector<float> predCollection;
  float IetaSeed;
  float IphiSeed;
  std::vector<std::vector<float>> id_;
  
  public:
  typedef std::vector<std::vector<float>> key_type;
  //framePredCollection();
  ~framePredCollection();
  
  std::vector<std::vector<float>> id() const {return id_;}
  
  std::vector<std::vector<float>> getFrameCollection(){
    return frameCollection;
  }
  
  void putFrameCollection(std::vector<std::vector<float>> frame){
    frameCollection=frame;
  }
  
  std::vector<float> getPredCollection(){
    return predCollection;
  }
  
  void putPredCollection(std::vector<float> prediction){
    predCollection=prediction;
  }
  
  void putIetaSeed (float Ieta_seed){
    IetaSeed = Ieta_seed;
  }
  
  void putIphiSeed ( float Iphi_seed){
    IphiSeed = Iphi_seed;
  }
  
  float getIetaSeed (){
    return IetaSeed;
  }
  
  float getIphiSeed (){
    return IphiSeed;
  }
};
//typedef edm::SortedCollection<framePredCollection,edm::StrictWeakOrdering<framePredCollection>> framePredSeedCollection;
typedef edm::SortedCollection<framePredCollection> framePredSeedCollection;
#endif
