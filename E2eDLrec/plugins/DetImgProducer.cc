// -*- C++ -*-
//
// Package:    E2eDLrec/plugins/DetImgProducer
// Class:      DetImgProducer
// 
/**\class ProducerTest DetImgProducer.cc E2eDLrec/plugins/DetImgProducer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Shravan Chaudhari
//         Created:  Fri, 05 Jun 2020 16:11:58 GMT
//
//


// system include files
//#include <memory>
//#include "ProdTutorial/ProducerTest/plugins/DetImgProducer.h"
#include "E2eDL/E2eDLrec/plugins/DetImgProducer.h"
#include "PhysicsTools/TensorFlow/interface/TensorFlow.h"

using namespace tensorflow;
using namespace std;


DetImgProducer::DetImgProducer(const edm::ParameterSet& iConfig)
{
 EBRecHitCollectionT_    = consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("reducedEBRecHitCollection"));
 photonCollectionT_ = consumes<PhotonCollection>(iConfig.getParameter<edm::InputTag>("photonCollection"));
 HBHERecHitCollectionT_  = consumes<HBHERecHitCollection>(iConfig.getParameter<edm::InputTag>("reducedHBHERecHitCollection"));
 EERecHitCollectionT_    = consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("reducedEERecHitCollection"));
 //TRKRecHitCollectionT_   = consumes<TrackingRecHitCollection>(iConfig.getParameter<edm::InputTag>("trackRecHitCollection"));
 trackCollectionT_       = consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("trackCollection"));
 vertexCollectionT_       = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexCollection"));
 jetCollectionT_         = consumes<reco::PFJetCollection>(iConfig.getParameter<edm::InputTag>("ak4PFJetCollection"));
 TRKRecHitCollectionT_   = consumes<TrackingRecHitCollection>(iConfig.getParameter<edm::InputTag>("trackRecHitCollection"));
 genParticleCollectionT_ = consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("genParticleCollection"));
 genJetCollectionT_      = consumes<reco::GenJetCollection>(iConfig.getParameter<edm::InputTag>("genJetCollection"));
 pfCollectionT_          = consumes<PFCollection>(iConfig.getParameter<edm::InputTag>("pfCollection"));
 //pfCandCollectionT_ = consumes<PFCollection>(iConfig.getParameter<edm::InputTag>("pfCollection"));
 pvCollectionT_ = consumes<PVCollection>(iConfig.getParameter<edm::InputTag>("pvCollection"));
 recoJetsT_              = consumes<edm::View<reco::Jet> >(iConfig.getParameter<edm::InputTag>("recoJetsForBTagging"));
 jetTagCollectionT_      = consumes<reco::JetTagCollection>(iConfig.getParameter<edm::InputTag>("jetTagCollection"));
 ipTagInfoCollectionT_   = consumes<std::vector<reco::CandIPTagInfo> > (iConfig.getParameter<edm::InputTag>("ipTagInfoCollection"));
 
 /*mode_      = iConfig.getParameter<std::string>("mode");
 minJetPt_  = iConfig.getParameter<double>("minJetPt");
 maxJetEta_ = iConfig.getParameter<double>("maxJetEta");
 z0PVCut_   = iConfig.getParameter<double>("z0PVCut");*/
 
 //std::cout << " >> Mode set to " << mode_ << std::endl;
 /*if ( mode_ == "JetLevel" ) {
   doJets_ = true;
   nJets_ = iConfig.getParameter<int>("nJets");
   std::cout << "\t>> nJets set to " << nJets_ << std::endl;
 } else if ( mode_ == "EventLevel" ) {
   doJets_ = false;
 } else {
   std::cout << " >> Assuming EventLevel Config. " << std::endl;
   doJets_ = false;
 }
 edm::Service<TFileService> fs;
 RHTree = fs->make<TTree>("RHTree", "RecHit tree");
  if ( doJets_ ) {
    branchesEvtSel_jet( RHTree, fs );
  } else {
    branchesEvtSel( RHTree, fs );
  }*/

  ///////////adjustable granularity stuff

  granularityMultiPhi[0]  = iConfig.getParameter<int>("granularityMultiPhi");
  granularityMultiEta[0]  = iConfig.getParameter<int>("granularityMultiEta");

  granularityMultiPhi[1] = 3;
  granularityMultiEta[1] = 3;

  for (unsigned int proj=0; proj<Nadjproj; proj++)
  {
  
    int totalMultiEta = granularityMultiEta[proj] * granularityMultiECAL;

    for (int i=0; i<eta_nbins_HBHE; i++)
    {
      double step=(eta_bins_HBHE[i+1]-eta_bins_HBHE[i])/totalMultiEta;
      for (int j=0; j<totalMultiEta; j++)
      {
        adjEtaBins[proj].push_back(eta_bins_HBHE[i]+step*j);
      }
    }
    adjEtaBins[proj].push_back(eta_bins_HBHE[eta_nbins_HBHE]);

    totalEtaBins[proj] = totalMultiEta*(eta_nbins_HBHE);
    totalPhiBins[proj] = granularityMultiPhi[proj] * granularityMultiECAL*HBHE_IPHI_NUM;

  }
	
 //usesResource("TFileService");
 edm::Service<TFileService> fs;
 h_sel = fs->make<TH1F>("h_sel", "isSelected;isSelected;Events", 2, 0., 2.);
 RHTree = fs->make<TTree>("RHTree", "RecHit tree");
 //RHTree->Branch("SC_iphi", &vIphi_Emax_);
 //RHTree->Branch("SC_ieta", &vIeta_Emax_);
 branchesEB           ( RHTree, fs );
 //branchesPhotonSel ( RHTree, fs );
 branchesHBHE (RHTree, fs );
 branchesECALstitched (RHTree, fs);
 branchesTracksAtECALstitched (RHTree, fs);
 branchesTracksAtECALadjustable( RHTree, fs);
 std::cout<<"Branches done "<<std::endl;
 
 //produces<float>("photonClasses").setBranchAlias("PhotonClass");
 produces<std::vector<float>>("EBenergy");
 produces<std::vector<float>>("HBHEenergy");
 produces<std::vector<float>>("HBHEenergyEB");
 produces<std::vector<float>>("ECALstitchedenergy");
 produces<std::vector<float>>("TracksAtECALstitchedPt");
 produces<std::vector<float>>("TracksAtECALadjPt");
 produces<std::vector<float>>("TracksAtECALadj");
 produces<std::vector<float>>("TracksAtECALadjPtMax");
 //produces<std::vector<int>>("JetSeedieta");
 //produces<std::vector<int>>("JetSeediphi");
 //if (!fw) { return; }
}


DetImgProducer::~DetImgProducer()
{
 
}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
DetImgProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   //std::cout<<"New Event started"<<std::endl;
   using namespace edm;
   nTotal++;
   // ----- Apply event selection cuts ----- //
   
   //auto photon_classes = std::make_unique<float>(10.0);
   fillEB( iEvent, iSetup );
   std::unique_ptr<std::vector<float>> EBenergy_edm (new std::vector<float>(vEB_energy_));
   /*for (unsigned int i=0;i<vEB_energy_.size();i++){
    std::cout<<"( "<<i/vEB_energy_width<<", "<<i%vEB_energy_width<<" ) = "<<vEB_energy_[i]<<" ";
   }*/
   std::cout<<" >> Adding EB done "<<std::endl;
   //EBEnergy_edm->push_back(vEB_energy_);
   //std::cout<<"Size1 is: "<<vEB_energy_.size()<<std::endl;
   std::cout<<" >> Size of EB Energy vector is: "<<std::move(EBenergy_edm).get()->size()<<std::endl;
   // PhotonCollection 
   //*photon_classes=get_photons(iEvent, iSetup );
   //iEvent.put(std::move(photon_classes),"photonClasses");
   iEvent.put(std::move(EBenergy_edm),"EBenergy");
 
   fillHBHE (iEvent, iSetup );
   std::unique_ptr<std::vector<float>> HBHEenergy_edm (new std::vector<float>(vHBHE_energy_));
   std::unique_ptr<std::vector<float>> HBHEenergyEB_edm (new std::vector<float>(vHBHE_energy_EB_));
   std::cout<<" >> Size of HBHE Energy vector is: "<<std::move(HBHEenergy_edm).get()->size()<<std::endl;
   std::cout<<" >> Size of EB HBHE Energy vector is: "<<std::move(HBHEenergyEB_edm).get()->size()<<std::endl;
   iEvent.put(std::move(HBHEenergy_edm),"HBHEenergy");
   iEvent.put(std::move(HBHEenergyEB_edm),"HBHEenergyEB");
   
   fillECALstitched (iEvent, iSetup);
   std::unique_ptr<std::vector<float>> ECALstitched_energy_edm (new std::vector<float>(vECAL_energy_));
   std::cout<<" >> Size of Stitched ECAL Energy vector is: "<<std::move(ECALstitched_energy_edm).get()->size()<<std::endl;
   iEvent.put(std::move(ECALstitched_energy_edm), "ECALstitchedenergy");

   
   fillTracksAtECALstitched (iEvent, iSetup );
   std::unique_ptr<std::vector<float>> TracksECALstitchedPt_edm (new std::vector<float>(vECAL_tracksPt_));
   std::cout<<" >> Size of Pt Tracks vector at Stitched ECAL is: "<<std::move(TracksECALstitchedPt_edm).get()->size()<<std::endl;
   iEvent.put(std::move(TracksECALstitchedPt_edm), "TracksAtECALstitchedPt");
	
   for (unsigned int i=0;i<Nadjproj;i++)
   {
     fillTracksAtECALadjustable( iEvent, iSetup, i );
     //fillTRKlayersAtECALadjustable( iEvent, iSetup, i );
   }
   std::cout<<" >> Number of TracksAtECALadjPt per event: "<<sizeof(vECALadj_tracksPt_)/sizeof(vECALadj_tracksPt_[0])<<std::endl;
   std::cout<<" >> Number of TracksAtECALadj per event: "<<sizeof(vECALadj_tracksPt_)/sizeof(vECALadj_tracksPt_[0])<<std::endl;
   std::cout<<" >> Number of TracksAtECALadjPtMax per event: "<<sizeof(vECALadj_tracksPt_max_)/sizeof(vECALadj_tracksPt_max_[0])<<std::endl;
   std::cout<<" >> Sizes of TracksadjPt, Tracksadj and TracksadjPtMax are: "<<vECALadj_tracksPt_[0].size()<<", "<<vECALadj_tracks_[0].size()<<", "<<vECALadj_tracksPt_max_[0].size()<<std::endl;
   std::unique_ptr<std::vector<float>> TracksECALadjPt_edm (new std::vector<float>(vECALadj_tracksPt_[0]));
   std::cout<<" >> Size of Pt Tracks vector at ECAL adjustable is : "<<std::move(TracksECALadjPt_edm).get()->size()<<std::endl;
   iEvent.put(std::move(TracksECALadjPt_edm),"TracksAtECALadjPt");
   std::unique_ptr<std::vector<float>> TracksECALadj_edm (new std::vector<float>(vECALadj_tracks_[0]));
   std::cout<<" >> Size of Track vector at ECAL adjustable is : "<<std::move(TracksECALadj_edm).get()->size()<<std::endl;
   iEvent.put(std::move(TracksECALadj_edm),"TracksAtECALadj");
   std::unique_ptr<std::vector<float>> TracksECALadjPt_max_edm (new std::vector<float>(vECALadj_tracksPt_max_[0]));
   std::cout<<" >> Size of max Pt Track vector at ECAL adjustable is : "<<std::move(TracksECALadjPt_max_edm).get()->size()<<std::endl;
   iEvent.put(std::move(TracksECALadjPt_max_edm),"TracksAtECALadjPtMax");
 
   std::cout<<" >> Added EB, HBHE, HBHE_EB, ECALstitched, TracksAtECALstitchedPt and TracksAtECALadjPt to edm root file"<<std::endl;
   //EBEnergy_edm->clear();
   //iEvent.put(photon_classes,"photon_classes");
   // Fill RHTree
   RHTree->Fill();
   //vEB_photon_frames.clear();
   //h_sel->Fill( 1. );
   nPassed++;
   /*for (int frame_x=0; frame_x<vEB_frame_height;frame_x++){
    for (int frame_y=0;frame_y<vEB_frame_width;frame_y++){
     std::cout<<"yes "<<
    }
   }*/
   
   std::cout<<std::endl;
   //predict_tf();
   //std::cout<<"TF_predict done "<<std::endl;
   //int vec_size=61200;
   //std::vector<float> vEB_energy;
   //vEB_energy=read_vEB_energy(vec_size);
   return;
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
DetImgProducer::beginStream(edm::StreamID)
{
 nTotal = 0;
 nPassed = 0;
 std::cout<<"'DetImgProducer' Stream began"<<std::endl;
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
DetImgProducer::endStream() {
 std::cout << "'DetImgProducer' selected: " << nPassed << "/" << nTotal << std::endl;
 //fw->Write();
 //fw->Close();
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
DetImgProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


//define this as a plug-in
DEFINE_FWK_MODULE(DetImgProducer);
