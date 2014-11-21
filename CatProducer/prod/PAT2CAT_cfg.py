## import skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *
process._Process__name = 'CAT'
## switch to uncheduled mode
process.options.allowUnscheduled = cms.untracked.bool(True)
#process.Tracer = cms.Service("Tracer")

process.load("PhysicsTools.PatAlgos.producersLayer1.patCandidates_cff")
process.load("CATTools.CatProducer.catCandidates_cff")

# Input source
process.source = cms.Source("PoolSource",
    secondaryFileNames = cms.untracked.vstring(),
    fileNames = cms.untracked.vstring(
      #'/store/relval/CMSSW_7_0_6_patch3/RelValZMM_13/GEN-SIM-RECO/PUpmx50ns_PLS170_V6AN1-v2/00000/1EF0EB3F-B412-E411-A7EC-0025905A612A.root'
      #'/store/relval/CMSSW_7_2_0_pre3/RelValZMM_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/40AB5AF9-621D-E411-9926-02163E00F4EC.root'
      'file:/cms/home/geonmo/CMSSW_7_2_1_patch4/src/CATTools/CatProducer/prod/data/080957A7-C36E-E411-A5BC-00266CF327C4.root',
    )
)

## for muon isolation study with weighted method
print "4508 PR needs to be merged"
print "git cms-merge-topic 4508"
print "or use the release above CMSSW_7_0_7"

### add different cone size
#let's use userIsolation function to use different cone size for the time being
#userIsolation("pat::User1Iso") for chargedHadronIso()
#userIsolation("pat::User2Iso") for neutralHadronIso()
#userIsolation("pat::User3Iso") for photonIso()
#userIsolation("pat::User4Iso") for puChargedHadronIso()
#userIsolation("pat::User5Iso") for particleIso()
#for muon
process.patMuons.isolationValues.user = cms.VInputTag("muPFIsoValueCharged03","muPFIsoValueNeutral03","muPFIsoValueGamma03","muPFIsoValuePU03","muPFIsoValueChargedAll03")
#for electron
process.patElectrons.isolationValues.user = cms.VInputTag("elPFIsoValueCharged03PFId","elPFIsoValueNeutral03PFId","elPFIsoValueGamma03PFId","elPFIsoValuePU03PFId","elPFIsoValueChargedAll03PFId")
process.patElectrons.isolationValuesNoPFId.user = cms.VInputTag("elPFIsoValueCharged03NoPFId","elPFIsoValueNeutral03NoPFId","elPFIsoValueGamma03NoPFId","elPFIsoValuePU03NoPFId","elPFIsoValueChargedAll03NoPFId")
###

process.load("CommonTools.ParticleFlow.deltaBetaWeights_cff")

from PhysicsTools.PatAlgos.tools.helpers import loadWithPostfix
loadWithPostfix(process,'RecoMuon.MuonIsolation.muonPFIsolation_cff',"Weighted")

process.patMuonsWeighted = process.patMuons.clone()
process.catMuonsWeighted = process.catMuons.clone()
process.catMuonsWeighted.src = 'patMuonsWeighted'

process.muPFIsoDepositNeutralWeighted.ExtractorPSet.inputCandView = 'pfWeightedNeutralHadrons'
process.muPFIsoDepositGammaWeighted.ExtractorPSet.inputCandView = 'pfWeightedPhotons'

process.patMuonsWeighted.isoDeposits = cms.PSet(
    pfChargedHadrons = cms.InputTag("muPFIsoDepositChargedWeighted" ),
    pfNeutralHadrons = cms.InputTag("muPFIsoDepositNeutralWeighted" ),
    pfPhotons = cms.InputTag("muPFIsoDepositGammaWeighted" ),
    pfPUChargedHadrons = cms.InputTag("muPFIsoDepositPUWeighted" ),
    pfChargedAll = cms.InputTag("muPFIsoDepositChargedAllWeighted" ),
    )

process.patMuonsWeighted.isolationValues = cms.PSet(
    pfChargedHadrons = cms.InputTag("muPFIsoValueCharged04Weighted"),
    pfNeutralHadrons = cms.InputTag("muPFIsoValueNeutral04Weighted" ),
    pfPhotons = cms.InputTag("muPFIsoValueGamma04Weighted" ),
    pfPUChargedHadrons = cms.InputTag("muPFIsoValuePU04Weighted" ),
    pfChargedAll = cms.InputTag("muPFIsoValueChargedAll04Weighted"),
    user = cms.VInputTag("muPFIsoValueCharged03Weighted","muPFIsoValueNeutral03Weighted","muPFIsoValueGamma03Weighted","muPFIsoValuePU03Weighted","muPFIsoValueChargedAll03Weighted"),
    )

##
## ------------------------------------------------------
#  In addition you usually want to change the following
#  parameters:
## ------------------------------------------------------
#
#   process.GlobalTag.globaltag =  ...    ##  (according to https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideFrontierConditions)
#                                         ##
#from PhysicsTools.PatAlgos.patInputFiles_cff import filesRelValProdTTbarAODSIM
#process.source.fileNames = filesRelValProdTTbarAODSIM
#                                         ##
process.maxEvents.input = -1
#                                         ##
#   process.out.outputCommands = [ ... ]  ##  (e.g. taken from PhysicsTools/PatAlgos/python/patEventContent_cff.py)
#                                         ##
process.out.fileName = 'CAT.root'
#                                         ##
#   process.options.wantSummary = False   ##  (to suppress the long output at the end of the job)

process.out.outputCommands = ['keep *_cat*_*_*',
                              'keep *_goodOfflinePrimaryVertices_*_*'
                             ]
