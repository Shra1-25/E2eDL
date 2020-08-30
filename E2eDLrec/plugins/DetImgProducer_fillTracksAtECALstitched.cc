//#include "ProdTutorial/ProducerTest/plugins/DetImgProducer.h"
#include "E2eDL/E2eDLrec/plugins/DetImgProducer.h"

// Fill Tracks into stitched EEm_EB_EEp image //////////////////////
// Store all Track positions into a stitched EEm_EB_EEp image 

// All Tracks 
TH2F *hEvt_EE_tracksPt[nEE];
//TH2F *hEvt_EE_tracksQPt[nEE];
std::vector<float> vECAL_tracksPt_;
//std::vector<float> vECAL_tracksQPt_;

// All Tracks from the PV
/*TH2F *hEvt_EE_tracksPt_PV[nEE];
TH2F *hEvt_EE_tracksQPt_PV[nEE];
TH2F *hEvt_EE_tracksd0_PV[nEE];
TH2F *hEvt_EE_tracksz0_PV[nEE];
TH2F *hEvt_EE_tracksd0sig_PV[nEE];
TH2F *hEvt_EE_tracksz0sig_PV[nEE];*/
//std::vector<float> vECAL_tracksPt_PV_;
/*std::vector<float> vECAL_tracksQPt_PV_;
std::vector<float> vECAL_tracksd0_PV_;
std::vector<float> vECAL_tracksz0_PV_;
std::vector<float> vECAL_tracksd0sig_PV_;
std::vector<float> vECAL_tracksz0sig_PV_;*/

// All Tracks not from the PV
//TH2F *hEvt_EE_tracksPt_nPV[nEE];
//TH2F *hEvt_EE_tracksQPt_nPV[nEE];
//std::vector<float> vECAL_tracksPt_nPV_;
//std::vector<float> vECAL_tracksQPt_nPV_;

TProfile2D *hECAL_tracks;
TProfile2D *hECAL_tracksPt;
//TProfile2D *hECAL_tracksQPt;

/*TH1F *hECAL_tracksz0;
TH1F *hECAL_tracksz0_s;
TH1F *hECAL_tracksz0BeforeQuality;*/


// Initialize branches _______________________________________________________________//
void DetImgProducer::branchesTracksAtECALstitched ( TTree* tree, edm::Service<TFileService> &fs ) {

  // Branches for images
  tree->Branch("ECAL_tracksPt",    &vECAL_tracksPt_);
  /*tree->Branch("ECAL_tracksQPt",    &vECAL_tracksQPt_);

  tree->Branch("ECAL_tracksPt_PV",       &vECAL_tracksPt_PV_);
  tree->Branch("ECAL_tracksQPt_PV",      &vECAL_tracksQPt_PV_);
  tree->Branch("ECAL_tracksd0_PV",       &vECAL_tracksd0_PV_);
  tree->Branch("ECAL_tracksz0_PV",       &vECAL_tracksz0_PV_);
  tree->Branch("ECAL_tracksd0sig_PV",    &vECAL_tracksd0sig_PV_);
  tree->Branch("ECAL_tracksz0sig_PV",    &vECAL_tracksz0sig_PV_);

  tree->Branch("ECAL_tracksPt_nPV",      &vECAL_tracksPt_nPV_);
  tree->Branch("ECAL_tracksQPt_nPV",     &vECAL_tracksQPt_nPV_);*/

  static const std::string strIndex[2] = {"m","p"};
  static const double* binIndex[2] = {eta_bins_EEm, eta_bins_EEp};

  // Intermediate helper histogram (single event only)
  for(unsigned int idx = 0; idx < 2; ++idx){
    hEvt_EE_tracksPt[idx] = new TH2F(("evt_EE"+strIndex[idx]+"_tracksPt").c_str(), "E(i#phi,i#eta);i#phi;i#eta",
				     EB_IPHI_MAX, -TMath::Pi(), TMath::Pi(),
				     5*(HBHE_IETA_MAX_HE-1-HBHE_IETA_MAX_EB), binIndex[idx] );

    /*hEvt_EE_tracksQPt[idx] = new TH2F(("evt_EE"+strIndex[idx]+"_tracksQPt").c_str(), "qxPt(i#phi,i#eta);i#phi;i#eta",
				       EB_IPHI_MAX, -TMath::Pi(), TMath::Pi(),
				       5*(HBHE_IETA_MAX_HE-1-HBHE_IETA_MAX_EB), binIndex[idx] );


    hEvt_EE_tracksPt_PV[idx] = new TH2F(("evt_EE"+strIndex[idx]+"_tracksPt_PV").c_str(), "E(i#phi,i#eta);i#phi;i#eta",
					EB_IPHI_MAX, -TMath::Pi(), TMath::Pi(),
					5*(HBHE_IETA_MAX_HE-1-HBHE_IETA_MAX_EB), binIndex[idx] );

    hEvt_EE_tracksQPt_PV[idx] = new TH2F(("evt_EE"+strIndex[idx]+"_tracksQPt_PV").c_str(), "qxPt(i#phi,i#eta);i#phi;i#eta",
					 EB_IPHI_MAX, -TMath::Pi(), TMath::Pi(),
					 5*(HBHE_IETA_MAX_HE-1-HBHE_IETA_MAX_EB), binIndex[idx] );

    hEvt_EE_tracksd0_PV[idx] = new TH2F(("evt_EE"+strIndex[idx]+"_tracksd0_PV").c_str(), "d0(i#phi,i#eta);i#phi;i#eta",
					EB_IPHI_MAX, -TMath::Pi(), TMath::Pi(),
					5*(HBHE_IETA_MAX_HE-1-HBHE_IETA_MAX_EB), binIndex[idx] );

    hEvt_EE_tracksz0_PV[idx] = new TH2F(("evt_EE"+strIndex[idx]+"_tracksz0_PV").c_str(), "z0(i#phi,i#eta);i#phi;i#eta",
					 EB_IPHI_MAX, -TMath::Pi(), TMath::Pi(),
					 5*(HBHE_IETA_MAX_HE-1-HBHE_IETA_MAX_EB), binIndex[idx] );


    hEvt_EE_tracksd0sig_PV[idx] = new TH2F(("evt_EE"+strIndex[idx]+"_tracksd0_PV").c_str(), "d0sig(i#phi,i#eta);i#phi;i#eta",
					   EB_IPHI_MAX, -TMath::Pi(), TMath::Pi(),
					   5*(HBHE_IETA_MAX_HE-1-HBHE_IETA_MAX_EB), binIndex[idx] );

    hEvt_EE_tracksz0sig_PV[idx] = new TH2F(("evt_EE"+strIndex[idx]+"_tracksz0_PV").c_str(), "z0sig(i#phi,i#eta);i#phi;i#eta",
					   EB_IPHI_MAX, -TMath::Pi(), TMath::Pi(),
					   5*(HBHE_IETA_MAX_HE-1-HBHE_IETA_MAX_EB), binIndex[idx] );

    hEvt_EE_tracksPt_nPV[idx] = new TH2F(("evt_EE"+strIndex[idx]+"_tracksPt_nPV").c_str(), "E(i#phi,i#eta);i#phi;i#eta",
					 EB_IPHI_MAX, -TMath::Pi(), TMath::Pi(),
					 5*(HBHE_IETA_MAX_HE-1-HBHE_IETA_MAX_EB), binIndex[idx] );

    hEvt_EE_tracksQPt_nPV[idx] = new TH2F(("evt_EE"+strIndex[idx]+"_tracksQPt_nPV").c_str(), "qxPt(i#phi,i#eta);i#phi;i#eta",
					  EB_IPHI_MAX, -TMath::Pi(), TMath::Pi(),
					  5*(HBHE_IETA_MAX_HE-1-HBHE_IETA_MAX_EB), binIndex[idx] );*/

  }




  // Histograms for monitoring
  hECAL_tracks = fs->make<TProfile2D>("ECAL_tracks", "E(i#phi,i#eta);i#phi;i#eta",
      EB_IPHI_MAX,    EB_IPHI_MIN-1, EB_IPHI_MAX,
      2*ECAL_IETA_MAX_EXT, -ECAL_IETA_MAX_EXT,   ECAL_IETA_MAX_EXT );

  hECAL_tracksPt = fs->make<TProfile2D>("ECAL_tracksPt", "E(i#phi,i#eta);i#phi;i#eta",
      EB_IPHI_MAX,    EB_IPHI_MIN-1, EB_IPHI_MAX,
      2*ECAL_IETA_MAX_EXT, -ECAL_IETA_MAX_EXT,   ECAL_IETA_MAX_EXT );

  /*hECAL_tracksQPt = fs->make<TProfile2D>("ECAL_tracksQPt", "qxPt(i#phi,i#eta);i#phi;i#eta",
      EB_IPHI_MAX,    EB_IPHI_MIN-1, EB_IPHI_MAX,
      2*ECAL_IETA_MAX_EXT, -ECAL_IETA_MAX_EXT,   ECAL_IETA_MAX_EXT );

  hECAL_tracksz0              = fs->make<TH1F>("ECAL_tracksz0", "z0;z0;Entries", 100,-20,20);
  hECAL_tracksz0_s            = fs->make<TH1F>("ECAL_tracksz0_s", "z0;z0;Entries", 100,-0.2,0.2);
  hECAL_tracksz0BeforeQuality = fs->make<TH1F>("ECAL_tracksz0BeforeQuality", "z0;z0;Entries", 100,-20,20);*/

} // branchesTracksAtECALstitched()

// Function to map EE(phi,eta) histograms to ECAL(iphi,ieta) vector _______________________________//
void fillTracksAtECAL_with_EEproj (std::vector<float>& vECAL_tracksPt_, TH2F *hEvt_EE_tracksPt_, /*TH2F *hEvt_EE_tracksQPt_, 
				   TH2F *hEvt_EE_tracksPt_PV_, TH2F *hEvt_EE_tracksQPt_PV_, TH2F *hEvt_EE_tracksd0_PV_, TH2F *hEvt_EE_tracksz0_PV_, TH2F *hEvt_EE_tracksd0sig_PV_, TH2F *hEvt_EE_tracksz0sig_PV_, 
				   TH2F *hEvt_EE_tracksPt_nPV_, TH2F *hEvt_EE_tracksQPt_nPV_,*/ 
				   int ieta_global_offset, int ieta_signed_offset ) {

  int ieta_global_, ieta_signed_;
  int ieta_, iphi_, idx_;
  float trackPt_;//, trackQPt_;
  //float trackPt_PV_, trackQPt_PV_, trackd0_PV_, trackz0_PV_, trackd0sig_PV_, trackz0sig_PV_;
  //float trackPt_nPV_, trackQPt_nPV_;


  for (int ieta = 1; ieta < hEvt_EE_tracksPt_->GetNbinsY()+1; ieta++) {
    ieta_ = ieta - 1;
    ieta_global_ = ieta_ + ieta_global_offset;
    ieta_signed_ = ieta_ + ieta_signed_offset;
    for (int iphi = 1; iphi < hEvt_EE_tracksPt_->GetNbinsX()+1; iphi++) {

      trackPt_        = hEvt_EE_tracksPt_->GetBinContent( iphi, ieta );
      /*trackQPt_       = hEvt_EE_tracksQPt_->GetBinContent( iphi, ieta );
		      
      trackPt_PV_     = hEvt_EE_tracksPt_PV_->GetBinContent( iphi, ieta );
      trackQPt_PV_    = hEvt_EE_tracksQPt_PV_->GetBinContent( iphi, ieta );
      trackd0_PV_     = hEvt_EE_tracksd0_PV_->GetBinContent( iphi, ieta );
      trackz0_PV_     = hEvt_EE_tracksz0_PV_->GetBinContent( iphi, ieta );
      trackd0sig_PV_  = hEvt_EE_tracksd0sig_PV_->GetBinContent( iphi, ieta );
      trackz0sig_PV_  = hEvt_EE_tracksz0sig_PV_->GetBinContent( iphi, ieta );

      trackPt_nPV_    = hEvt_EE_tracksPt_nPV_->GetBinContent( iphi, ieta );
      trackQPt_nPV_   = hEvt_EE_tracksQPt_nPV_->GetBinContent( iphi, ieta );*/

      if ( (trackPt_ <= zs) ) continue;
      // NOTE: EB iphi = 1 does not correspond to physical phi = -pi so need to shift!
      iphi_ = iphi  + 5*38; // shift
      iphi_ = iphi_ > EB_IPHI_MAX ? iphi_-EB_IPHI_MAX : iphi_; // wrap-around
      iphi_ = iphi_ - 1;
      idx_  = ieta_global_*EB_IPHI_MAX + iphi_;
      // Fill vector for image
      vECAL_tracksPt_[idx_]  = trackPt_;
      /*vECAL_tracksQPt_[idx_] = trackQPt_;

      vECAL_tracksPt_PV_[idx_]    = trackPt_PV_;
      vECAL_tracksQPt_PV_[idx_]   = trackQPt_PV_;
      vECAL_tracksd0_PV_[idx_]    = trackd0_PV_;
      vECAL_tracksz0_PV_[idx_]    = trackz0_PV_;
      vECAL_tracksd0sig_PV_[idx_] = trackd0sig_PV_;
      vECAL_tracksz0sig_PV_[idx_] = trackz0sig_PV_;

      vECAL_tracksPt_nPV_[idx_]  = trackPt_nPV_;
      vECAL_tracksQPt_nPV_[idx_] = trackQPt_nPV_;*/

      // Fill histogram for monitoring
      hECAL_tracks->Fill( iphi_, ieta_signed_, 1. );
      hECAL_tracksPt->Fill( iphi_, ieta_signed_, trackPt_ );
      //hECAL_tracksQPt->Fill( iphi_, ieta_signed_, trackQPt_ );

    } // iphi_
  } // ieta_

} // fillTracksAtECAL_with_EEproj

// Fill stitched EE-, EB, EE+ rechits ________________________________________________________//
void DetImgProducer::fillTracksAtECALstitched ( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {

  int iphi_, ieta_, iz_, idx_;
  int ieta_global, ieta_signed;
  int ieta_global_offset, ieta_signed_offset;
  float eta, phi, trackPt_; //trackQPt_, trackd0_, trackz0_, trackd0sig_, trackz0sig_;
  GlobalPoint pos;

  vECAL_tracksPt_.assign( 2*ECAL_IETA_MAX_EXT*EB_IPHI_MAX, 0. );
  /*vECAL_tracksQPt_.assign( 2*ECAL_IETA_MAX_EXT*EB_IPHI_MAX, 0. );

  vECAL_tracksPt_PV_.assign( 2*ECAL_IETA_MAX_EXT*EB_IPHI_MAX, 0. );
  vECAL_tracksQPt_PV_.assign( 2*ECAL_IETA_MAX_EXT*EB_IPHI_MAX, 0. );
  vECAL_tracksd0_PV_.assign( 2*ECAL_IETA_MAX_EXT*EB_IPHI_MAX, 0. );
  vECAL_tracksz0_PV_.assign( 2*ECAL_IETA_MAX_EXT*EB_IPHI_MAX, 0. );
  vECAL_tracksd0sig_PV_.assign( 2*ECAL_IETA_MAX_EXT*EB_IPHI_MAX, 0. );
  vECAL_tracksz0sig_PV_.assign( 2*ECAL_IETA_MAX_EXT*EB_IPHI_MAX, 0. );

  vECAL_tracksPt_nPV_.assign( 2*ECAL_IETA_MAX_EXT*EB_IPHI_MAX, 0. );
  vECAL_tracksQPt_nPV_.assign( 2*ECAL_IETA_MAX_EXT*EB_IPHI_MAX, 0. );*/

  for ( int iz(0); iz < nEE; ++iz ){
    hEvt_EE_tracksPt[iz]->Reset();
    /*hEvt_EE_tracksQPt[iz]->Reset();

    hEvt_EE_tracksPt_PV[iz]->Reset();
    hEvt_EE_tracksQPt_PV[iz]->Reset();
    hEvt_EE_tracksd0_PV[iz]->Reset();
    hEvt_EE_tracksz0_PV[iz]->Reset();
    hEvt_EE_tracksd0sig_PV[iz]->Reset();
    hEvt_EE_tracksz0sig_PV[iz]->Reset();

    hEvt_EE_tracksPt_nPV[iz]->Reset();
    hEvt_EE_tracksQPt_nPV[iz]->Reset();*/
  }


  edm::Handle<EcalRecHitCollection> EBRecHitsH_;
  iEvent.getByToken( EBRecHitCollectionT_, EBRecHitsH_ );

  edm::Handle<EcalRecHitCollection> EERecHitsH_;
  iEvent.getByToken( EERecHitCollectionT_, EERecHitsH_ );

  edm::ESHandle<CaloGeometry> caloGeomH_;
  iSetup.get<CaloGeometryRecord>().get( caloGeomH_ );
  const CaloGeometry* caloGeom = caloGeomH_.product();

  edm::Handle<reco::TrackCollection> tracksH_;
  iEvent.getByToken( trackCollectionT_, tracksH_ );

  edm::Handle<reco::VertexCollection> vertexInfo;
  iEvent.getByToken(vertexCollectionT_, vertexInfo);
  const reco::VertexCollection& vtxs = *vertexInfo;

  reco::Track::TrackQuality tkQt_ = reco::Track::qualityByName("highPurity");

  for ( reco::TrackCollection::const_iterator iTk = tracksH_->begin();
        iTk != tracksH_->end(); ++iTk ) {
    if ( !(iTk->quality(tkQt_)) ) continue;
    eta = iTk->eta();
    phi = iTk->phi();
    if ( std::abs(eta) > 3. ) continue;
    DetId id( spr::findDetIdECAL( caloGeom, eta, phi, false ) );
    if ( id.subdetId() == EcalBarrel ) continue;
    if ( id.subdetId() == EcalEndcap ) {
      iz_ = (eta > 0.) ? 1 : 0;
      // Fill intermediate helper histogram by eta,phi
      hEvt_EE_tracksPt[iz_]->Fill( phi, eta, iTk->pt() );
      //hEvt_EE_tracksQPt[iz_]->Fill( phi, eta, iTk->charge()*iTk->pt() );


      //const double z0 = ( !vtxs.empty() ? iTk->dz(vtxs[0].position()) : iTk->dz() );

      // if is PV
      /*if(fabs(z0) < z0PVCut_){

	const double d0 = ( !vtxs.empty() ? iTk->dxy(vtxs[0].position()) : iTk->dxy() );
			    
	hEvt_EE_tracksPt_PV[iz_] ->Fill( phi, eta, iTk->pt() );
	hEvt_EE_tracksQPt_PV[iz_]->Fill( phi, eta, iTk->charge()*iTk->pt() );

	float d0sig = d0/iTk->dxyError();
	float z0sig = z0/iTk->dzError();
	hEvt_EE_tracksd0_PV[iz_] ->Fill( phi, eta, d0 );
	hEvt_EE_tracksz0_PV[iz_] ->Fill( phi, eta, z0 );
	hEvt_EE_tracksd0sig_PV[iz_] ->Fill( phi, eta, d0sig );
	hEvt_EE_tracksz0sig_PV[iz_] ->Fill( phi, eta, z0sig );


      }else{	// if is not PV

	hEvt_EE_tracksPt_nPV[iz_] ->Fill( phi, eta, iTk->pt() );
	hEvt_EE_tracksQPt_nPV[iz_]->Fill( phi, eta, iTk->charge()*iTk->pt() );
      }*/


    }
  } // tracks


  // Map EE-(phi,eta) to bottom part of ECAL(iphi,ieta)
  ieta_global_offset = 0;
  ieta_signed_offset = -ECAL_IETA_MAX_EXT;
  fillTracksAtECAL_with_EEproj( vECAL_tracksPt_, hEvt_EE_tracksPt[0], /*hEvt_EE_tracksQPt[0], 
				hEvt_EE_tracksPt_PV[0], hEvt_EE_tracksQPt_PV[0], hEvt_EE_tracksd0_PV[0], hEvt_EE_tracksz0_PV[0], hEvt_EE_tracksd0sig_PV[0], hEvt_EE_tracksz0sig_PV[0], 
				hEvt_EE_tracksPt_nPV[0], hEvt_EE_tracksQPt_nPV[0],*/ 
				ieta_global_offset, ieta_signed_offset );

  // Fill middle part of ECAL(iphi,ieta) with the EB rechits.
  ieta_global_offset = 55;

  for ( reco::TrackCollection::const_iterator iTk = tracksH_->begin();
        iTk != tracksH_->end(); ++iTk ) { 

    eta = iTk->eta();
    phi = iTk->phi();
    trackPt_ = iTk->pt();
    /*trackQPt_ = (iTk->charge()*iTk->pt());
    trackd0_ =  ( !vtxs.empty() ? iTk->dxy(vtxs[0].position()) : iTk->dxy() );
    trackz0_ =  ( !vtxs.empty() ? iTk->dz(vtxs[0].position()) : iTk->dz() );
    trackd0sig_ = trackd0_/iTk->dxyError();
    trackz0sig_ = trackz0_/iTk->dzError();
    hECAL_tracksz0BeforeQuality->Fill(trackz0_);
    if ( !(iTk->quality(tkQt_)) ) continue;
    hECAL_tracksz0  ->Fill(trackz0_);
    hECAL_tracksz0_s->Fill(trackz0_);*/

    if ( std::abs(eta) > 3. ) continue;
    DetId id( spr::findDetIdECAL( caloGeom, eta, phi, false ) );
    if ( id.subdetId() == EcalEndcap ) continue;
    if ( id.subdetId() == EcalBarrel ) { 
      EBDetId ebId( id );
      iphi_ = ebId.iphi() - 1;
      ieta_ = ebId.ieta() > 0 ? ebId.ieta()-1 : ebId.ieta();
      if ( trackPt_ <= zs ) continue;
      // Fill vector for image
      ieta_signed = ieta_;
      ieta_global = ieta_ + EB_IETA_MAX + ieta_global_offset;
      idx_ = ieta_global*EB_IPHI_MAX + iphi_; 
      vECAL_tracksPt_[idx_] += trackPt_;
      //vECAL_tracksQPt_[idx_] += trackQPt_;

      /*if(fabs(trackz0_) < z0PVCut_){
	vECAL_tracksPt_PV_[idx_] += trackPt_;
	vECAL_tracksQPt_PV_[idx_] += trackQPt_;

	vECAL_tracksd0_PV_[idx_] += trackd0_;
	vECAL_tracksz0_PV_[idx_] += trackz0_;
	vECAL_tracksd0sig_PV_[idx_] += trackd0sig_;
	vECAL_tracksz0sig_PV_[idx_] += trackz0sig_;

      }else{
	vECAL_tracksPt_nPV_[idx_] += trackPt_;
	vECAL_tracksQPt_nPV_[idx_] += trackQPt_;
      }*/

      // Fill histogram for monitoring
      hECAL_tracks->Fill( iphi_, ieta_signed, 1. );
      hECAL_tracksPt->Fill( iphi_, ieta_signed, trackPt_ );
      //hECAL_tracksQPt->Fill( iphi_, ieta_signed, trackQPt_ );
    }

  } // EB Tracks



  // Map EE+(phi,eta) to upper part of ECAL(iphi,ieta)
  ieta_global_offset = ECAL_IETA_MAX_EXT + EB_IETA_MAX;
  ieta_signed_offset = EB_IETA_MAX;
  fillTracksAtECAL_with_EEproj( vECAL_tracksPt_, hEvt_EE_tracksPt[1], /*hEvt_EE_tracksQPt[1], 
				hEvt_EE_tracksPt_PV[1], hEvt_EE_tracksQPt_PV[1], hEvt_EE_tracksd0_PV[1], hEvt_EE_tracksz0_PV[1], hEvt_EE_tracksd0sig_PV[1], hEvt_EE_tracksz0sig_PV[1], 
				hEvt_EE_tracksPt_nPV[1], hEvt_EE_tracksQPt_nPV[1],*/ 
				ieta_global_offset, ieta_signed_offset );

} // fillTracksAtECALstitched()
