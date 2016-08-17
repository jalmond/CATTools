import FWCore.ParameterSet.Config as cms
import os

process = cms.Process("Ana")

process.load("FWCore.MessageLogger.MessageLogger_cfi")
#process.load("Configuration.StandardSequences.Services_cff")
#process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.MessageLogger.cerr.FwkReport.reportEvery = 10000

process.source = cms.Source("PoolSource",
fileNames = cms.untracked.vstring(
#        'file:/cms/scratch/jalmond/privateCatuples/v7-6-3/EE/40/catTuple_1.root'
        "root://cms-xrdr.sdfarm.kr:1094///xrd/store/group/CAT/ZZ_TuneCUETP8M1_13TeV-pythia8/v7-6-4_RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/160325_075601/0000/catTuple_12.root"

      )
)


process.nEventsTotal = cms.EDProducer("EventCountProducer")

process.load("CATTools.CatAnalyzer.flatGenWeights_cfi")



process.ntuple = cms.EDAnalyzer("GenericNtupleMakerSNU",
    failureMode = cms.untracked.string("keep"), # choose one among keep/skip/error
    eventCounters = cms.vstring("nEventsTotal"), #"nEventsTotal", "nEventsClean", "nEventsPAT"),
    genjet = cms.InputTag("slimmedGenJets"),
    genLabel      = cms.InputTag("prunedGenParticles"),
    triggerBits = cms.InputTag("TriggerResults","","HLT"),
    triggerObjects = cms.InputTag("catTrigger"),
    triggerPrescales = cms.InputTag("patTrigger"),
    muons = cms.InputTag("catMuons"),
    electrons = cms.InputTag("catElectrons"),                                
    jets = cms.InputTag("catJets"),                                
    vertices = cms.InputTag("catVertex"),
    met = cms.InputTag("catMETs"),
    genWeightLabel = cms.InputTag("genWeight"),
    pdfweights = cms.InputTag("flatGenWeights","pdf"),
    scaleupweights = cms.InputTag("flatGenWeights","scaleup"),
    scaledownweights = cms.InputTag("flatGenWeights","scaledown"),

    runFullTrig= cms.bool(True),
    keepAllGen= cms.bool(True),
    makeSlim= cms.bool(False),
    allweights= cms.bool(True),                                

    metFilterBitsPAT = cms.InputTag("TriggerResults","","PAT"),                                                                                                     metFilterBitsRECO = cms.InputTag("TriggerResults","","RECO"),               metFilterNames = cms.vstring(                                               
 "CSCTightHaloFilter",
 "eeBadScFilter",
                "goodVertices",
), 
    int = cms.PSet(
        nGoodPV           =  cms.InputTag("catVertex"   , "nGoodPV"),
        nPV               =  cms.InputTag("catVertex"   , "nPV"    ),
        nTrueInteraction  =  cms.InputTag("pileupWeight", "nTrueInteraction" ),
        
        
    ),
    float = cms.PSet(
        puWeightGold   = cms.InputTag("pileupWeight"),
        puWeightGoldUp = cms.InputTag("pileupWeight", "up"),
        puWeightGoldDn = cms.InputTag("pileupWeight", "dn"),



    ),

    floats = cms.PSet(
    ),

cands_int= cms.PSet(
),

cands_bool= cms.PSet(

      photons = cms.PSet(
            src = cms.InputTag("catPhotons"),
            exprs = cms.untracked.PSet(
#                photonID_loose   = cms.string("photonID('cutBasedPhotonID-Spring15-25ns-V1-standalone-loose')"),
#                photonID_medium   = cms.string("photonID('cutBasedPhotonID-Spring15-25ns-V1-standalone-medium')"),
#                photonID_tight   = cms.string("photonID('cutBasedPhotonID-Spring15-25ns-V1-standalone-tight')"),
                #photonID_mva        = cms.string("photonID('mvaPhoID-Spring15-25ns-nonTrig-V2-wp90')"),
                mcMatched = cms.string("mcMatched"),
                haspixseed = cms.string("HasPixelSeed"),
                passelectronveto = cms.string("PassElectronVeto"),
                ),
            selections = cms.untracked.PSet(),
            ),
        ),#end of cand jets

                                
    cands = cms.PSet(
        photons = cms.PSet(
            src = cms.InputTag("catPhotons"),
            exprs = cms.untracked.PSet(
                 pt  = cms.string("pt"),
                 eta = cms.string("eta"),
                 phi = cms.string("phi"),
                 energy   = cms.string("energy"),
                 chargedHadronIso = cms.string("chargedHadronIso"),
                 puChargedHadronIso  = cms.string("puChargedHadronIso"),
                 neutralHadronIso  = cms.string("neutralHadronIso"),
                 photonIso = cms.string("photonIso"),
                 rhoIso = cms.string("rhoIso"),
                 #chargedHadronIsoWithEA = cms.string("chargedHadronIsoWithEA"),
                 #neutralHadronIsoWithEA  = cms.string("neutralHadronIsoWithEA"),
                 #photonIsoWithEA = cms.string("photonIsoWithEA"),
                 sigmaietaieta = cms.string("SigmaiEtaiEta"),
                 r9 = cms.string("r9"),
                 hovere = cms.string("HoverE"),
                 sceta = cms.string("SCEta"),
                 scphi = cms.string("SCPhi"),
                 scrawenergy = cms.string("SCRawEnergy"),
                 scpreshowerenergy = cms.string("SCPreShowerEnergy"),
                 ),
            selections = cms.untracked.PSet(
            ),
        ),


        met = cms.PSet(
            src = cms.InputTag("catMETs"),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                phi = cms.string("phi"),
                sumet = cms.string("sumEt"),
                unclusteredEn_Px_up = cms.string("unclusteredEnPx(1)"),
                unclusteredEn_Py_up = cms.string("unclusteredEnPy(1)"),
                unclusteredEn_SumEt_up = cms.string("unclusteredEnSumEt(1)"),
                unclusteredEn_Phi_up = cms.string("unclusteredEnPhi(1)"),
                unclusteredEn_Px_down = cms.string("unclusteredEnPx(-1)"),
                unclusteredEn_Py_down = cms.string("unclusteredEnPy(-1)"),
                unclusteredEn_SumEt_down = cms.string("unclusteredEnSumEt(-1)"),
                unclusteredEn_Phi_down = cms.string("unclusteredEnPhi(-1)"),

                jetEn_Px_up  = cms.string("JetEnPx(1)"),
                jetEn_Py_up  = cms.string("JetEnPy(1)"),
                jetEn_SumEt_up  = cms.string("JetEnSumEt(1)"),
                jetEn_Px_down  = cms.string("JetEnPx(-1)"),
                jetEn_Py_down  = cms.string("JetEnPy(-1)"),
                jetEn_SumEt_down  = cms.string("JetEnSumEt(-1)"),
                jetRes_Px_up  = cms.string("JetResPx(1)"),
                jetRes_Py_up  = cms.string("JetResPy(1)"),
                jetRes_SumEt_up  = cms.string("JetResSumEt(1)"),
                jetRes_Px_down  = cms.string("JetResPx(-1)"),
                jetRes_Py_down  = cms.string("JetResPy(-1)"),
                jetRes_SumEt_down  = cms.string("JetResSumEt(-1)"),
            ),
            selections = cms.untracked.PSet(),
        ),
        metNoHF = cms.PSet(
            src = cms.InputTag("catMETsNoHF"),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                phi = cms.string("phi"),
                sumet = cms.string("sumEt"),
                unclusteredEn_Px_up = cms.string("unclusteredEnPx(1)"),
                unclusteredEn_Py_up = cms.string("unclusteredEnPy(1)"),
                unclusteredEn_SumEt_up = cms.string("unclusteredEnSumEt(1)"),
                unclusteredEn_Phi_up = cms.string("unclusteredEnPhi(1)"),
                unclusteredEn_Px_down = cms.string("unclusteredEnPx(-1)"),
                unclusteredEn_Py_down = cms.string("unclusteredEnPy(-1)"),
                unclusteredEn_SumEt_down = cms.string("unclusteredEnSumEt(-1)"),
                unclusteredEn_Phi_down = cms.string("unclusteredEnPhi(-1)"),
                jetEn_Px_up  = cms.string("JetEnPx(1)"),
                jetEn_Py_up  = cms.string("JetEnPy(1)"),
                jetEn_SumEt_up  = cms.string("JetEnSumEt(1)"),
                jetEn_Px_down  = cms.string("JetEnPx(-1)"),
                jetEn_Py_down  = cms.string("JetEnPy(-1)"),
                jetEn_SumEt_down  = cms.string("JetEnSumEt(-1)"),
                jetRes_Px_up  = cms.string("JetResPx(1)"),
                jetRes_Py_up  = cms.string("JetResPy(1)"),
                jetRes_SumEt_up  = cms.string("JetResSumEt(1)"),
                jetRes_Px_down  = cms.string("JetResPx(-1)"),
                jetRes_Py_down  = cms.string("JetResPy(-1)"),
                jetRes_SumEt_down  = cms.string("JetResSumEt(-1)"),

            ),
            selections = cms.untracked.PSet(),
            ),
        
        )#end of cands
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string("ntuple.root"),
)

#process.load("CATTools.CatProducer.pseudoTop_cff")
process.p = cms.Path(
    process.flatGenWeights*
    process.nEventsTotal*
    process.ntuple
)

