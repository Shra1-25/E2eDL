import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing('analysis')
options.register('skipEvents', 
    default=0, 
    mult=VarParsing.VarParsing.multiplicity.singleton,
    mytype=VarParsing.VarParsing.varType.int,
    info = "skipEvents")
# TODO: put this option in cmsRun scripts
#options.register('processMode', 
#    default='JetLevel', 
#    mult=VarParsing.VarParsing.multiplicity.singleton,
#    mytype=VarParsing.VarParsing.varType.string,
#    info = "process mode: JetLevel or EventLevel")
options.parseArguments()

process = cms.Process("Inference")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) ) #options.maxEvents

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
      options.inputFiles
      )#"file:/afs/cern.ch/user/s/schaudha/public/CMSSW_10_6_8/src/demo/ZprimeToTT_M-2000_W-20_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_AODSIM_PUMoriond17.root"
    , skipEvents = cms.untracked.uint32(0)#options.skipEvents
    )
print (" >> Loaded",len(options.inputFiles),"input files from list.")

process.load("ProdTutorial.ProducerTest.FrameInference_cfi")
process.p = cms.Path(process.FrameInference)
