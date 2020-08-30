/*#include "ProdTutorial/ProducerTest/plugins/TopProducer.h"
#include "ProdTutorial/ProducerTest/plugins/DetImgProducer.h"*/
/*#include "E2eDL/E2eDLrec/plugins/TopProducer.h"
#include "E2eDL/E2eDLrec/plugins/DetImgProducer.h
using std::vector;

const unsigned nJets = 2; //TODO: use cfg level nJets_
TH1D *h_ggqq_jet_pT;
TH1D *h_ggqq_jet_E;
TH1D *h_ggqq_jet_eta;
TH1D *h_ggqq_jet_m0;
TH1D *h_ggqq_jet_nJet;
TH1D *h_nGG;
TH1D *h_nQQ;
vector<float> v_ggqq_jet_m0_;
vector<float> v_ggqq_jet_pt_;
vector<float> v_ggqq_jetIsQuark_;
vector<float> v_ggqq_jetPdgIds_;

vector<float> v_ggqq_subJetE_[nJets];
vector<float> v_ggqq_subJetPx_[nJets];
vector<float> v_ggqq_subJetPy_[nJets];
vector<float> v_ggqq_subJetPz_[nJets];

// Initialize branches _____________________________________________________//
void TopProducer::branchesEvtSel_jet_dijet_gg_qq ( TTree* tree, edm::Service<TFileService> &fs ) {

  h_ggqq_jet_pT    = fs->make<TH1D>("h_jet_pT"  , "p_{T};p_{T};Particles", 100,  0., 500.);
  h_ggqq_jet_E     = fs->make<TH1D>("h_jet_E"   , "E;E;Particles"        , 100,  0., 800.);
  h_ggqq_jet_eta   = fs->make<TH1D>("h_jet_eta" , "#eta;#eta;Particles"  , 100, -5., 5.);
  h_ggqq_jet_nJet  = fs->make<TH1D>("h_jet_nJet", "nJet;nJet;Events"     ,  10,  0., 10.);
  h_ggqq_jet_m0    = fs->make<TH1D>("h_jet_m0"  , "m0;m0;Events"         , 100,  0., 100.);
  h_nGG       = fs->make<TH1D>("h_nGG"     , "nGG;nGG;Events"       ,   3,  0.,   3.);
  h_nQQ       = fs->make<TH1D>("h_nQQ"     , "nQQ;nQQ;Events"       ,   3,  0.,   3.);

  tree->Branch("jetM",       &v_ggqq_jet_m0_);
  tree->Branch("jetPt",      &v_ggqq_jet_pt_);
  tree->Branch("jetIsQuark", &v_ggqq_jetIsQuark_);
  tree->Branch("jetPdgIds",  &v_ggqq_jetPdgIds_);

  char hname[50];
  for ( unsigned iJ = 0; iJ != nJets; iJ++ ) {
    sprintf(hname, "subJet%d_E", iJ);
    tree->Branch(hname,            &v_ggqq_subJetE_[iJ]);
    sprintf(hname, "subJet%d_Px", iJ);
    tree->Branch(hname,            &v_ggqq_subJetPx_[iJ]);
    sprintf(hname, "subJet%d_Py", iJ);
    tree->Branch(hname,            &v_ggqq_subJetPy_[iJ]);
    sprintf(hname, "subJet%d_Pz", iJ);
    tree->Branch(hname,            &v_ggqq_subJetPz_[iJ]);
  }

} // branchesEvtSel_jet_dijet_gg_qq()

// Run jet selection _____________________________________________________//
bool TopProducer::runEvtSel_jet_dijet_gg_qq( const edm::Event& iEvent, const edm::EventSetup& iSetup )
{

  edm::Handle<reco::GenParticleCollection> genParticles;
  iEvent.getByToken( genParticleCollectionT_, genParticles );

  edm::Handle<reco::PFJetCollection> jets;
  iEvent.getByToken(jetCollectionT_, jets);
  float dR;

  vJetIdxs.clear();
  v_ggqq_jetPdgIds_.clear();
  /*
  edm::Handle<reco::GenJetCollection> genJets;
  iEvent.getByToken(genJetCollectionT_, genJets);
  std::vector<float> v_ggqq_jetFakePhoIdxs;
  */

  /*unsigned nGG = 0;
  unsigned nQQ = 0;

  if ( debug ) std::cout << " >>>>>>>>>>>>>>>>>>>> evt:" << std::endl;
  for (reco::GenParticleCollection::const_iterator iGen = genParticles->begin();
      iGen != genParticles->end();
      ++iGen) {

    if ( iGen->numberOfMothers() != 2 ) continue;
    //if ( iGen->status() != 3 ) continue; // pythia6: 3 = hard scatter particle
    if ( iGen->status() != 23 ) continue; // pythia8: 23 = outgoing particle from hard scatter
    if ( debug ) std::cout << " >> id:" << iGen->pdgId() << " status:" << iGen->status() << " nDaught:" << iGen->numberOfDaughters() << " pt:"<< iGen->pt() << " eta:" <<iGen->eta() << " phi:" <<iGen->phi() << " nMoms:" <<iGen->numberOfMothers()<< std::endl;

    // Loop over jets
    for ( unsigned iJ(0); iJ != jets->size(); ++iJ ) {
      //if ( debug ) std::cout << " >>>>>> jet[" << iJ << "]" << std::endl;
      reco::PFJetRef iJet( jets, iJ );
      if ( std::abs(iJet->pt())  < minJetPt_ ) continue;
      if ( std::abs(iJet->eta()) > maxJetEta_ ) continue;
      dR = reco::deltaR( iJet->eta(),iJet->phi(), iGen->eta(),iGen->phi() );
      if ( debug ) {
        std::cout << " >>>>>> jet[" << iJ << "] Pt:" << iJet->pt() << " jetEta:" << iJet->eta() << " jetPhi:" << iJet->phi()
        << " dR:" << dR << std::endl;
      }
      if ( dR > 0.4 ) continue;
      vJetIdxs.push_back( iJ );
      v_ggqq_jetPdgIds_.push_back( std::abs(iGen->pdgId()) );
      if ( debug ) {
        std::cout << " >>>>>> DR matched: jet[" << iJ << "] pdgId:" << std::abs(iGen->pdgId()) << std::endl;
      }
      break;
    } // reco jets

  } // gen particles

  // Check jet multiplicity
  if ( vJetIdxs.size() != nJets ) return false;

  // Check jet identities
  for ( unsigned iJ(0); iJ != nJets; ++iJ ) {
    if ( v_ggqq_jetPdgIds_[iJ] < 4 ) nQQ++;
    else if ( v_ggqq_jetPdgIds_[iJ] == 21 ) nGG++;
  }
  if ( vJetIdxs[0] == vJetIdxs[1] ) return false; // protect against double matching: only valid for nJets==2
  if ( nQQ+nGG != nJets ) return false; // require dijet
  if ( nQQ != nJets && nGG != nJets ) return false; // require gg or qq final state
  //if ( nQQ != 1 && nGG != 1 ) return false; // require qg final state

  if ( debug ) std::cout << " >> has_jet_dijet_gg_qq: passed" << std::endl;
  return true;

} // runEvtSel_jet_dijet_gg_qq()

// Fill branches and histograms _____________________________________________________//
void TopProducer::fillEvtSel_jet_dijet_gg_qq ( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {

  edm::Handle<reco::PFJetCollection> jets;
  iEvent.getByToken(jetCollectionT_, jets);

  h_ggqq_jet_nJet->Fill( vJetIdxs.size() );

  unsigned nGG = 0;
  unsigned nQQ = 0;
  v_ggqq_jet_pt_.clear();
  v_ggqq_jet_m0_.clear();
  v_ggqq_jetIsQuark_.clear();
  for ( unsigned iJ(0); iJ != vJetIdxs.size(); ++iJ ) {

    reco::PFJetRef iJet( jets, vJetIdxs[iJ] );

    // Fill histograms 
    h_ggqq_jet_pT->Fill( std::abs(iJet->pt()) );
    h_ggqq_jet_eta->Fill( iJet->eta() );
    h_ggqq_jet_E->Fill( iJet->energy() );
    h_ggqq_jet_m0->Fill( iJet->mass() );

    // Fill branches 
    v_ggqq_jet_pt_.push_back( iJet->pt() );
    v_ggqq_jet_m0_.push_back( iJet->mass() );
    if ( v_ggqq_jetPdgIds_[iJ] < 4 ) {
      v_ggqq_jetIsQuark_.push_back( true );
      nQQ++;
    } else {
      v_ggqq_jetIsQuark_.push_back( false );
      nGG++;
    }

    // Get jet constituents
    v_ggqq_subJetE_[iJ].clear();
    v_ggqq_subJetPx_[iJ].clear();
    v_ggqq_subJetPy_[iJ].clear();
    v_ggqq_subJetPz_[iJ].clear();
    //std::vector<reco::PFCandidatePtr> jetConstituents = iJet->getPFConstituents();
    unsigned int nConstituents = iJet->getPFConstituents().size();
    for ( unsigned int j = 0; j < nConstituents; j++ ) {
      const reco::PFCandidatePtr subJet = iJet->getPFConstituent( j );
      std::cout << " >> " << j << ": E:" << subJet->energy() << " px:" << subJet->px() << " py:" << subJet->py() << " pz:" << subJet->pz() << std::endl;
      v_ggqq_subJetE_[iJ].push_back( subJet->energy() );
      v_ggqq_subJetPx_[iJ].push_back( subJet->px() );
      v_ggqq_subJetPy_[iJ].push_back( subJet->py() );
      v_ggqq_subJetPz_[iJ].push_back( subJet->pz() );
    }
  }
  h_nGG->Fill( nGG );
  h_nQQ->Fill( nQQ );


} // fillEvtSel_jet_dijet_gg_qq()*/
