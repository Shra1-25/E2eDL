/*#include "ProdTutorial/ProducerTest/plugins/QGProducer.h"
#include "ProdTutorial/ProducerTest/plugins/DetImgProducer.h"*/
#include "E2eDL/E2eDLrec/plugins/QGProducer.h"
#include "E2eDL/E2eDLrec/plugins/DetImgProducer.h"

using std::vector;

const int search_window = 7;
const int image_padding = 12;
vector<int>   vFailedJetIdx_;
unsigned int jet_runId_;
unsigned int jet_lumiId_;
unsigned long long jet_eventId_;

//const std::string jetSelection = "dijet_gg_qq"; // TODO: put switch at cfg level
const std::string jetSelection = "dijet";

void QGProducer::branchesEvtSel_jet ( TTree* tree, edm::Service<TFileService> &fs ) {

  tree->Branch("eventId",        &jet_eventId_);
  tree->Branch("runId",          &jet_runId_);
  tree->Branch("lumiId",         &jet_lumiId_);
  tree->Branch("jetSeed_iphi",   &vJetSeed_iphi_);
  tree->Branch("jetSeed_ieta",   &vJetSeed_ieta_);

  // Fill branches in explicit jet selection
  /*if ( jetSelection == "dijet_gg_qq" ) {
    branchesEvtSel_jet_dijet_gg_qq( tree, fs );
  } else {
    branchesEvtSel_jet_dijet( tree, fs );
  }*/

} // branchesEvtSel_jet()

bool QGProducer::runEventSel_jet ( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {

   edm::ESHandle<CaloGeometry> caloGeomH_;
   iSetup.get<CaloGeometryRecord>().get( caloGeomH_ );
   const CaloGeometry* caloGeom = caloGeomH_.product();
	
   edm::Handle<HBHERecHitCollection> HBHERecHitsH_;
   iEvent.getByToken( HBHERecHitCollectionT_, HBHERecHitsH_ );
	
   edm::Handle<reco::PFJetCollection> jets;
   iEvent.getByToken(jetCollectionT_, jets);
   if ( debug ) std::cout << " >> PFJetCol.size: " << jets->size() << std::endl;
   
   
   float seedE;
   int iphi_, ieta_, ietaAbs_;
   int nJet = 0;
   
   vFailedJetIdx_.clear();
   vJetSeed_iphi_.clear();
   vJetSeed_ieta_.clear();
   
   std::cout<<" >> Reading and selecting Jets from "<<jets->size()<<" jet seeds: "<<std::endl;
   for (unsigned iJ=0;iJ<jets->size();iJ++){
   	bool keepJet = true;
    	iphi_ = -1;
    	ieta_ = -1;
	reco::PFJetRef iJet( jets, iJ );
	   
	// Jet selection criteria
    	if ( std::abs(iJet->pt())  < minJetPt_ ) {keepJet = false; 
		std::cout<<"     * Selection failed at Jet index: "<<iJ+1<<" because abs(pt) < minJetPt_ --> pt: "<<std::abs(iJet->pt())<<" minJetPt_: "<<minJetPt_<<". Adding "<<iphi_<<" to JetSeediphi and "<<ieta_<<" JetSeedieta vectors."<<std::endl;
		vJetSeed_iphi_.push_back( iphi_ );
    		vJetSeed_ieta_.push_back( ieta_ );
    		nJet++;					 
	}
    	else if ( std::abs(iJet->eta()) > maxJetEta_ ) {keepJet = false; 
		std::cout<<"     * Selection failed at Jet index: "<<iJ+1<<" because abs(eta) > maxJetEta_ --> eta: "<<std::abs(iJet->eta())<<" maxJetEta_: "<<maxJetEta_<<". Adding "<<iphi_<<" to JetSeediphi and "<<ieta_<<" JetSeedieta vectors."<<std::endl;
		vJetSeed_iphi_.push_back( iphi_ );
    		vJetSeed_ieta_.push_back( ieta_ );
    		nJet++;					  
	}
	//std::cout<<" # keepJet: "<<keepJet<<" --> pt: "<<std::abs(iJet->pt())<<", eta: "<<std::abs(iJet->eta())<<", minJetPt: "<<minJetPt_<<", maxJetEta: "<<maxJetEta_<<std::endl;
	if (keepJet){
	if ( debug ) std::cout << " >> jet[" << iJ+1 << "]Pt:" << iJet->pt()  << " Eta:" << iJet->eta()  << " Phi:" << iJet->phi() 
			   << " jetE:" << iJet->energy() << " jetM:" << iJet->mass() << std::endl;
	HcalDetId hId( spr::findDetIdHCAL( caloGeom, iJet->eta(), iJet->phi(), false ) );
    	if ( hId.subdet() != HcalBarrel && hId.subdet() != HcalEndcap ){
      		vFailedJetIdx_.push_back(iJ);
      		continue;
    	}
	   
	HBHERecHitCollection::const_iterator iRHit( HBHERecHitsH_->find(hId) );
    	seedE = ( iRHit == HBHERecHitsH_->end() ) ? 0. : iRHit->energy() ;
    	HcalDetId seedId = hId;
    	if ( debug ) std::cout << " >> hId.ieta:" << hId.ieta() << " hId.iphi:" << hId.iphi() << " E:" << seedE << std::endl;
	
	// Look for the most energetic HBHE tower deposit within a search window
	for ( int ieta = 0; ieta < search_window; ieta++ ) {

      		ieta_ = hId.ieta() - (search_window/2)+ieta;

      		if ( std::abs(ieta_) > HBHE_IETA_MAX_HE-1 ) continue;
      		if ( std::abs(ieta_) < HBHE_IETA_MIN_HB ) continue;

      		HcalSubdetector subdet_ = std::abs(ieta_) > HBHE_IETA_MAX_HB ? HcalEndcap : HcalBarrel;

      		for ( int iphi = 0; iphi < search_window; iphi++ ) {

        		iphi_ = hId.iphi() - (search_window/2)+iphi;

        		// iphi should wrap around
        		if ( iphi_ > HBHE_IPHI_MAX ) {
          		iphi_ = iphi_-HBHE_IPHI_MAX;
        		} else if ( iphi_ < HBHE_IPHI_MIN ) {
          		iphi_ = HBHE_IPHI_MAX-abs(iphi_); 
        		}
        		// Skip non-existent and lower energy towers 
        		HcalDetId hId_( subdet_, ieta_, iphi_, 1 );
        		HBHERecHitCollection::const_iterator iRHit( HBHERecHitsH_->find(hId_) );
        		if ( iRHit == HBHERecHitsH_->end() ) continue;
        		if ( iRHit->energy() <= seedE ) continue;
        		if ( debug ) std::cout << " !! hId.ieta:" << hId_.ieta() << " hId.iphi:" << hId_.iphi() << " E:" << iRHit->energy() << std::endl;

        		seedE = iRHit->energy();
        		seedId = hId_;

      		} // iphi 
    	} // ieta
	
    	// NOTE: HBHE iphi = 1 does not correspond to EB iphi = 1!
    	// => Need to shift by 2 HBHE towers: HBHE::iphi: [1,...,71,72]->[3,4,...,71,72,1,2]
    	iphi_  = seedId.iphi() + 2; // shift
    	iphi_  = iphi_ > HBHE_IPHI_MAX ? iphi_-HBHE_IPHI_MAX : iphi_; // wrap-around
    	iphi_  = iphi_ - 1; // make histogram-friendly
    	ietaAbs_  = seedId.ietaAbs() == HBHE_IETA_MAX_HE ? HBHE_IETA_MAX_HE-1 : seedId.ietaAbs(); // last HBHE ieta embedded
    	ieta_  = seedId.zside() > 0 ? ietaAbs_-1 : -ietaAbs_;
    	ieta_  = ieta_+HBHE_IETA_MAX_HE-1;

    	// If the seed is too close to the edge of HE, discard event
    	// Required to keep the seed at the image center
    	if ( HBHE_IETA_MAX_HE-1 - ietaAbs_ < image_padding ) { 
      	if ( debug ) std::cout << " Fail HE edge cut " << std::endl;
      	vFailedJetIdx_.push_back(iJ);
	std::cout<<"     * Failed Jet seed at index: "<<iJ+1<<" seed is too close to the edge of HE. Adding -1 to JetSeediphi and JetSeedieta vectors."<<std::endl;
	ieta_=-1;
	iphi_=-1;
	vJetSeed_iphi_.push_back( iphi_ );
    	vJetSeed_ieta_.push_back( ieta_ );
    	nJet++;
      	continue;
    	}
    	// Save position of most energetic HBHE tower
    	// in EB-aligned coordinates
    	if ( debug ) std::cout << " !! ieta_:" << ieta_ << " iphi_:" << iphi_ << " ietaAbs_:" << ietaAbs_ << " E:" << seedE << std::endl;
	
    	vJetSeed_iphi_.push_back( iphi_ );
    	vJetSeed_ieta_.push_back( ieta_ );
    	nJet++;}
   } // good jets	
	
	
// Remove jets that failed the Seed cuts 
  /*for(int failedJetIdx : vFailedJetIdx_)
    vJetIdxs.erase(std::remove(vJetIdxs.begin(),vJetIdxs.end(),failedJetIdx),vJetIdxs.end());
  if ( vJetIdxs.size() == 0){
    if ( debug ) std::cout << " No passing jets...  " << std::endl;
    return false;
  }
  
  if ( (nJets_ > 0) && nJet != nJets_ ) return false;
  if ( debug ) std::cout << " >> analyze: passed" << std::endl;
  jet_eventId_ = iEvent.id().event();
  jet_runId_ = iEvent.id().run();
  jet_lumiId_ = iEvent.id().luminosityBlock();
  if ( jetSelection == "dijet_gg_qq" ) {
    fillEvtSel_jet_dijet_gg_qq( iEvent, iSetup );
  } else {
    fillEvtSel_jet_dijet( iEvent, iSetup );
  }*/

  return true;

} // runEvtSel_jet()
