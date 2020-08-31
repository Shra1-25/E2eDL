/*#include "ProdTutorial/ProducerTest/plugins/TopProducer.h"
#include "ProdTutorial/ProducerTest/plugins/DetImgProducer.h"
#include "ProdTutorial/ProducerTest/plugins/predict_tf.h"
#include "ProdTutorial/ProducerTest/plugins/croppingFrames.h"
#include "ProdTutorial/ProducerTest/plugins/frameStriding.h"
#include "PhysicsTools/TensorFlow/interface/TensorFlow.h"*/
#include "E2eDL/E2eDLrec/plugins/TopProducer.h"
#include "E2eDL/E2eDLrec/plugins/DetImgProducer.h"
#include "E2eDL/E2eDLrec/plugins/predict_tf.h"
#include "E2eDL/E2eDLrec/plugins/croppingFrames.h"
#include "E2eDL/E2eDLrec/plugins/frameStriding.h"
#include "PhysicsTools/TensorFlow/interface/TensorFlow.h"
#include <fstream>

using namespace tensorflow;
using namespace std;

TopProducer::TopProducer(const edm::ParameterSet& iConfig)
{
 //vEB_photon_frames = consumes<std::vector<std::vector<float>>>(iConfig.getParameter<edm::InputTag>("frames_"));
 photonCollectionT_ = consumes<PhotonCollection>(iConfig.getParameter<edm::InputTag>("photonCollection"));
 EBRecHitCollectionT_    = consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("reducedEBRecHitCollection"));
 HBHERecHitCollectionT_  = consumes<HBHERecHitCollection>(iConfig.getParameter<edm::InputTag>("reducedHBHERecHitCollection"));
 ECALstitched_energy_token=consumes<std::vector<float>>(iConfig.getParameter<edm::InputTag>("ECALstitchedenergy"));
 TracksAtECALstitchedPt_token=consumes<std::vector<float>>(iConfig.getParameter<edm::InputTag>("TracksAtECALstitchedPt"));
 TracksAtECALadjPt_token=consumes<std::vector<float>>(iConfig.getParameter<edm::InputTag>("TracksAtECALadjPt"));
 //JetSeed_ieta_token=consumes<std::vector<int>>(iConfig.getParameter<edm::InputTag>("JetSeedieta"));
 //JetSeed_iphi_token=consumes<std::vector<int>>(iConfig.getParameter<edm::InputTag>("JetSeediphi"));
 HBHEenergy_token = consumes<std::vector<float>>(iConfig.getParameter<edm::InputTag>("HBHEenergy"));
 vertexCollectionT_       = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexCollection"));
 jetCollectionT_         = consumes<reco::PFJetCollection>(iConfig.getParameter<edm::InputTag>("ak8PFJetCollection"));
 TRKRecHitCollectionT_   = consumes<TrackingRecHitCollection>(iConfig.getParameter<edm::InputTag>("trackRecHitCollection"));
 genParticleCollectionT_ = consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("genParticleCollection"));
 genJetCollectionT_      = consumes<reco::GenJetCollection>(iConfig.getParameter<edm::InputTag>("ak8GenJetCollection"));
 pfCollectionT_          = consumes<PFCollection>(iConfig.getParameter<edm::InputTag>("pfCollection"));
 recoJetsT_              = consumes<edm::View<reco::Jet> >(iConfig.getParameter<edm::InputTag>("recoJetsForBTagging"));
 jetTagCollectionT_      = consumes<reco::JetTagCollection>(iConfig.getParameter<edm::InputTag>("jetTagCollection"));
 ipTagInfoCollectionT_   = consumes<std::vector<reco::CandIPTagInfo> > (iConfig.getParameter<edm::InputTag>("ipTagInfoCollection"));
 HBHEjetCollectionT_ = consumes<edm::SortedCollection<framePredCollection> > (iConfig.getParameter<edm::InputTag>("HBHEjetCollection"));
 ECALstitchedJetCollectionT_ = consumes<edm::SortedCollection<framePredCollection> > (iConfig.getParameter<edm::InputTag>("ECALstitchedJetCollection"));
 TracksAtECALstitchedJetCollectionPtT_ = consumes<edm::SortedCollection<framePredCollection> > (iConfig.getParameter<edm::InputTag>("TracksAtECALstitchedJetCollection"));
  
 mode_      = iConfig.getParameter<std::string>("mode");
 minJetPt_  = iConfig.getParameter<double>("minJetPt");
 maxJetEta_ = iConfig.getParameter<double>("maxJetEta");
 z0PVCut_   = iConfig.getParameter<double>("z0PVCut");
 modelName = iConfig.getParameter<std::string>("TopQuarksModelName");
 std::cout << " >> Mode set to " << mode_ << std::endl;	
 if ( mode_ == "JetLevel" ) {
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
  TopTree = fs->make<TTree>("TopTree", "RecHit tree");
   if ( doJets_ ) {
     branchesEvtSel_jet( TopTree, fs );
   } else {
     branchesEvtSel( TopTree, fs );
   }
		
 produces<std::vector<float>>("TopQecalStitchedClass");
 produces<std::vector<float>>("TopQtracksAtECALstitchedPtClass");
 produces<std::vector<float>>("TopQtracksAtECALadjPtClass");
 produces<std::vector<float>>("TopQhbheEnergyClass");
 produces<std::vector<int>>("ak8JetSeedieta");
 produces<std::vector<int>>("ak8JetSeediphi");
 produces<topqJetCollection>("TopQtracksAtECALstitchedJetCollectionPt");
 produces<topqJetCollection>("TopQtracksAtECALadjJetCollectionPt");
 produces<topqJetCollection>("TopQecalStitchedJetCollection");
 produces<topqJetCollection>("TopQhbheJetCollection");
}

TopProducer::~TopProducer()
{
 
}

//
// member functions
//

// ------------ method called to produce the data  ------------
void
TopProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   std::cout<<" >> Running TopProducer."<<std::endl;
   /*edm::Handle<edm::SortedCollection<framePredCollection>> HBHEjetCollection_handle;
   iEvent.getByToken(HBHEjetCollectionT_, HBHEjetCollection_handle);
   edm::SortedCollection<framePredCollection> qgHBHEjetCollection = *HBHEjetCollection_handle;
	
   edm::Handle<edm::SortedCollection<framePredCollection>> ECALstitchedJetCollection_handle;
   iEvent.getByToken(ECALstitchedJetCollectionT_, ECALstitchedJetCollection_handle);
   edm::SortedCollection<framePredCollection> qgECALstitchedJetCollection = *ECALstitchedJetCollection_handle;
   
   edm::Handle<edm::SortedCollection<framePredCollection>> TracksAtECALstitchedJetCollectionPt_handle;
   iEvent.getByToken(TracksAtECALstitchedJetCollectionPtT_, TracksAtECALstitchedJetCollectionPt_handle);
   edm::SortedCollection<framePredCollection> qgTracksAtECALstitchedJetCollectionPt = *TracksAtECALstitchedJetCollectionPt_handle;

   // Code (Commented below) to verify ECALstitched and TracksAtECALstitchd jet collection branch of edm root file	
   std::cout<<"Current size of HBHE jet collection: "<<qgHBHEjetCollection.size()<<std::endl;
   std::cout<<"Current size of ECAL stitched jet collection: "<<qgECALstitchedJetCollection.size()<<std::endl;
   std::cout<<"Current size of Tracks at ECAL stitched jet collection: "<<qgTracksAtECALstitchedJetCollectionPt.size()<<std::endl;
   std::cout<<"Following verification processes are in the above order."<<std::endl;
   std::vector<qgJetCollection> vqgJetCollection={qgHBHEjetCollection, qgECALstitchedJetCollection, qgTracksAtECALstitchedJetCollectionPt};
   for (qgJetCollection qgJetCollection_itr : vqgJetCollection){
   float seedx,seedy;
   for (int frameidx=0;frameidx<int(qgJetCollection_itr.size());frameidx++){
	seedx = qgJetCollection_itr[frameidx].getIetaSeed();
   	seedy = qgJetCollection_itr[frameidx].getIphiSeed();
	std::cout<<"Verifying seed "<<frameidx+1<<"/"<<qgJetCollection_itr.size()<<std::endl;
	std::vector<float> ph_pred=qgJetCollection_itr[frameidx].getPredCollection();
   	std::cout<<" >> Class Object Seeds are: ";
    	std::cout<<"["<<seedx<<", "<<seedy<<"], ";
   	std::cout<<std::endl;
   	std::vector<std::vector<float>> temp_frame=qgJetCollection_itr[frameidx].getFrameCollection();
   	if (temp_frame.size()==0) std::cout<<" >> Empty photon frame collection for the curent seed."<<std::endl;
    	else {
		std::cout<<" >> Size of temp_frame: ("<<temp_frame.size()<<", "<<temp_frame[0].size()<<std::endl;
    		std::vector<float> temp_pred=predict_tf(temp_frame,"ResNet.pb","inputs","outputs");
		std::cout<<" >> Class Object model predictions of seed "<<frameidx+1<<"/"<<qgJetCollection_itr.size()<<" are: [";
    		for (int idx=0;idx<int(temp_pred.size());idx++){
      			std::cout<<temp_pred[idx]<<", ";
      		}
		std::cout<<"]"<<std::endl;
	}
	
	std::cout<<" >> Stored predictions for seed "<<frameidx+1<<" are: [";
	for (int idx=0;idx<int(ph_pred.size());idx++){	
	   std::cout<<ph_pred[idx]<<", ";
	}
      	std::cout<<"]"<<std::endl;
    }
   }*/
	
   using namespace edm;
   nTotal++;
   vJetSeed_ieta_.clear(); vJetSeed_iphi_.clear();
   bool passedSelection = false;
   if ( doJets_ ) {
     std::cout<<" >> doJets set"<<std::endl;
     passedSelection = runEventSel_jet( iEvent, iSetup );
     std::cout<<" >> Size of JetSeed vector (ak8JetSeed_eta_size, ak8JetSeed_phi_size) is: ("<<vJetSeed_ieta_.size()<<", "<<vJetSeed_iphi_.size()<<")"<<std::endl;
     std::cout<<" >> The jet seeds are (ieta,iphi): ";
     if (vJetSeed_ieta_.size()==0){vJetSeed_ieta_.push_back(-1); vJetSeed_iphi_.push_back(-1); std::cout<<"(-1, -1)"<<std::endl;}
     else{
     	for (int idx=0;idx<int(vJetSeed_ieta_.size());idx++){
     		std::cout<<"("<<vJetSeed_ieta_[idx]<<","<<vJetSeed_iphi_[idx]<<") ";
     	}
     	std::cout<<std::endl;
     }
     if (vJetSeed_ieta_.size()==vJetSeed_iphi_.size()){
     	for (int idx=0;idx<int(vJetSeed_ieta_.size());idx++){
     		if(vJetSeed_ieta_[idx]>=0){vJetSeed_ieta_[idx]=int(vJetSeed_ieta_[idx]*5+2);}  //5 EB xtals per HB tower
		if(vJetSeed_iphi_[idx]>=0){vJetSeed_iphi_[idx]=int(vJetSeed_iphi_[idx]*5+2);}  //5 EB xtals per HB tower
		//std::cout<<vJetSeed_ieta_[idx]<<" "<<vJetSeed_iphi_[idx];
     	}
     }
     std::unique_ptr<std::vector<int>> JetSeedieta_edm (new std::vector<int>(vJetSeed_ieta_));
     std::unique_ptr<std::vector<int>> JetSeediphi_edm (new std::vector<int>(vJetSeed_iphi_));
     iEvent.put(std::move(JetSeedieta_edm),"ak8JetSeedieta");
     iEvent.put(std::move(JetSeediphi_edm),"ak8JetSeediphi");
     //vJetSeed_ieta_.clear(); vJetSeed_iphi_.clear();
   } else {
     std::cout<<" >> doJets not set"<<std::endl;
     passedSelection = runEvtSel( iEvent, iSetup );
     std::cout<<" >> Size of JetSeed vector (ak8JetSeed_eta_size, ak8JetSeed_phi_size) is: ("<<vJetSeed_ieta_.size()<<", "<<vJetSeed_iphi_.size()<<")"<<std::endl;
     std::cout<<" The jet seeds are (ieta,iphi): ";
     if (vJetSeed_ieta_.size()==0){vJetSeed_ieta_.push_back(-1); vJetSeed_iphi_.push_back(-1); std::cout<<"(-1, -1)"<<std::endl;}
     else{
	   for (int idx=0;idx<int(vJetSeed_ieta_.size());idx++){
     		std::cout<<"("<<vJetSeed_ieta_[idx]<<","<<vJetSeed_iphi_[idx]<<") ";
     	}
     	std::cout<<std::endl;
     }
     if (vJetSeed_ieta_.size()==vJetSeed_iphi_.size()){
     	for (int idx=0;idx<int(vJetSeed_ieta_.size());idx++){
     		if(vJetSeed_ieta_[idx]>=0){vJetSeed_ieta_[idx]=int(vJetSeed_ieta_[idx]*5+2);}  //5 EB xtals per HB tower
		if(vJetSeed_iphi_[idx]>=0){vJetSeed_iphi_[idx]=int(vJetSeed_iphi_[idx]*5+2);}  //5 EB xtals per HB tower
		//std::cout<<vJetSeed_ieta_[idx]<<" "<<vJetSeed_iphi_[idx];
     	}
     }
     std::unique_ptr<std::vector<int>> JetSeedieta_edm (new std::vector<int>(vJetSeed_ieta_));
     std::unique_ptr<std::vector<int>> JetSeediphi_edm (new std::vector<int>(vJetSeed_iphi_));
     iEvent.put(std::move(JetSeedieta_edm),"ak8JetSeedieta");
     iEvent.put(std::move(JetSeediphi_edm),"ak8JetSeediphi");
   }

   if ( !passedSelection ) {
     h_sel->Fill( 0. );;
     return;
   }  
 
   edm::Handle<std::vector<float>> ECALstitched_energy_handle;
   iEvent.getByToken(ECALstitched_energy_token, ECALstitched_energy_handle);
   edm::Handle<std::vector<float>> TracksAtECALstitchedPt_handle;
   iEvent.getByToken(TracksAtECALstitchedPt_token, TracksAtECALstitchedPt_handle);
   edm::Handle<std::vector<float>> TracksAtECALadjPt_handle;
   iEvent.getByToken(TracksAtECALadjPt_token, TracksAtECALadjPt_handle);
   /*edm::Handle<std::vector<int>> JetSeed_ieta_handle;
   iEvent.getByToken(JetSeed_ieta_token, JetSeed_ieta_handle);
   edm::Handle<std::vector<int>> JetSeed_iphi_handle;
   iEvent.getByToken(JetSeed_iphi_token, JetSeed_iphi_handle);*/
   edm::Handle<std::vector<float>> HBHEenergy_handle;
   iEvent.getByToken(HBHEenergy_token, HBHEenergy_handle);
   
   std::vector<std::vector<float>> empty_vec;
   std::vector<float>vECALstitched=*ECALstitched_energy_handle;
   std::vector<float>vTracksAtECALstitchedPt=*TracksAtECALstitchedPt_handle;
   std::vector<float>vTracksAtECALadjPt=*TracksAtECALadjPt_handle;
   /*std::vector<int>vJetSeed_ieta=*JetSeed_ieta_handle;
   std::vector<int>vJetSeed_iphi=*JetSeed_iphi_handle;*/
   topqJetCollection HBHEJetCollection;
   topqJetCollection ECALstitchedJetCollection;
   topqJetCollection TracksAtECALstitchedJetCollectionPt;
   topqJetCollection TracksAtECALadjJetCollectionPt;
	
   std::vector<float>vECALstitchedClass;
   std::vector<float>vTracksAtECALstitchedPtClass;
   std::vector<float>vTracksAtECALadjPtClass;
   std::vector<float>vHBHEenergyClass;
   vECALstitchedClass.clear();
   vTracksAtECALstitchedPtClass.clear();
   vTracksAtECALadjPtClass.clear();
   vHBHEenergyClass.clear();
   vHBHEenergy_frame.clear();
   vECALstitched_frame.clear();
   vTracksAtECALstitchedPt_frame.clear();
   vTracksAtECALadjPt_frame.clear();
   for (int idx=0;idx<int(vJetSeed_ieta_.size());idx++){
    std::cout<<"Generating stitched and adjustable ECAL frames and their track frames from the jet seed "<<idx+1<<"/"<<vJetSeed_ieta_.size()<<" with seed value: ("<<vJetSeed_ieta_[idx]<<","<<vJetSeed_iphi_[idx]<<")"<<std::endl;
    if(vJetSeed_ieta_[idx]>=0) {vECALstitched_frame=croppingFrames(vECALstitched, vJetSeed_ieta_[idx], vJetSeed_iphi_[idx], 280, 360, 125, 125); 
                               	vTracksAtECALstitchedPt_frame=croppingFrames(vTracksAtECALstitchedPt, vJetSeed_ieta_[idx], vJetSeed_iphi_[idx], 280, 360, 125, 125);
				vTracksAtECALadjPt_frame=croppingFrames(vTracksAtECALadjPt, vJetSeed_ieta_[idx], vJetSeed_iphi_[idx], 280, 360, 125, 125);
    /*string filename="ECALstitched_"+std::to_string(nPassed+1)+"_"+std::to_string(idx+1)+".csv";
    std::ofstream file1(filename);
    for (int i=0;i<int(vECALstitched_frame.size());i++){
     for(int j=0;j<int(vECALstitched_frame[0].size());j++){
      file1<<vECALstitched_frame[i][j];
       if (j!=int(vECALstitched_frame[0].size())-1){file1<<",";}
      }
     file1<<"\n";
    }*/
    /*filename="TracksAtECALstitchedPt_"+std::to_string(nPassed+1)+"_"+std::to_string(idx+1)+".csv";
    std::ofstream file2(filename);
    for (int i=0;i<int(vTracksAtECALstitchedPt_frame.size());i++){
     for(int j=0;j<int(vTracksAtECALstitchedPt_frame[0].size());j++){
       file2<<vTracksAtECALstitchedPt_frame[i][j];
       if (j!=int(vTracksAtECALstitchedPt_frame[0].size())-1){file2<<",";}
      }
     file2<<"\n";
    }*/
    //vECALstitchedClass.push_back(predict_tf(vECALstitched_frame, "ResNet.pb", "inputs","softmax_1/Sigmoid"));
    vECALstitchedClass = predict_tf(vECALstitched_frame, modelName, "inputs","outputs");
    //vTracksAtECALstitchedPtClass.push_back(predict_tf(vTracksAtECALstitchedPt_frame, "ResNet.pb", "inputs", "softmax_1/Sigmoid"));
    vTracksAtECALstitchedPtClass = predict_tf(vTracksAtECALstitchedPt_frame, modelName, "inputs", "outputs");
    //vTracksAtECALadjPtClass.push_back(predict_tf(vTracksAtECALadjPt_frame, "ResNet.pb", "inputs", "softmax_1/Sigmoid"));
    vTracksAtECALadjPtClass = predict_tf(vTracksAtECALadjPt_frame, modelName, "inputs", "outputs");
				
    framePredCollection topqECALstitchedJetCollection;
    framePredCollection topqTracksAtECALstitchedJetCollection;
    framePredCollection topqTracksAtECALadjJetCollection;
				
    topqECALstitchedJetCollection.putPredCollection(vECALstitchedClass);
    topqTracksAtECALstitchedJetCollection.putPredCollection(vTracksAtECALstitchedPtClass);
    topqTracksAtECALadjJetCollection.putPredCollection(vTracksAtECALadjPtClass);
				
    topqECALstitchedJetCollection.putFrameCollection(vECALstitched_frame);
    topqTracksAtECALstitchedJetCollection.putFrameCollection(vTracksAtECALstitchedPt_frame);
    topqTracksAtECALadjJetCollection.putFrameCollection(vTracksAtECALadjPt_frame);
				
    topqECALstitchedJetCollection.putIetaSeed(vJetSeed_ieta_[idx]);
    topqECALstitchedJetCollection.putIphiSeed(vJetSeed_iphi_[idx]);
    topqTracksAtECALstitchedJetCollection.putIetaSeed(vJetSeed_ieta_[idx]);
    topqTracksAtECALstitchedJetCollection.putIphiSeed(vJetSeed_iphi_[idx]);
    topqTracksAtECALadjJetCollection.putIetaSeed(vJetSeed_ieta_[idx]);
    topqTracksAtECALadjJetCollection.putIphiSeed(vJetSeed_iphi_[idx]);
				
    ECALstitchedJetCollection.push_back(topqECALstitchedJetCollection);
    TracksAtECALstitchedJetCollectionPt.push_back(topqTracksAtECALstitchedJetCollection);
    TracksAtECALadjJetCollectionPt.push_back(topqTracksAtECALadjJetCollection);
    }
    else {
     vECALstitchedClass.push_back(-1);
     vTracksAtECALstitchedPtClass.push_back(-1);
     vTracksAtECALadjPtClass.push_back(-1);
	    
     framePredCollection topqECALstitchedJetCollection;
     topqECALstitchedJetCollection.putPredCollection(vpredictions);
     topqECALstitchedJetCollection.putFrameCollection(empty_vec);
     topqECALstitchedJetCollection.putIetaSeed(vJetSeed_ieta_[idx]);
     topqECALstitchedJetCollection.putIphiSeed(vJetSeed_iphi_[idx]);
     ECALstitchedJetCollection.push_back(topqECALstitchedJetCollection);
	    
     framePredCollection topqTracksAtECALstitchedJetCollection;
     topqTracksAtECALstitchedJetCollection.putPredCollection(vpredictions);
     topqTracksAtECALstitchedJetCollection.putFrameCollection(empty_vec);
     topqTracksAtECALstitchedJetCollection.putIetaSeed(vJetSeed_ieta_[idx]);
     topqTracksAtECALstitchedJetCollection.putIphiSeed(vJetSeed_iphi_[idx]);
     TracksAtECALstitchedJetCollectionPt.push_back(topqTracksAtECALstitchedJetCollection);
	    
     framePredCollection topqTracksAtECALadjJetCollection;
     topqTracksAtECALadjJetCollection.putPredCollection(vpredictions);
     topqTracksAtECALadjJetCollection.putFrameCollection(empty_vec);
     topqTracksAtECALadjJetCollection.putIetaSeed(vJetSeed_ieta_[idx]);
     topqTracksAtECALadjJetCollection.putIphiSeed(vJetSeed_iphi_[idx]);
     TracksAtECALadjJetCollectionPt.push_back(topqTracksAtECALadjJetCollection);
    
     std::cout<<" >> TopInference Prediction of Stitched ECAL: "<<vECALstitchedClass[idx]<<std::endl;
     std::cout<<" >> TopInference Prediction of Tracks at Stitched ECAL: "<<vTracksAtECALstitchedPtClass[idx]<<std::endl;
     std::cout<<" >> TopInference Prediction of Tracks at ECAL adjustable: "<<vTracksAtECALadjPtClass[idx]<<std::endl;
    }
   }
   std::cout<<std::endl; //Stitched ECAL and their track frames created.
   	
   std::vector<float> vHBHEenergy=*HBHEenergy_handle;
   std::cout<<" >> Size of HBHE energy vector read: "<<vHBHEenergy.size()<<std::endl;
   std::vector<std::vector<float>> vHBHEenergy_strided = frameStriding(vHBHEenergy,56,72,5,5);
   std::cout<<" >> Size of Strided HBHE energy vector: ("<<vHBHEenergy_strided.size()<<","<<vHBHEenergy_strided[0].size()<<")"<<std::endl; //HBHE energy vector upsampled.
   std::vector<float> vHBHE_strided_flat (vHBHEenergy_strided.size()*vHBHEenergy_strided[0].size(),0);
   for (int x=0;x<int(vHBHEenergy_strided.size());x++){
    for (int y=0;y<int(vHBHEenergy_strided[0].size());y++){
     vHBHE_strided_flat[x*vHBHEenergy_strided[0].size()+y]=vHBHEenergy_strided[x][y];
    }
   }
   
   for (int idx=0;idx<int(vJetSeed_ieta_.size());idx++){
    std::cout<<"Generating HBHE energy frames from the jet seed "<<idx+1<<"/"<<vJetSeed_ieta_.size()<<" with seed value: ("<<vJetSeed_ieta_[idx]<<","<<vJetSeed_iphi_[idx]<<")"<<std::endl;
    if(vJetSeed_ieta_[idx]>=0) {vHBHEenergy_frame=croppingFrames(vHBHE_strided_flat, vJetSeed_ieta_[idx], vJetSeed_iphi_[idx], 280, 360, 125, 125); 
   /*for(int i=140;i<141;i++){
    for (int ki=0; ki<5;ki++){
     for (int j=0;j<360;j++){
      for (int kj=0;kj<5;kj++){
      std::cout<<"("<<i<<","<<j<<"): "<<vHBHEenergy_strided[5*i+ki][5*j+kj]<<" "<<vHBHEenergy[i*360+j]/25<<" ";
     }
     }
    }
   }
   std::cout<<std::endl;*/
    //std::cout<<std::endl;
    /*string filename="HBHEenergy"+std::to_string(nPassed+1)+".csv";
    std::ofstream file3(filename);
    for (int i=0;i<int(vHBHEenergy_frame.size());i++){
     for(int j=0;j<int(vHBHEenergy_frame[0].size());j++){
      file3<<vHBHEenergy_frame[i][j];
      if (j!=int(vHBHEenergy_frame[0].size())-1){file3<<",";}
     }
    file3<<"\n";
   }*/
   //vHBHEenergyClass.push_back(predict_tf(vHBHEenergy_frame, "qg_model.pb", "inputs", "softmax_1/Sigmoid"));
   vHBHEenergyClass = predict_tf(vHBHEenergy_frame, modelName, "inputs", "outputs");//"softmax_1/Sigmoid");
   framePredCollection topqHBHEJetCollection;
   topqHBHEJetCollection.putPredCollection(vHBHEenergyClass);
   topqHBHEJetCollection.putFrameCollection(vHBHEenergy_frame);
   topqHBHEJetCollection.putIetaSeed(vJetSeed_ieta_[idx]);
   topqHBHEJetCollection.putIphiSeed(vJetSeed_iphi_[idx]);
   HBHEJetCollection.push_back(topqHBHEJetCollection);
   }
   else {
     vHBHEenergyClass.push_back(-1);
     std::cout<<" >> TopInference Prediction of HBHE energy: "<<vHBHEenergyClass[idx]<<std::endl;
     framePredCollection topqHBHEJetCollection;
     topqHBHEJetCollection.putPredCollection(vpredictions);
     topqHBHEJetCollection.putFrameCollection(empty_vec);
     topqHBHEJetCollection.putIetaSeed(vJetSeed_ieta_[idx]);
     topqHBHEJetCollection.putIphiSeed(vJetSeed_iphi_[idx]);
     HBHEJetCollection.push_back(topqHBHEJetCollection);
    }
   }
   std::unique_ptr<topqJetCollection> HBHEJetCollection_edm (new topqJetCollection(HBHEJetCollection));
   iEvent.put(std::move(HBHEJetCollection_edm),"TopQhbheJetCollection");
   std::unique_ptr<topqJetCollection> ECALstitchedJetCollection_edm (new topqJetCollection(ECALstitchedJetCollection));
   iEvent.put(std::move(ECALstitchedJetCollection_edm),"TopQecalStitchedJetCollection");
   std::unique_ptr<topqJetCollection> TracksAtECALstitchedJetCollectionPt_edm (new topqJetCollection(TracksAtECALstitchedJetCollectionPt));
   iEvent.put(std::move(TracksAtECALstitchedJetCollectionPt_edm),"TopQtracksAtECALstitchedJetCollectionPt");
   std::unique_ptr<topqJetCollection> TracksAtECALadjJetCollectionPt_edm (new topqJetCollection(TracksAtECALadjJetCollectionPt));
   iEvent.put(std::move(TracksAtECALadjJetCollectionPt_edm),"TopQtracksAtECALadjJetCollectionPt");
   std::unique_ptr<std::vector<float>> vECALstitchedClass_edm (new std::vector<float>(vECALstitchedClass));
   iEvent.put(std::move(vECALstitchedClass_edm),"TopQecalStitchedClass");
   std::unique_ptr<std::vector<float>> vTracksAtECALstitchedPtClass_edm (new std::vector<float>(vTracksAtECALstitchedPtClass));
   iEvent.put(std::move(vTracksAtECALstitchedPtClass_edm),"TopQtracksAtECALstitchedPtClass");
   std::unique_ptr<std::vector<float>> vTracksAtECALadjPtClass_edm (new std::vector<float>(vTracksAtECALadjPtClass));
   iEvent.put(std::move(vTracksAtECALadjPtClass_edm),"TopQtracksAtECALadjPtClass");
   std::unique_ptr<std::vector<float>> vHBHEenergyClass_edm (new std::vector<float>(vHBHEenergyClass));
   iEvent.put(std::move(vHBHEenergyClass_edm),"TopQhbheEnergyClass");
   std::cout<<std::endl;

   TopTree->Fill();
   nPassed++;
   // ----- Apply event selection cuts ----- //
   //std::cout<<"Event "<<nPassed-1<<"finished"<<"Prceeding to event "<<nPassed<<std::endl;
   return;
}
// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
TopProducer::beginStream(edm::StreamID)
{
 nTotal = 0;
 nPassed = 0;
 std::cout<<"'TopProducer' Stream began"<<std::endl;
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
TopProducer::endStream() {
 std::cout << "'TopProducer' selected: " << nPassed << "/" << nTotal << std::endl;
}

// ------------ method called when starting to processes a run  ------------
/*
void
TopProducer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
TopProducer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
TopProducer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
TopProducer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TopProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
const reco::PFCandidate*
TopProducer::getPFCand(edm::Handle<PFCollection> pfCands, float eta, float phi, float& minDr, bool debug ) {

  minDr = 10;
  const reco::PFCandidate* minDRCand = nullptr;
  
  for ( PFCollection::const_iterator iPFC = pfCands->begin();
        iPFC != pfCands->end(); ++iPFC ) {

    const reco::Track* thisTrk = iPFC->bestTrack();
    if ( !thisTrk ) continue;

    float thisdR = reco::deltaR( eta, phi, thisTrk->eta(), thisTrk->phi() );
    if (debug) std::cout << "\tthisdR: " << thisdR << " " << thisTrk->pt() << " " << iPFC->particleId() << std::endl;

    const reco::PFCandidate& thisPFCand = (*iPFC);
      
    if ( (thisdR < 0.01) && (thisdR <minDr) ) {
      minDr    = thisdR; 
      minDRCand = &thisPFCand;
    }
  }

  return minDRCand;  
}

const reco::Track*
TopProducer::getTrackCand(edm::Handle<reco::TrackCollection> trackCands, float eta, float phi, float& minDr, bool debug ) {

  minDr = 10;
  const reco::Track* minDRCand = nullptr;
  reco::Track::TrackQuality tkQt_ = reco::Track::qualityByName("highPurity");

  for ( reco::TrackCollection::const_iterator iTk = trackCands->begin();
        iTk != trackCands->end(); ++iTk ) {
    if ( !(iTk->quality(tkQt_)) ) continue;  

    float thisdR = reco::deltaR( eta, phi, iTk->eta(),iTk->phi() );
    if (debug) std::cout << "\tthisdR: " << thisdR << " " << iTk->pt() << std::endl;

    const reco::Track& thisTrackCand = (*iTk);
      
    if ( (thisdR < 0.01) && (thisdR <minDr) ) {
      minDr    = thisdR; 
      minDRCand = &thisTrackCand;
    }
  }

  return minDRCand;  
}




int TopProducer::getTruthLabel(const reco::PFJetRef& recJet, edm::Handle<reco::GenParticleCollection> genParticles, float dRMatch , bool debug ){
  if ( debug ) {
    std::cout << " Mathcing reco jetPt:" << recJet->pt() << " jetEta:" << recJet->eta() << " jetPhi:" << recJet->phi() << std::endl;
  }

  for (reco::GenParticleCollection::const_iterator iGen = genParticles->begin();
       iGen != genParticles->end();
       ++iGen) {

    // From: (page 7/ Table 1.5.2)
    //https://indico.desy.de/indico/event/7142/session/9/contribution/31/material/slides/6.pdf
    //code range explanation:
    // 11 - 19 beam particles
    // 21 - 29 particles of the hardest subprocess
    // 31 - 39 particles of subsequent subprocesses in multiparton interactions
    // 41 - 49 particles produced by initial-state-showers
    // 51 - 59 particles produced by final-state-showers
    // 61 - 69 particles produced by beam-remnant treatment
    // 71 - 79 partons in preparation of hadronization process
    // 81 - 89 primary hadrons produced by hadronization process
    // 91 - 99 particles produced in decay process, or by Bose-Einstein effects

    // Do not want to match to the final particles in the shower
    if ( iGen->status() > 99 ) continue;
    
    // Only want to match to partons/leptons/bosons
    if ( iGen->pdgId() > 25 ) continue;

    float dR = reco::deltaR( recJet->eta(),recJet->phi(), iGen->eta(),iGen->phi() );

    if ( debug ) std::cout << " \t >> dR " << dR << " id:" << iGen->pdgId() << " status:" << iGen->status() << " nDaught:" << iGen->numberOfDaughters() << " pt:"<< iGen->pt() << " eta:" <<iGen->eta() << " phi:" <<iGen->phi() << " nMoms:" <<iGen->numberOfMothers()<< std::endl;

    if ( dR > dRMatch ) continue; 
    if ( debug ) std::cout << " Matched pdgID " << iGen->pdgId() << std::endl;

    return iGen->pdgId();

  } // gen particles 





  return -99;
}


float TopProducer::getBTaggingValue(const reco::PFJetRef& recJet, edm::Handle<edm::View<reco::Jet> >& recoJetCollection, edm::Handle<reco::JetTagCollection>& btagCollection, float dRMatch, bool debug ){

  // loop over jets
  for( edm::View<reco::Jet>::const_iterator jetToMatch = recoJetCollection->begin(); jetToMatch != recoJetCollection->end(); ++jetToMatch )
    {
      reco::Jet thisJet = *jetToMatch;
      float dR = reco::deltaR( recJet->eta(),recJet->phi(), thisJet.eta(),thisJet.phi() );
      if(dR > 0.1) continue;

      size_t idx = (jetToMatch - recoJetCollection->begin());
      edm::RefToBase<reco::Jet> jetRef = recoJetCollection->refAt(idx);

      if(debug) std::cout << "btag discriminator value = " << (*btagCollection)[jetRef] << std::endl;
      return (*btagCollection)[jetRef];
  
    }

  if(debug){
    std::cout << "ERROR  No btag match: " << std::endl;
    
    // loop over jets
    for( edm::View<reco::Jet>::const_iterator jetToMatch = recoJetCollection->begin(); jetToMatch != recoJetCollection->end(); ++jetToMatch )
      {
	const reco::Jet thisJet = *jetToMatch;
	std::cout << "\t Match attempt pt: " <<  thisJet.pt() << " vs " <<  recJet->pt()
		  << " eta: " << thisJet.eta() << " vs " << recJet->eta()
		  << "phi: "<< thisJet.phi() << " vs " << recJet->phi()
		  << std::endl;
	float dR = reco::deltaR( recJet->eta(),recJet->phi(), thisJet.eta(),thisJet.phi() );
	std::cout << "dR " << dR << std::endl;
      }
  }    

  return -99;
}
//define this as a plug-in
DEFINE_FWK_MODULE(TopProducer);
