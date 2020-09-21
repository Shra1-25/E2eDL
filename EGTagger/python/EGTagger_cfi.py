import FWCore.ParameterSet.Config as cms

EGInference = cms.EDProducer('EGTagger'
    , photonCollection = cms.InputTag('gedPhotons')
    , EGframes = cms.InputTag('EGFrameProducer','EGframes')
    )
