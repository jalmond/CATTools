import FWCore.ParameterSet.Config as cms
import os
process = cms.Process("h2muAnalyzer")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

datadir = '/xrootd/store/group/CAT/SingleMuon/v7-3-6_Run2015B-PromptReco-v1/150820_215216/0000/'
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/j/jlee/work/cat74/src/CATTools/CatProducer/prod/catTuple.root')
    #fileNames = cms.untracked.vstring()
)

#for f in os.listdir(datadir):
#    process.source.fileNames.append("file:"+datadir+f)

print process.source.fileNames
runOnMC=True
### for run data
lumiFile = 'Cert_246908-255031_13TeV_PromptReco_Collisions15_50ns_JSON.txt'
for i in process.source.fileNames:
    if 'Run2015' in i:
        runOnMC=False
if not runOnMC:
    from FWCore.PythonUtilities.LumiList import LumiList
    lumiList = LumiList(os.environ["CMSSW_BASE"]+'/src/CATTools/CatProducer/prod/LumiMask/'+lumiFile)
    process.source.lumisToProcess = lumiList.getVLuminosityBlockRange()
    print process.source.lumisToProcess
        
process.h2mu = cms.EDAnalyzer("h2muAnalyzer",
    vertices = cms.InputTag("catVertex"),
    muons = cms.InputTag("catMuons"),
    electrons = cms.InputTag("catElectrons"),
    jets = cms.InputTag("catJets"),
    mets = cms.InputTag("catMETs"),
    mcLabel = cms.InputTag("prunedGenParticles"),
    triggerBits = cms.InputTag("TriggerResults","","HLT"),
    triggerObjects = cms.InputTag("catTrigger"),
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string("h2mu.root")
)

process.p = cms.Path(process.h2mu)
process.MessageLogger.cerr.FwkReport.reportEvery = 50000
