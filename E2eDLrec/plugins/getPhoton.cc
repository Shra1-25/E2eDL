//#include "ProdTutorial/ProducerTest/plugins/QGProducer.h"
/*#include "ProdTutorial/ProducerTest/plugins/EGProducer.h"
#include "ProdTutorial/ProducerTest/plugins/predict_tf.h"
#include "ProdTutorial/ProducerTest/plugins/croppingFrames.h"
#include "ProdTutorial/ProducerTest/plugins/DetImgProducer.h"*/
#include "E2eDL/E2eDLrec/plugins/EGProducer.h"
#include "E2eDL/E2eDLrec/plugins/predict_tf.h"
#include "E2eDL/E2eDLrec/plugins/croppingFrames.h"
#include "E2eDL/E2eDLrec/plugins/DetImgProducer.h"
#include <fstream>
#include <sstream>

// Initialize branches _____________________________________________________//
void EGProducer::branchesPhotonSel ( TTree* tree, edm::Service<TFileService> &fs )
{
  /*hSC_pT = fs->make<TH1F>("SC_pT", "Pt", 27, 15., 150.);
  hMinDRgenRecoPho = fs->make<TH1F>("minDRgenRecoPho", "#DeltaR(#gamma_{gen},#gamma_{reco})_{min};#DeltaR;N", 100, 0., 25*0.0174);
  hMinDRrecoPtoGenPt = fs->make<TH1F>("minDRrecoPtoGenPt", "#DeltaR(#gamma_{gen},#gamma_{reco})_{min}, p_{T,reco}/p_{T,gen};p_{T,reco}/p_{T,gen};N", 60, -10., 10.);*/

  //tree->Branch("SC_mass",   &vSC_mass_);
  //tree->Branch("SC_DR",     &vSC_DR_);
  //tree->Branch("SC_E",      &vSC_E_);
  //tree->Branch("SC_pT",     &vSC_pT_);
  tree->Branch("SC_eta",    &vSC_eta_);
  tree->Branch("SC_phi",    &vSC_phi_);
  //tree->Branch("photon_frames",&vEB_photon_frames);
}
// Define struct to handle mapping for gen pho<->matched reco photons<->matched presel photons
struct pho_map {
  unsigned int idx;
  std::vector<unsigned int> matchedRecoPhoIdxs;
  std::vector<unsigned int> matchedPreselPhoIdxs;
};

std::vector<pho_map> vPhos;

void EGProducer::get_photons ( const edm::Event& iEvent, const edm::EventSetup& iSetup, EB_photonFrames& vEB_photonFrames){
 edm::Handle<PhotonCollection> photons;
 iEvent.getByToken(photonCollectionT_, photons);
 
 edm::Handle<EcalRecHitCollection> EBRecHitsH;
 iEvent.getByToken(EBRecHitCollectionT_, EBRecHitsH);
 std::string modelName = iConfig.getParameter<std::string>("EGModelName");
 // Provides access to global cell position and coordinates below
 /*edm::ESHandle<CaloGeometry> caloGeomH;
 iSetup.get<CaloGeometryRecord>().get(caloGeomH);
 const CaloGeometry* caloGeom = caloGeomH.product();*/
 nPho = 0;
 //int iphi_Emax, ieta_Emax;
 float Emax;
 std::vector<std::vector<float>> empty_vec;
 vIphi_Emax_.clear();
 vIeta_Emax_.clear();
 vPreselPhoIdxs_.clear();
 vEB_photon_frames.clear();
 empty_vec.clear();
  
  
 int iphi_, ieta_; // rows:ieta, cols:iphi
 std::cout<<"Photons size : "<<photons->size()<<std::endl;
 if (photons->size()<=0){std::cout<<" >> Prediction: -1"<<std::endl; 
                          framePredCollection photonJetCollection;
                          photonJetCollection.putPredCollection(vpredictions);
                          photonJetCollection.putFrameCollection(empty_vec);
                          vEB_photonFrames.push_back(photonJetCollection);
                        }
 for ( unsigned int iP = 0; iP < photons->size(); iP++ ) {
  PhotonRef iRecoPho( photons, iP );
  
  // Get underlying super cluster
  reco::SuperClusterRef const& iSC = iRecoPho->superCluster();
  std::vector<std::pair<DetId, float>> const& SCHits( iSC->hitsAndFractions() );
  std::cout<<"Processing photon "<<iP+1<<"/"<<photons->size()<<" of the event "<<nPassed+1<<":"<<endl;
  std::cout << " >> SChits.size: " << SCHits.size() << std::endl;
  
  // Get Emax crystal
  Emax = 0.;
  iphi_Emax = -1;
  ieta_Emax = -1;
 
   
  // Loop over SC hits of photon
  for(unsigned iH(0); iH != SCHits.size(); ++iH) {

   // Get DetId
   if ( SCHits[iH].first.subdetId() != EcalBarrel ) continue;
   EcalRecHitCollection::const_iterator iRHit( EBRecHitsH->find(SCHits[iH].first) );
   if ( iRHit == EBRecHitsH->end() ) continue;
  
   // Convert coordinates to ordinals
   EBDetId ebId( iRHit->id() );
   //EBDetId ebId( iSC->seed()->seed() );
   ieta_ = ebId.ieta() > 0 ? ebId.ieta()-1 : ebId.ieta(); // [-85,...,-1,1,...,85]
   ieta_ += EBDetId::MAX_IETA; // [0,...,169]
   iphi_ = ebId.iphi()-1; // [0,...,359]
   // Keep coordinates of shower max
   if ( iRHit->energy() > Emax ) {
     Emax = iRHit->energy();
     iphi_Emax = iphi_;
     ieta_Emax = ieta_;
   }
   //std::cout << " >> " << iH << ": iphi_,ieta_,E: " << iphi_ << ", " << ieta_ << ", " << iRHit->energy() << std::endl;
  }//SCHits
  // Apply selection on position of shower seed
  //std::cout << " >> Found: iphi_Emax,ieta_Emax: " << iphi_Emax << ", " << ieta_Emax << std::endl;
  if ( Emax <= zs ) {
    std::cout<<" >> EB_energy is less than zero: "<<Emax<<std::endl;
    std::cout<<" >> Prediction: -1"<<std::endl; 
    //vpredictions.push_back(-1);
    framePredCollection photonJetCollection;
    photonJetCollection.putPredCollection(vpredictions);
    photonJetCollection.putFrameCollection(empty_vec);
    photonJetCollection.putIetaSeed(ieta_Emax);
    photonJetCollection.putIphiSeed(iphi_Emax);
    vEB_photonFrames.push_back(photonJetCollection);
    if (iP==(photons->size()-1)){std::cout<<" >> All Done"<<std::endl;}
    continue;
  }
   
  vIphi_Emax_.push_back( iphi_Emax );
  vIeta_Emax_.push_back( ieta_Emax );
  std::cout << " >> Found: iphi_Emax,ieta_Emax: " << iphi_Emax << ", " << ieta_Emax << std::endl;
  std::cout<<" >> EB_energy at iphi_Emax,ieta_Emax: "<<vEB_energy_[ieta_Emax*vEB_energy_width+iphi_Emax]<<endl;
  if ( ieta_Emax > 169 - 16 || ieta_Emax < 15 )  // seed centered on [15,15] so must be padded by 15 below and 16 above
  {  
    std::cout<<" >> Prediction: -1"<<std::endl;
    //vpredictions.push_back(-1);
    framePredCollection photonJetCollection;
    photonJetCollection.putPredCollection(vpredictions);
    photonJetCollection.putFrameCollection(empty_vec);
    photonJetCollection.putIetaSeed(ieta_Emax);
    photonJetCollection.putIphiSeed(iphi_Emax);
    vEB_photonFrames.push_back(photonJetCollection);
    if (iP==(photons->size()-1)){std::cout<<" >> All Done"<<std::endl;}
    continue;
  }
  nPho++;
  
  vEB_frame=croppingFrames(vEB_energy_, ieta_Emax, iphi_Emax, 170,360,32,32);
  /*for(int x_idx=0; x_idx<int(vEB_frame.size()); x_idx++){
    for (int y_idx=0; y_idx<int(vEB_frame[0].size()); y_idx++){
      std::cout<<"["<<x_idx<<", "<<y_idx<<"]: "<<vEB_frame[x_idx][y_idx];
    }
  }*/
  std::cout<<" >> Current Photon frame is: "<<iP+1<<"/"<<photons->size()<<std::endl;
  framePredCollection photonJetCollection;
  photonJetCollection.putPredCollection(predict_tf(vEB_frame, modelName,"inputs","softmax_1/Sigmoid"));
  /*vEB_flat_frame.clear();
  for (int frame_x=0;frame_x<int(vEB_frame.size());frame_x++){
    for (int frame_y=0;frame_y<int(vEB_frame[0].size());frame_y++){
      vEB_flat_frame.push_back(vEB_frame[frame_x][frame_y]);
    }
  }*/
  //std::cout<<" >> Size of flat frame: "<<vEB_flat_frame.size()<<std::endl;
  photonJetCollection.putFrameCollection(vEB_frame);
  photonJetCollection.putIetaSeed(ieta_Emax);
  photonJetCollection.putIphiSeed(iphi_Emax);
  vEB_photonFrames.push_back(photonJetCollection);
  /*if (vEB_photon_frames.size()>0){ 
   RHTree->Branch(branchname,&vEB_photon_frames[vEB_photon_frames.size()-1]);
  }*/
  /*for (int i=0;i<32;i++){
    for (int j=0;j<32;j++){
      std::cout<<"("<<vEB_flat_frame[i*32+j]<<", "<<vEB_frame[i][j]<<") ";
    }
    std::cout<<endl;
  }*/
 }
 return;
}
