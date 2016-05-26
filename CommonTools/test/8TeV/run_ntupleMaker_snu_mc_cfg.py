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
        #"file:catTupleNEW.root"
        "file:catTuple_3evts.root"
)
)

process.options.wantSummary = True

process.nEventsTotal = cms.EDProducer("EventCountProducer")

process.ntuple = cms.EDAnalyzer("GenericNtupleMakerSNU8TeV",
    failureMode = cms.untracked.string("keep"), # choose one among keep/skip/error
    eventCounters = cms.vstring("nEventsTotal"), #"nEventsTotal", "nEventsClean", "nEventsPAT"),
    jets = cms.InputTag("catJets"),
                           
    int = cms.PSet(
        nPV               =  cms.InputTag("recoEventInfo"   , "pnV"    ),
        nTrueInteraction  =  cms.InputTag("pileupWeight", "nTrueInteraction" ),
        nGoodPV           =  cms.InputTag("recoVertexs" ,"goodOfflinePrimaryVertices"  )
    ),
    float = cms.PSet(
        puWeight   = cms.InputTag("pileupWeight"),
        puWeightUp = cms.InputTag("pileupWeight", "up"),
        puWeightDn = cms.InputTag("pileupWeight", "dn"),
    ),

    floats = cms.PSet(
    ),

    cands_bool = cms.PSet(
        muon = cms.PSet(                                
            src = cms.InputTag("catMuons"),
             exprs = cms.untracked.PSet(
                isTracker = cms.string("isTrackerMuon"),
                isGlobal = cms.string("isGlobalMuon"),
                isLoose = cms.string("isLooseMuon"),
                isTight = cms.string("isTightMuon"),
                matched = cms.string("mcMatched"),
                isPF = cms.string("isPFMuon"),
                ),
            selections = cms.untracked.PSet(),
            ),
        
        electrons = cms.PSet(
            src = cms.InputTag("catElectrons"),
            exprs = cms.untracked.PSet(
                electronID_loose   = cms.string("electronID('eidLoose')"),
                electronID_tight   = cms.string("electronID('eidTight')"),
                mcMatched = cms.string("mcMatched"),
                isPF = cms.string("isPF"),
                passConversionVeto = cms.string("passConversionVeto"),
                ),
            selections = cms.untracked.PSet(),
            ),
        jets = cms.PSet(
            src = cms.InputTag("catJets"),
            exprs = cms.untracked.PSet(
                isLoose = cms.string("looseJetID"),
                ),
            selections = cms.untracked.PSet(),
            ),
        
        ),#end of cand jets

                                
   cands_int = cms.PSet(
        muon = cms.PSet(
            src = cms.InputTag("catMuons"),
             exprs = cms.untracked.PSet(
                validhits = cms.string("numberOfValidHits"),
                validmuonhits = cms.string("numberOfValidMuonHits"),
                matchedstations = cms.string("numberOfMatchedStations"),
                validpixhits = cms.string("numberOfValidPixelHits"),
                trackerlayers= cms.string("trackerLayersWithMeasurement"),
                q = cms.string("charge"),
                ),
            selections = cms.untracked.PSet(),
            ),
        electrons = cms.PSet(
            src = cms.InputTag("catElectrons"),
            exprs = cms.untracked.PSet(
                q = cms.string("charge"),
                ),
            selections = cms.untracked.PSet(),
            ),


        jets = cms.PSet(
            src = cms.InputTag("catJets"),
            exprs = cms.untracked.PSet(
                partonFlavour = cms.string("partonFlavour"),
                hadronFlavour = cms.string("hadronFlavour"),
                partonPdgId = cms.string("partonPdgId"),
                vtxNtracks = cms.string("vtxNtracks"),
                ),
            selections = cms.untracked.PSet(
                ),
            ),

        ),# end of cand_int
                                

                    
    cands = cms.PSet(
        muon = cms.PSet(
            src = cms.InputTag("catMuons"),
            exprs = cms.untracked.PSet(
                x  = cms.string("vx"),
                y  = cms.string("vy"),
                z  = cms.string("vz"),
                pt  = cms.string("pt"),
                eta = cms.string("eta"),
                phi = cms.string("phi"),
                m   = cms.string("mass"),
                energy   = cms.string("energy"),
                relIso03 = cms.string("relIso(0.3)"),
                relIso04 = cms.string("relIso(0.4)"),
                dxy = cms.string("dxy"),
                normchi = cms.string("normalizedChi2"),
                dz = cms.string("dz"),
            ),
            selections = cms.untracked.PSet(),
        ),

        electrons = cms.PSet(
            src = cms.InputTag("catElectrons"),
            #index = cms.untracked.int32(0),
            exprs = cms.untracked.PSet(
                x  = cms.string("vx"),
                y  = cms.string("vy"),
                z  = cms.string("vz"),
                pt  = cms.string("pt"),
                eta = cms.string("eta"),
                phi = cms.string("phi"),
                m   = cms.string("mass"),
                electronID_mva_trig_tight        = cms.string("electronID('mvaTrigV0')"),
                energy   = cms.string("energy"),
                relIso03 = cms.string("relIso(0.3)"),
                relIso04 = cms.string("relIso(0.4)"),
                absIso03 = cms.string("absIso(0.3)"),
                absIso04 = cms.string("absIso(0.4)"),
                chIso03 = cms.string("chargedHadronIso(0.3)"),
                nhIso03 = cms.string("neutralHadronIso(0.3)"),
                phIso03 = cms.string("photonIso(0.3)"),
                puChIso03 = cms.string("puChargedHadronIso(0.3)"),
                chIso04 = cms.string("chargedHadronIso(0.4)"),
                nhIso04 = cms.string("neutralHadronIso(0.4)"),
                phIso04 = cms.string("photonIso(0.4)"),
                puChIso04 = cms.string("puChargedHadronIso(0.4)"), 
                scEta = cms.string("scEta"),
                dxy = cms.string("dxy"),
                dz = cms.string("dz"),
                isGsfCtfScPixChargeConsistent = cms.string("isGsfCtfScPixChargeConsistent"),
            ),
            selections = cms.untracked.PSet(
            ),
        ),


        jets = cms.PSet(
            src = cms.InputTag("catJets"),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                eta = cms.string("eta"),
                phi = cms.string("phi"),
                m   = cms.string("mass"),
                energy   = cms.string("energy"),
                csv= cms.string("bDiscriminator('combinedSecondaryVertexBJetTags')"),
                PileupJetId = cms.string("pileupJetId"),
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
    process.nEventsTotal*
    process.ntuple
)

