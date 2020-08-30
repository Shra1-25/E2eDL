//#include "ProdTutorial/ProducerTest/plugins/DetImgProducer.h"
#include "E2eDL/E2eDLrec/plugins/DetImgProducer.h"

// Fill EB rec hits ////////////////////////////////
// Store event rechits in a vector of length equal
// to number of crystals in EB (ieta:170 x iphi:360)

//TProfile2D *hEB_energy;
//TProfile2D *hEB_time;
//std::vector<float> vEB_energy_;
//std::vector<float> vEB_time_;

// Initialize branches _____________________________________________________//
void DetImgProducer::branchesEB ( TTree* tree, edm::Service<TFileService> &fs ) {

  // Branches for images
  tree->Branch("EB_energy", &vEB_energy_);
  tree->Branch("EB_time",   &vEB_time_);
  //tree->Branch("Photon_Collection", &vEB_frame);

  // Histograms for monitoring
  hEB_energy = fs->make<TProfile2D>("EB_energy", "E(i#phi,i#eta);i#phi;i#eta",
      EB_IPHI_MAX  , EB_IPHI_MIN-1, EB_IPHI_MAX,
      2*EB_IETA_MAX,-EB_IETA_MAX,   EB_IETA_MAX );
  hEB_time = fs->make<TProfile2D>("EB_time", "t(i#phi,i#eta);i#phi;i#eta",
      EB_IPHI_MAX  , EB_IPHI_MIN-1, EB_IPHI_MAX,
      2*EB_IETA_MAX,-EB_IETA_MAX,   EB_IETA_MAX );
  /*hEB_frame = fs->make<TProfile2D>("Photon_Collection", "E(i#phi,i#eta);i#phi;i#eta",
      EB_IPHI_MAX  , EB_IPHI_MIN-1, EB_IPHI_MAX,
      2*EB_IETA_MAX,-EB_IETA_MAX,   EB_IETA_MAX );*/

} // branchesEB()

// Fill EB rechits _________________________________________________________________//
void DetImgProducer::fillEB ( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {

  int iphi_, ieta_, idx_; // rows:ieta, cols:iphi
  float energy_;

  vEB_energy_.assign( EBDetId::kSizeForDenseIndexing, 0. );
  vEB_time_.assign( EBDetId::kSizeForDenseIndexing, 0. );

  edm::Handle<EcalRecHitCollection> EBRecHitsH_;
  iEvent.getByToken( EBRecHitCollectionT_, EBRecHitsH_);

  // Fill EB rechits 
  for ( EcalRecHitCollection::const_iterator iRHit = EBRecHitsH_->begin();
        iRHit != EBRecHitsH_->end(); ++iRHit ) {

    energy_ = iRHit->energy();
    if ( energy_ <= zs ) continue;
    // Get detector id and convert to histogram-friendly coordinates
    EBDetId ebId( iRHit->id() );
    iphi_ = ebId.iphi() - 1;
    ieta_ = ebId.ieta() > 0 ? ebId.ieta()-1 : ebId.ieta();
    // Fill histograms for monitoring 
    hEB_energy->Fill( iphi_,ieta_,energy_ );
    hEB_time->Fill( iphi_,ieta_,iRHit->time() );
    // Get Hashed Index: provides convenient 
    // index mapping from [ieta][iphi] -> [idx]
    idx_ = ebId.hashedIndex(); // (ieta_+EB_IETA_MAX)*EB_IPHI_MAX + iphi_
    // Fill vectors for images
    vEB_energy_[idx_] = energy_;
    vEB_time_[idx_] = iRHit->time();

  } // EB rechits
 } // fillEB()
