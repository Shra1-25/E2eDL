/*#include "ProdTutorial/ProducerTest/plugins/QGProducer.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "ProdTutorial/ProducerTest/plugins/DetImgProducer.h"*/
#include "E2eDL/E2eDLrec/plugins/QGProducer.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "E2eDL/E2eDLrec/plugins/DetImgProducer.h"

using std::vector;
using std::cout;
using std::endl;

TH1D *h_dijet_jet_pT;
TH1D *h_dijet_jet_E;
TH1D *h_dijet_jet_eta;
TH1D *h_dijet_jet_m0;
TH1D *h_dijet_jet_nJet;
vector<float> vDijet_jet_pT_;
vector<float> vDijet_jet_m0_;
vector<float> vDijet_jet_eta_;
vector<float> vDijet_jet_phi_;
vector<float> vDijet_jet_truthLabel_;
vector<float> vDijet_jet_btaggingValue_;


// Initialize branches _____________________________________________________//
void QGProducer::branchesEvtSel_jet_dijet( TTree* tree, edm::Service<TFileService> &fs ) {

  h_dijet_jet_pT    = fs->make<TH1D>("h_jet_pT"  , "p_{T};p_{T};Particles", 100,  0., 500.);
  h_dijet_jet_E     = fs->make<TH1D>("h_jet_E"   , "E;E;Particles"        , 100,  0., 800.);
  h_dijet_jet_eta   = fs->make<TH1D>("h_jet_eta" , "#eta;#eta;Particles"  , 100, -5., 5.);
  h_dijet_jet_nJet  = fs->make<TH1D>("h_jet_nJet", "nJet;nJet;Events"     ,  10,  0., 10.);
  h_dijet_jet_m0    = fs->make<TH1D>("h_jet_m0"  , "m0;m0;Events"         , 100,  0., 100.);

  tree->Branch("jetPt",          &vDijet_jet_pT_);
  tree->Branch("jetM",           &vDijet_jet_m0_);
  tree->Branch("jetEta",         &vDijet_jet_eta_);
  tree->Branch("jetPhi",         &vDijet_jet_phi_);
  tree->Branch("jet_truthLabel", &vDijet_jet_truthLabel_);
  tree->Branch("jet_btagValue",  &vDijet_jet_btaggingValue_);

} // branchesEvtSel_jet_dijet()

// Run jet selection _____________________________________________________//
bool QGProducer::runEvtSel_jet_dijet( const edm::Event& iEvent, const edm::EventSetup& iSetup )
{

  edm::Handle<reco::PFJetCollection> jets;
  iEvent.getByToken(jetCollectionT_, jets);

  vJetIdxs.clear();
  vDijet_jet_pT_.clear();
  vDijet_jet_m0_.clear();
  vDijet_jet_eta_.clear();
  vDijet_jet_phi_.clear();
  vDijet_jet_truthLabel_.clear();
  vDijet_jet_btaggingValue_.clear();

  int nJet = 0;
  // Loop over jets
  for ( unsigned iJ(0); iJ != jets->size(); ++iJ ) {

    reco::PFJetRef iJet( jets, iJ );
    if ( std::abs(iJet->pt()) < minJetPt_ ) continue;
    if ( std::abs(iJet->eta()) > maxJetEta_) continue;
    if ( debug ) std::cout << " >> jet[" << iJ << "]Pt:" << iJet->pt() << " jetE:" << iJet->energy() << " jetM:" << iJet->mass() << std::endl;

    vJetIdxs.push_back(iJ);

    nJet++;
    if ( (nJets_ > 0) && (nJet >= nJets_) ) break;

  } // jets


  if ( debug ) {
    for(int thisJetIdx : vJetIdxs)
      std::cout << " >> vJetIdxs:" << thisJetIdx << std::endl;
  }

  if ( (nJets_ > 0) && (nJet != nJets_) ){
    if ( debug ) std::cout << " Fail jet multiplicity:  " << nJet << " != " << nJets_ << std::endl;
    return false;
  }

  if ( vJetIdxs.size() == 0){
    if ( debug ) std::cout << " No passing jets...  " << std::endl;
    return false;
  }

  if ( debug ) std::cout << " >> has_jet_dijet: passed" << std::endl;
  return true;

} // runEvtSel_jet_dijet() 

// Fill branches and histograms _____________________________________________________//
void QGProducer::fillEvtSel_jet_dijet( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {

  edm::Handle<reco::PFJetCollection> jets;
  iEvent.getByToken(jetCollectionT_, jets);

  edm::Handle<reco::GenParticleCollection> genParticles;
  iEvent.getByToken( genParticleCollectionT_, genParticles );

  edm::Handle<edm::View<reco::Jet> > recoJetCollection;
  iEvent.getByToken(recoJetsT_, recoJetCollection);

  edm::Handle<reco::JetTagCollection> btagDiscriminators;
  iEvent.getByToken(jetTagCollectionT_, btagDiscriminators);

  edm::Handle<std::vector<reco::CandIPTagInfo> > ipTagInfo;
  iEvent.getByToken(ipTagInfoCollectionT_, ipTagInfo);

  edm::Handle<reco::VertexCollection> vertexInfo;
  iEvent.getByToken(vertexCollectionT_, vertexInfo);
  //const reco::VertexCollection& vtxs = *vertexInfo;
	      
 
  h_dijet_jet_nJet->Fill( vJetIdxs.size() );
  // Fill branches and histograms 
  for(int thisJetIdx : vJetIdxs){
    reco::PFJetRef thisJet( jets, thisJetIdx );
    if ( debug ) std::cout << " >> Jet[" << thisJetIdx << "] Pt:" << thisJet->pt() << std::endl;

    if (debug) std::cout << " Passed Jet " << " Pt:" << thisJet->pt()  << " Eta:" << thisJet->eta()  << " Phi:" << thisJet->phi() 
			 << " jetE:" << thisJet->energy() << " jetM:" << thisJet->mass() 
			 << " photonE:" << thisJet->photonEnergy()  
			 << " chargedHadronEnergy:" << thisJet->chargedHadronEnergy()  
			 << " neutralHadronEnergy :" << thisJet->neutralHadronEnergy()
			 << " electronEnergy	 :" << thisJet->electronEnergy	()
			 << " muonEnergy		 :" << thisJet->muonEnergy		()
			 << " HFHadronEnergy	 :" << thisJet->HFHadronEnergy	()
			 << " HFEMEnergy		 :" << thisJet->HFEMEnergy		()
			 << " chargedEmEnergy	 :" << thisJet->chargedEmEnergy	()
			 << " chargedMuEnergy	 :" << thisJet->chargedMuEnergy	()
			 << " neutralEmEnergy	 :" << thisJet->neutralEmEnergy	()
			 << std::endl;

    h_dijet_jet_pT->Fill( std::abs(thisJet->pt()) );
    h_dijet_jet_E->Fill( thisJet->energy() );
    h_dijet_jet_m0->Fill( thisJet->mass() );
    h_dijet_jet_eta->Fill( thisJet->eta() );
    vDijet_jet_pT_.push_back( std::abs(thisJet->pt()) );
    vDijet_jet_m0_.push_back( thisJet->mass() );
    vDijet_jet_eta_.push_back( thisJet->eta() );
    vDijet_jet_phi_.push_back( thisJet->phi() );

    int truthLabel = getTruthLabel(thisJet,genParticles,0.4, false);
    if(truthLabel == -99){
      std::cout << "ERROR truth -99" << std::endl;
      getTruthLabel(thisJet,genParticles,0.4, true);
    }
    vDijet_jet_truthLabel_      .push_back(truthLabel);

    float bTagValue = getBTaggingValue(thisJet,recoJetCollection,btagDiscriminators);
    vDijet_jet_btaggingValue_      .push_back(bTagValue);

  }//vJetIdxs


} // fillEvtSel_jet_dijet()
