/*#include "ProdTutorial/ProducerTest/plugins/QGProducer.h"
#include "ProdTutorial/ProducerTest/plugins/DetImgProducer.h"*/
#include "E2eDL/E2eDLrec/plugins/QGProducer.h"
#include "E2eDL/E2eDLrec/plugins/DetImgProducer.h"
// Run event selection ////////////////////////////////

TH1F *h_m0;
TH1F *h_nJet;
TH1F *h_phoPt; 
TH1F *h_phoE;
TH1F *h_phoEta;
TH1F *h_phoR9;
TH1F *h_phoSieie;
TH1F *h_phoMva;
TH1F *h_jetPt;
TH1F *h_jetE;
TH1F *h_jetEta;

unsigned long long eventId_;
unsigned int runId_;
unsigned int lumiId_;
float m0_;
//float nJet_;
float diPhoE_;
float diPhoPt_;
std::vector<float> vFC_inputs_;

float m0cut = 90.;
//float m0cut = 80.;

// Initialize branches _____________________________________________________//
void QGProducer::branchesEvtSel ( TTree* tree, edm::Service<TFileService> &fs ) {

  h_m0     = fs->make<TH1F>("h_m0"    , "m0;m0;Events"         ,  50, m0cut, m0cut+150.);

  h_phoPt  = fs->make<TH1F>("h_phoPt" , "p_{T};p_{T};Particles", 100,  0., 500.);
  h_phoE   = fs->make<TH1F>("h_phoE"  , "E;E;Particles"        , 100,  0., 800.);
  h_phoEta = fs->make<TH1F>("h_phoEta", "#eta;#eta;Particles"  , 100, -5., 5.);
  h_phoR9  = fs->make<TH1F>("h_phoR9" , "R9;R9;Particles"  , 50, 0., 1.);
  h_phoSieie  = fs->make<TH1F>("h_phoSieie" , "Sieie;Sieie;Particles"  , 50, 0., 0.1);
  //h_phoMva = fs->make<TH1F>("h_phoMva", "#mva;#mva;Particles"  , 100, -1., 1.);
  /*
  h_jetPt  = fs->make<TH1F>("h_jetPt" , "p_{T};p_{T};Particles", 100,  0., 500.);
  h_jetE   = fs->make<TH1F>("h_jetE"  , "E;E;Particles"        , 100,  0., 800.);
  h_jetEta = fs->make<TH1F>("h_jetEta", "#eta;#eta;Particles"  , 100, -5., 5.);
  h_nJet   = fs->make<TH1F>("h_nJet"  , "nJet;nJet;Events"     ,  10,  0.,  10.);
  */

  tree->Branch("eventId",        &eventId_);
  tree->Branch("runId",          &runId_);
  tree->Branch("lumiId",         &lumiId_);
  tree->Branch("m0",             &m0_);
  //tree->Branch("nJet",           &nJet_);
  tree->Branch("FC_inputs",      &vFC_inputs_);
  tree->Branch("diPhoE",         &diPhoE_);
  tree->Branch("diPhoPt",        &diPhoPt_);

} // branchesEvtSel()

// Run event selection _______________________________________________________________//
bool QGProducer::runEvtSel ( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {

  edm::Handle<reco::PhotonCollection> photons;
  //edm::Handle<pat::PhotonCollection> photons;
  iEvent.getByToken( photonCollectionT_, photons );

  int nPhoTrg = 0;

  // Perform photon pre-selection
  float m0; //dR;
  float leadPhoPt = 0.;
  math::PtEtaPhiELorentzVectorD vDiPho;
  std::vector<int> vPhoIdxs;
  for ( unsigned int iP = 0; iP < photons->size(); iP++ ) {
    reco::PhotonRef iPho( photons, iP );
    //pat::PhotonRef iPho( photons, iP );
    if ( std::abs(iPho->pt()) < 18. ) continue;
    //std::cout << iPho->full5x5_sigmaIetaIeta() << std::endl;
    if ( std::abs(iPho->eta()) > 1.44 ) continue;
    if ( iPho->r9() < 0.5 ) continue;
    if ( iPho->hadTowOverEm() > 0.07 ) continue;
    if ( iPho->full5x5_sigmaIetaIeta() > 0.0105 ) continue;
    if ( iPho->hasPixelSeed() == true ) continue;
    //if ( std::abs(iPho->eta()) > 2.1 ) continue;
    //if ( std::abs(iPho->eta()) > 1.44 && std::abs(iPho->eta()) < 1.57 ) continue;
    if (debug) std::cout << " >> pT:" << iPho->pt() << " eta:" << iPho->eta() << " phi: " << iPho->phi() << " E:" << iPho->energy() << std::endl;
    nPhoTrg++;
    if ( std::abs(iPho->pt()) > leadPhoPt ) leadPhoPt = std::abs(iPho->pt()); 
    vDiPho += iPho->p4();
    vPhoIdxs.push_back( iP );

  } // reco photons 
  m0 = vDiPho.mass();
  if ( m0 < m0cut ) return false;
  if ( nPhoTrg != 2 ) return false;
  if ( leadPhoPt < 30. ) return false;

  // Apply selection
  int nPho = 0;
  int leadPho = -1;
  leadPhoPt = 0.;
  for ( int iP = 0; iP < nPhoTrg; iP++ ) {
    reco::PhotonRef iPho( photons, vPhoIdxs[iP] );
    //pat::PhotonRef iPho( photons, vPhoIdxs[iP] );
    // Get leading photon pt
    if ( std::abs(iPho->pt()) > leadPhoPt ) {
      leadPhoPt = std::abs(iPho->pt()); 
      leadPho = iP;
    }
    // Minimum pt/m0 cut
    if ( std::abs(iPho->pt()) < m0/4. ) continue;
    nPho++;
  }
  if ( nPho != 2 ) return false;
  if ( leadPhoPt < m0/3 ) return false;
  nPho = nPhoTrg;
  //std::cout << " n:" << nPho << " m0:" << m0 << std::endl;

  /*
  // Count number of jets
  edm::Handle<reco::PFJetCollection> jets;
  iEvent.getByToken( jetCollectionT_, jets );
  bool isDRIsolated;
  int nJet = 0;
  std::vector<int> vJetIdxs;
  for ( unsigned int iJ = 0; iJ < jets->size(); iJ++ ) {
    reco::PFJetRef iJet( jets, iJ );
    if ( std::abs(iJet->pt()) < 30. ) continue;
    if ( std::abs(iJet->eta()) > 2.5 ) continue;
    // deltaR check
    isDRIsolated = true;
    for ( int iP = 0; iP < nPho; iP++ ) {
      reco::PhotonRef iPho( photons, vPhoIdxs[iP] );
      dR = reco::deltaR( iJet->eta(),iJet->phi(), iPho->eta(),iPho->phi() );
      if ( dR < 0.4 ) {
        isDRIsolated = false;
        break;
      }
    }
    if ( !isDRIsolated ) continue;
    //std::cout << " >> pT:" << iJet->pt() << " eta:" << iJet->eta() << " phi: " << iJet->phi() << " E:" << iJet->energy() << std::endl;
    nJet++;
    vJetIdxs.push_back( iJ );
  }
  //if ( nJet != 2 ) return false;
  */

  // Get photon order
  int ptOrder[2] = {0, 1};
  if ( leadPho == 1 ) {
    ptOrder[0] = 1;
    ptOrder[1] = 0;
  }
  //std::cout << " ptOrder[:]: " << ptOrder[0] << " " << ptOrder[1] << std::endl;

  // Fill kinematic variables
  //h_nJet->Fill( nJet );
  h_m0->Fill( m0 );
  diPhoE_  = 0.;
  diPhoPt_ = 0.;
  float dphi[2] = {0., 0.};
  vFC_inputs_.clear();
  for ( int iP = 0; iP < nPho; iP++ ) {
    reco::PhotonRef iPho( photons, vPhoIdxs[ptOrder[iP]] );
    //pat::PhotonRef iPho( photons, vPhoIdxs[ptOrder[iP]] );
    h_phoPt->Fill( iPho->pt() ); 
    h_phoE->Fill( iPho->energy() );
    h_phoEta->Fill( iPho->eta() ); 
    h_phoR9->Fill( iPho->r9() ); 
    h_phoSieie->Fill( iPho->full5x5_sigmaIetaIeta() ); 
    //h_phoMva->Fill( iPho->pfMVA() ); 
    //std::cout << iPho->pfMVA() << std::endl;
    diPhoE_  += std::abs( iPho->energy() );
    diPhoPt_ += std::abs( iPho->pt() );
    vFC_inputs_.push_back( iPho->pt()/m0 );
    vFC_inputs_.push_back( iPho->eta() );
    dphi[iP] = iPho->phi();
  }
  vFC_inputs_.push_back( TMath::Cos(reco::deltaPhi(dphi[0], dphi[1])) );

  /*
  for ( int iJ = 0; iJ < nJet; iJ++ ) {
    reco::PFJetRef iJet( jets, vJetIdxs[iJ] );
    h_jetPt->Fill( iJet->pt() ); 
    h_jetE->Fill( iJet->energy() );
    h_jetEta->Fill( iJet->eta() ); 
  }
  */

  // Write out event
  m0_ = m0;
  //nJet_ = nJet;
  eventId_ = iEvent.id().event();
  runId_ = iEvent.id().run();
  lumiId_ = iEvent.id().luminosityBlock();

  return true;

} // runEvtSel()

/*
bool RecHitAnalyzer::runEvtSel ( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {
  edm::Handle<reco::PhotonCollection> photons;
  iEvent.getByToken(photonCollectionT_, photons);
  edm::Handle<reco::GenParticleCollection> genParticles;
  iEvent.getByToken(genParticleCollectionT_, genParticles);
  int nPho = 0;
  std::cout << "PhoCol.size: " << photons->size() << std::endl;
  math::XYZTLorentzVector vDiPho;
  for (reco::GenParticleCollection::const_iterator iGen = genParticles->begin();
       iGen != genParticles->end();
       ++iGen) {
    // ID cuts
    if ( std::abs(iGen->pdgId()) != 22 ) continue;
    if ( iGen->status() != 1 ) continue; // NOT the same as Pythia status
    if ( !iGen->mother() ) continue;
    if ( iGen->mother()->pdgId() != 35 && iGen->mother()->pdgId() != 22 ) continue;
    //std::cout << "status:" <<iGen->status() << " pT:" << iGen->pt() << " eta:" << iGen->eta() << " E:" << iGen->energy() << " mothId:" << iGen->mother()->pdgId() << std::endl;
    nPho++;
    vDiPho += iGen->p4();
  } // genParticle loop: count good photons
  // Require exactly 2 gen-level photons
  // Indifferent about photons of status != 1
  std::cout << "nPho:" << nPho << std::endl;
  if ( nPho != 4 ) return false;
  //if ( vDiPho.mass() < 80. ) return false;
  // Fill loop
  for (reco::GenParticleCollection::const_iterator iGen = genParticles->begin();
       iGen != genParticles->end();
       ++iGen) {
    // PDG ID cut
    if ( std::abs(iGen->pdgId()) != 22 ) continue;
    if ( iGen->status() != 1 ) continue; // NOT the same as Pythia status
    if ( !iGen->mother() ) continue;
    if ( iGen->mother()->pdgId() != 35 && iGen->mother()->pdgId() != 22 ) continue;
    // Fill histograms
    h_pT-> Fill( iGen->pt()      );
    h_E->  Fill( iGen->energy()  );
    h_eta->Fill( iGen->eta()     );
  } // genParticle loop: fill hist
  h_m0->Fill( vDiPho.mass() );
  std::cout << " m0: " << vDiPho.mass() <<" (" << vDiPho.T() << ")" << std::endl;
  m0_ = vDiPho.mass();
  // Write out event ID
  eventId_ = iEvent.id().event();
  return true;
} // runEvtSel()
*/

/*
//____ Apply event selection cuts _____//
bool RecHitAnalyzer::runSelections_H24G ( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {
  edm::Handle<reco::PhotonCollection> photons;
  iEvent.getByToken(photonCollectionT_, photons);
  edm::Handle<reco::GenParticleCollection> genParticles;
  iEvent.getByToken(genParticleCollectionT_, genParticles);
  int nPho = 0;
  //bool isHiggs = true;
  //bool isDecayed = true;
  float etaCut = 1.44;
  //float etaCut = 2.3;
  //float etaCut = 2.5;
  float ptCut = 18.;
  //float dRCut = 0.4;
  //float dR, dEta, dPhi;
  std::cout << " >> recoPhoCol.size: " << photons->size() << std::endl;
  math::PtEtaPhiELorentzVectorD vDiPho;
  //math::XYZTLorentzVector vDiPho;
  std::vector<float> vE, vPt, vEta, vPhi;
  float leadPhoPt = 0;
  // Apply diphoton trigger-like selection
  for(reco::PhotonCollection::const_iterator iPho = photons->begin();
      iPho != photons->end();
      ++iPho) {
    // Kinematic cuts
    if ( std::abs(iPho->eta()) > etaCut ) continue;
    if ( std::abs(iPho->pt()) < ptCut ) continue;
    nPho++;
    // Record kinematics
    vDiPho += iPho->p4();
    vE.push_back(   iPho->energy() );
    vPt.push_back(  iPho->pt()     );
    vEta.push_back( iPho->eta()    );
    vPhi.push_back( iPho->phi()    );
    if ( std::abs(iPho->pt()) > leadPhoPt ) leadPhoPt = std::abs(iPho->pt());
  } // recoPhotons
  // Apply diphoton trigger-like selection
  if ( nPho != 2 ) return false;
  if ( leadPhoPt < 30. ) return false;
  m0_ = vDiPho.mass();
  if ( m0_ < 90. ) return false;
  //// Check dR
  //dEta = std::abs( vEta[0] - vEta[1] );
  //dPhi = std::abs( vPhi[0] - vPhi[1] );
  //dR = TMath::Power(dEta,2.) + TMath::Power(dPhi,2.);
  //dR = TMath::Sqrt(dR);
  //if ( dR < dRCut ) return false;
  std::cout << " >> passed trigger" << std::endl;
  // Apply good photon selection
  int i = 0;
  nPho = 0;
  leadPhoPt = 0.;
  for(reco::PhotonCollection::const_iterator iPho = photons->begin();
      iPho != photons->end();
      ++iPho) {
    // Kinematic cuts
    if ( std::abs(iPho->eta()) > etaCut ) continue;
    //if ( std::abs(iPho->eta()) > 1.44 && std::abs(iPho->eta()) < 1.57 ) continue;
    if ( std::abs(iPho->pt()) < m0_/4. ) continue;
    if ( std::abs(iPho->pt()) > leadPhoPt ) leadPhoPt = std::abs(iPho->pt());
    vPho_[i] = iPho->p4();
    nPho++;
    i++;
  } // recoPhotons
  if ( nPho != 2 ) return false;
  if ( leadPhoPt < m0_/3. ) return false;
  // Fill histograms
  diPhoE_  = 0.;
  diPhoPt_ = 0.;
  for(int i = 0; i < 2; i++) {
    std::cout << " >> pT:" << vPt[i] << " eta:" << vEta[i] << " phi: " << vPhi[i] << " E:" << vE[i] << std::endl;
    h_pT-> Fill( vPt[i]  );
    h_E->  Fill( vE[i]   );
    h_eta->Fill( vEta[i] );
    diPhoE_  += vE[i];
    diPhoPt_ += vPt[i];
  }
  //vDiPho = vDiPho/m0_;
  //vDiPho = vDiPho/diPhoE_;
  //vDiPho = vDiPho/diPhoPt_;
  //h_m0->Fill( vDiPho.mass() );
  //std::cout << " >> m0: " << vDiPho.mass() << " diPhoPt: " << diPhoPt_ << " diPhoE: " << diPhoE_ << std::endl;
  h_m0->Fill( m0_ );
  std::cout << " >> m0: " << m0_ << " diPhoPt: " << diPhoPt_ << " diPhoE: " << diPhoE_ << std::endl;
  //for (reco::GenParticleCollection::const_iterator iGen = genParticles->begin();
  //     iGen != genParticles->end();
  //     ++iGen) {
  //  // ID cuts
  //  if ( std::abs(iGen->pdgId()) != 22 ) continue;
  //  if ( iGen->status() != 1 ) continue; // NOT the same as Pythia status
  //  if ( !iGen->mother() ) continue;
  //  if ( iGen->mother()->pdgId() != 35 && iGen->mother()->pdgId() != 22 ) continue;
  //  //std::cout << "status:" <<iGen->status() << " pT:" << iGen->pt() << " eta:" << iGen->eta() << " E:" << iGen->energy() << " mothId:" << iGen->mother()->pdgId() << std::endl;
  //  // Kinematic cuts
  //  if ( std::abs(iGen->eta()) > etaCut ) continue;
  //  if ( std::abs(iGen->pt()) < ptCut ) continue;
  //  nPho++;
  //  vDiPho += iGen->p4();
  //  if ( std::abs(iGen->pt()) > leadPt ) leadPt = std::abs(iGen->pt());
  //} // genParticle loop: count good photons
  //// Require exactly 2 gen-level photons
  //// Indifferent about photons of status != 1
  //std::cout << "GenCollection: " << nPho << std::endl;
  //if ( nPho != 4 ) return false;
  ////if ( vDiPho.mass() < 80. ) return false;
  //// Fill loop
  //for (reco::GenParticleCollection::const_iterator iGen = genParticles->begin();
  //     iGen != genParticles->end();
  //     ++iGen) {
  //  // PDG ID cut
  //  if ( std::abs(iGen->pdgId()) != 22 ) continue;
  //  if ( iGen->status() != 1 ) continue; // NOT the same as Pythia status
  //  if ( !iGen->mother() ) continue;
  //  if ( iGen->mother()->pdgId() != 35 && iGen->mother()->pdgId() != 22 ) continue;
  //  // Kinematic cuts
  //  if ( std::abs(iGen->eta()) > etaCut ) continue;
  //  if ( std::abs(iGen->pt()) < ptCut ) continue;
  //  std::cout << " pT:" << iGen->pt() << " eta:" << iGen->eta() << " phi: " << iGen->phi() << " E:" << iGen->energy() << std::endl;
  //  // Fill histograms
  //  //h_pT-> Fill( iGen->pt()      );
  //  h_E->  Fill( iGen->energy()  );
  //  h_eta->Fill( iGen->eta()     );
  //} // genParticle loop: fill hist
  //h_pT-> Fill( leadPt );
  //h_m0->Fill( vDiPho.mass() );
  //std::cout << "leadPt: " << leadPt << std::endl;
  //std::cout << " m0: " << vDiPho.mass() <<" (" << vDiPho.T() << ")" << std::endl;
  //m0_ = vDiPho.mass();
  //std::cout << "PhoCol.size: " << photons->size() << std::endl;
  //for(reco::PhotonCollection::const_iterator iPho = photons->begin();
  //    iPho != photons->end();
  //    ++iPho) {
  //  if ( std::abs(iPho->eta()) > etaCut ) continue;
  //  //if ( std::abs(iPho->pt()) < ptCut-2. ) continue;
  //  std::cout << " pT:" << iPho->pt() << " eta:" << iPho->eta() << " phi: " << iPho->phi() << " E:" << iPho->energy() << std::endl;
  //}
  // Check leading jet in reco jet collection
  edm::Handle<reco::PFJetCollection> jets;
  iEvent.getByToken(jetCollectionT_, jets);
  std::cout << " >> PFJetCol.size: " << jets->size() << std::endl;
  for(reco::PFJetCollection::const_iterator iJet = jets->begin();
      iJet != jets->end();
      ++iJet) {
    //std::cout << " pT:" << iJet->pt() << " eta:" << iJet->eta() << " phi: " << iJet->phi() << " E:" << iJet->energy() << std::endl;
  }
  // Check leading jet in gen jet collection
  float leadJetPt = 0.;
  edm::Handle<reco::GenJetCollection> genJets;
  iEvent.getByToken(genJetCollectionT_, genJets);
  std::cout << " >> GenJetCol.size: " << jets->size() << std::endl;
  for(reco::GenJetCollection::const_iterator iJet = genJets->begin();
      iJet != genJets->end();
      ++iJet) {
    if ( std::abs(iJet->pt()) > leadJetPt ) leadJetPt = std::abs(iJet->pt());
    //std::cout << " >> pT:" << iJet->pt() << " eta:" << iJet->eta() << " phi: " << iJet->phi() << " E:" << iJet->energy() << std::endl;
  }
  std::cout << " >> leadJetPt: " << leadJetPt << std::endl;
  h_leadJetPt->Fill( leadJetPt );
  return true;
} // runSelections_H24G
//____ Apply event selection cuts _____//
bool RecHitAnalyzer::runSelections_H2GG ( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {
  edm::Handle<reco::PhotonCollection> photons;
  iEvent.getByToken(photonCollectionT_, photons);
  edm::Handle<reco::GenParticleCollection> genParticles;
  iEvent.getByToken(genParticleCollectionT_, genParticles);
  int nPho = 0;
  //bool isHiggs = true;
  //bool isDecayed = true;
  //float etaCut = 1.4;
  //float etaCut = 2.3;
  //float etaCut = 5.;
  //float ptCut = 0.;
  //float dRCut = 0.4;
  //float dR, dEta, dPhi;
  std::cout << "PhoCol.size: " << photons->size() << std::endl;
  math::XYZTLorentzVector vDiPho;
  for (reco::GenParticleCollection::const_iterator iGen = genParticles->begin();
       iGen != genParticles->end();
       ++iGen) {
    // ID cuts
    if ( std::abs(iGen->pdgId()) != 22 ) continue;
    if ( iGen->status() != 1 ) continue; // NOT the same as Pythia status
    if ( !iGen->mother() ) continue;
    if ( iGen->mother()->pdgId() != 25 && iGen->mother()->pdgId() != 22 ) continue;
    //std::cout << "status:" <<iGen->status() << " pT:" << iGen->pt() << " eta:" << iGen->eta() << " E:" << iGen->energy() << " mothId:" << iGen->mother()->pdgId() << std::endl;
    nPho++;
    vDiPho += iGen->p4();
  } // genParticle loop: count good photons
  // Require exactly 2 gen-level photons
  // Indifferent about photons of status != 1
  std::cout << nPho << std::endl;
  if ( nPho != 2 ) return false;
  //if ( vDiPho.mass() < 80. ) return false;
  // Fill loop
  for (reco::GenParticleCollection::const_iterator iGen = genParticles->begin();
       iGen != genParticles->end();
       ++iGen) {
    // PDG ID cut
    if ( std::abs(iGen->pdgId()) != 22 ) continue;
    if ( iGen->status() != 1 ) continue; // NOT the same as Pythia status
    if ( !iGen->mother() ) continue;
    if ( iGen->mother()->pdgId() != 25 && iGen->mother()->pdgId() != 22 ) continue;
    // Fill histograms
    h_pT-> Fill( iGen->pt()      );
    h_E->  Fill( iGen->energy()  );
    h_eta->Fill( iGen->eta()     );
  } // genParticle loop: fill hist
  h_m0->Fill( vDiPho.mass() );
  std::cout << " m0: " << vDiPho.mass() <<" (" << vDiPho.T() << ")" << std::endl;
  m0_ = vDiPho.mass();
  return true;
} // runSelections_H2GG()
//____ Apply event selection cuts _____//
bool RecHitAnalyzer::runSelections ( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {
  // Initialize data collection pointers
  edm::Handle<reco::PhotonCollection> photons;
  iEvent.getByToken(photonCollectionT_, photons);
  edm::Handle<reco::GenParticleCollection> genParticles;
  iEvent.getByToken(genParticleCollectionT_, genParticles);
  int nPho = 0;
  //float etaCut = 1.4;
  float etaCut = 2.3;
  float ptCut = 25.;
  std::cout << "PhoCol.size: " << photons->size() << std::endl;
  for(reco::PhotonCollection::const_iterator iPho = photons->begin();
      iPho != photons->end();
      ++iPho) {
    // Kinematic cuts
    if ( std::abs(iPho->eta()) > etaCut ) continue;
    if ( std::abs(iPho->pt()) < ptCut ) continue;
    nPho++;
  } // recoPhotons
  // Require at least 2 passed reco photons
  // Will also include PU photons
  if ( nPho < 2 ) return false;
  float dRCut = 0.4;
  float dEta, dPhi, dR;
  for(reco::PhotonCollection::const_iterator iPho = photons->begin();
      iPho != photons->end();
      ++iPho) {
    // Kinematic cuts
    if ( std::abs(iPho->eta()) > etaCut ) continue;
    if ( std::abs(iPho->pt()) < ptCut ) continue;
    std::cout << "nPho:" << nPho << " pT:" << iPho->pt() << " eta:" << iPho->eta() << " E:" << iPho->energy() << std::endl;
    for (reco::GenParticleCollection::const_iterator iGen = genParticles->begin();
        iGen != genParticles->end();
        ++iGen) {
      // ID cuts
      if ( std::abs(iGen->pdgId()) != 22 ) continue;
      if ( iGen->status() != 1 ) continue; // NOT the same as Pythia status
      if ( !iGen->mother() ) continue;
      if ( std::abs(iGen->mother()->status()) != 44 && std::abs(iGen->mother()->status()) != 23 ) continue;
      // Match by dR
      dEta = std::abs( iPho->eta() - iGen->eta() );
      dPhi = std::abs( iPho->phi() - iGen->phi() );
      dR = TMath::Power(dEta,2.) + TMath::Power(dPhi,2.);
      dR = TMath::Sqrt(dR);
      if ( dR < dRCut ) {
        h_pT-> Fill( iGen->pt()      );
        h_E->  Fill( iGen->energy()  );
        h_eta->Fill( iGen->eta()     );
        break;
      }
    } // genParticle loop: count good photons
  } // recoPhotons
  return true;
} // runSelections()
*/
