/*#include "ProdTutorial/ProducerTest/plugins/DetImgProducer.h"
#include "ProdTutorial/ProducerTest/plugins/EGProducer.h"
#include "PhysicsTools/TensorFlow/interface/TensorFlow.h"
#include "ProdTutorial/ProducerTest/plugins/predict_tf.h"*/
#include "E2eDL/E2eDLrec/plugins/DetImgProducer.h"
#include "E2eDL/E2eDLrec/plugins/EGProducer.h"
#include "PhysicsTools/TensorFlow/interface/TensorFlow.h"
#include "E2eDL/E2eDLrec/plugins/predict_tf.h"
//#include <fstream>

EGProducer::EGProducer(const edm::ParameterSet& iConfig)
{
 //vEB_photon_frames = consumes<std::vector<std::vector<float>>>(iConfig.getParameter<edm::InputTag>("frames_"));
 EBRecHitCollectionT_    = consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("reducedEBRecHitCollection"));
 photonCollectionT_ = consumes<PhotonCollection>(iConfig.getParameter<edm::InputTag>("photonCollection"));
 vEB_energy_token = consumes<std::vector<float>>(iConfig.getParameter<edm::InputTag>("EBEnergy"));
 
 edm::Service<TFileService> fs;
 EGTree = fs->make<TTree>("EGTree", "RecHit tree");
 branchesPhotonSel ( EGTree, fs );
 
 //modelName = iConfig.getParameter<std::string>("EGModelName");
 
 produces<std::vector<float>> ("EBenergyClass");
 produces<EB_photonFrames> ("photonFramePredSeedCollection");
 //produces<trialCollection> ("trialCollection");
}
  
 EGProducer::~EGProducer()
{
 
}

void
EGProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   std::cout<<" >> Running EGProducer."<<std::endl;
   using namespace edm;
   nTotal++;

   edm::Handle<std::vector<float>> vEB_energy_handle;
   iEvent.getByToken(vEB_energy_token, vEB_energy_handle);
  
   vEB_energy_=*vEB_energy_handle;
   vEB_flat_frame.clear();
   vEB_frame.clear();
   //vpredictions.clear();   
   EB_photonFrames vEB_photonFrames;
 
   //producing cropped frames, getting photon seeds and running inference for getting their prediction
   get_photons(iEvent, iSetup, vEB_photonFrames );//stored in vEB_frames vectors

   // Code (Commented below) to verify photonFrameJetCollection Branch of edm root file.
   /*std::cout<<"Current size of photon jet collection: "<<vEB_photonFrames.size()<<std::endl;
   std::vector<float> seedx = vEB_photonFrames[vEB_photonFrames.size()-1].getIetaSeeds();
   std::vector<float> seedy = vEB_photonFrames[vEB_photonFrames.size()-1].getIphiSeeds();
   std::cout<<" >> Class Object Seeds are: ";
   for (int seed_idx=0;seed_idx<int(seedx.size());seed_idx++){
    std::cout<<"["<<seedx[seed_idx]<<", "<<seedy[seed_idx]<<"], ";
   }
   std::cout<<std::endl;
   std::vector<std::vector<float>> temp_flat=vEB_photonFrames[vEB_photonFrames.size()-1].getFrameCollection();
   
   for (int seedidx=0;seedidx<int(temp_flat.size());seedidx++){
    std::vector<std::vector<float>> temp_frame = std::vector<std::vector<float>> (32, std::vector<float>(32,0.0));
    std::cout<<"Size of temp_flat: "<<temp_flat[seedidx].size()<<std::endl;
    for (int idx=0;idx<int(temp_flat[seedidx].size());idx++){
     temp_frame[int(idx/32)][idx%32]=temp_flat[seedidx][idx];
     //std::cout<<"["<<idx/32<<", "<<idx%32<<"]: ("<<temp_frame[int(idx/32)][int(idx%32)]<<") ";
    }
    //std::cout<<std::endl;
    std::cout<<" >> Class Object predictions of seed "<<seedidx<<"/"<<temp_flat.size()<<" are: "<<std::endl;
    predict_tf(temp_frame,"e_vs_ph_model.pb","inputs","softmax_1/Sigmoid");
   }*/
   
   std::unique_ptr<EB_photonFrames> vEB_photonFrames_edm (new EB_photonFrames(vEB_photonFrames));
   iEvent.put(std::move(vEB_photonFrames_edm),"photonFramePredSeedCollection");
   std::unique_ptr<std::vector<float>> vpredictions_edm (new std::vector<float>(vpredictions));
   iEvent.put(std::move(vpredictions_edm),"EBenergyClass");
   std::cout<<std::endl;
   nPassed++;
  // ----- Apply event selection cuts ----- //
   //std::cout<<"Event "<<nPassed-1<<"finished"<<"Prceeding to event "<<nPassed<<std::endl;
   return;
}

void
EGProducer::beginStream(edm::StreamID)
{
 nTotal = 0;
 nPassed = 0;
 std::cout<<"'EGProducer' Stream began"<<std::endl;
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
EGProducer::endStream() {
 std::cout << "'ProducerInference' selected: " << nPassed << "/" << nTotal << std::endl;
}

// ------------ method called when starting to processes a run  ------------
/*
void
ProducerTest::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
ProducerTest::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
ProducerTest::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
ProducerTest::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
EGProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(EGProducer);
