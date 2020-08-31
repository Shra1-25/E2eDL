import FWCore.ParameterSet.Config as cms 

EGInference = cms.EDProducer('EGProducer'
    , reducedEBRecHitCollection = cms.InputTag('reducedEcalRecHitsEB')
    , photonCollection = cms.InputTag('gedPhotons')
    , EBEnergy = cms.InputTag('ProducerFrames','EBenergy')
    , modelName = cms.string("e_vs_ph_model.pb")
    #, mode = cms.string("JetLevel")
    # Jet level cfg
    #, nJets = cms.int32(-1)
    #, minJetPt = cms.double(35.)
    #, maxJetEta = cms.double(2.4)
    #, z0PVCut  = cms.double(0.1)
    )
