import FWCore.ParameterSet.Config as cms

EGFrames = cms.EDProducer('EGFrameProducer'
    , photonCollection = cms.InputTag('gedPhotons')
    , reducedEBRecHitCollection = cms.InputTag('reducedEcalRecHitsEB')
    , doEBenergy = cms.bool(True)
    , EBEnergy = cms.InputTag('ProducerFrames','EBenergy')
    )
