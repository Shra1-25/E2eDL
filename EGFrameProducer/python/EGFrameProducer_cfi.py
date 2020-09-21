import FWCore.ParameterSet.Config as cms

EGInference = cms.EDProducer('EGFrameProducer'
    , photonCollection = cms.InputTag('gedPhotons')
    , reducedEBRecHitCollection = cms.InputTag('reducedEcalRecHitsEB')
    , EGModelName = cms.string("e_vs_ph_model.pb")
    , doInference = cms.bool(True)
    , doEBenergy = cms.bool(True)
    , EBEnergy = cms.InputTag('ProducerFrames','EBenergy')
    )
