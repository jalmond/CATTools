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
        "root://cms-xrdr.sdfarm.kr:1094///xrd/store/group/CAT/WZ_TuneCUETP8M1_13TeV-pythia8/v7-4-2_RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/150923_192845/0000/catTuple_7.root"
      )
)


process.nEventsTotal = cms.EDProducer("EventCountProducer")




process.ntuple = cms.EDAnalyzer("GenericNtupleMakerSNU",
    failureMode = cms.untracked.string("keep"), # choose one among keep/skip/error
    eventCounters = cms.vstring("nEventsTotal"), #"nEventsTotal", "nEventsClean", "nEventsPAT"),
    genLabel      = cms.InputTag("prunedGenParticles"),
    triggerBits = cms.InputTag("TriggerResults","","HLT"),
    triggerObjects = cms.InputTag("catTrigger"),
    triggerPrescales = cms.InputTag("patTrigger"),
    muons = cms.InputTag("catMuons"),
    electrons = cms.InputTag("catElectrons"),                                
    vertices = cms.InputTag("catVertex"),
    met = cms.InputTag("catMETs"),
    runFullTrig= cms.bool(True),
    metFilterBitsPAT = cms.InputTag("TriggerResults","","PAT"),                                                                                                     metFilterBitsRECO = cms.InputTag("TriggerResults","","RECO"),                                                                                                   metFilterNames = cms.vstring(                                               
    "HBHENoiseFilter",
    "CSCTightHaloFilter",
    "goodVertices",
    "eeBadScFilter",
    "EcalDeadCellTriggerPrimitiveFilter",
), 
    int = cms.PSet(
        nGoodPV           =  cms.InputTag("catVertex"   , "nGoodPV"),
        nPV               =  cms.InputTag("catVertex"   , "nPV"    ),
        nTrueInteraction  =  cms.InputTag("pileupWeight", "nTrueInteraction" ),
        lumiMaskGold      =  cms.InputTag("lumiMask"),
        lumiMaskSilver      =  cms.InputTag("lumiMaskSilver"),
        
        GenTTCat =  cms.InputTag("GenTtbarCatergories" , "genTtbarId"),

        genWeight_id1   = cms.InputTag("genWeight" , "id1"),
        genWeight_id2   = cms.InputTag("genWeight" , "id2"),


    ),
    float = cms.PSet(
        genWeightQ   = cms.InputTag("genWeight" , "Q"),
        genWeight   = cms.InputTag("genWeight", "genWeight"),
        lheWeight   = cms.InputTag("genWeight", "lheWeight"),
        genWeightX1   = cms.InputTag("genWeight" , "x2"),
        genWeightX2   = cms.InputTag("genWeight" , "x1"),

        puWeightSilver   = cms.InputTag("pileupWeightSilver"),
        puWeightSilverUp = cms.InputTag("pileupWeightSilver", "up"),
        puWeightSilverDn = cms.InputTag("pileupWeightSilver", "dn"),
        puWeightGold   = cms.InputTag("pileupWeight"),
        puWeightGoldUp = cms.InputTag("pileupWeight", "up"),
        puWeightGoldDn = cms.InputTag("pileupWeight", "dn"),

    ),

    floats = cms.PSet(
        pdfWeight = cms.InputTag("genWeight", "pdfWeights"),
    ),

    cands_bool = cms.PSet(
        muon = cms.PSet(                                
            src = cms.InputTag("catMuons"),
             exprs = cms.untracked.PSet(
                isTracker = cms.string("isTrackerMuon"),
                isGlobal = cms.string("isGlobalMuon"),
                isLoose = cms.string("isLooseMuon"),
                isMedium = cms.string("isMediumMuon"),
                isTight = cms.string("isTightMuon"),
                isSoft = cms.string("isSoftMuon"),
                matched = cms.string("mcMatched"),
                isPF = cms.string("isPFMuon"),
                ),
            selections = cms.untracked.PSet(),
            ),
        
        electrons = cms.PSet(
            src = cms.InputTag("catElectrons"),
            exprs = cms.untracked.PSet(
                electronID_loose   = cms.string("electronID('cutBasedElectronID-Spring15-25ns-V1-standalone-loose')"),
                electronID_medium   = cms.string("electronID('cutBasedElectronID-Spring15-25ns-V1-standalone-medium')"),
                electronID_tight   = cms.string("electronID('cutBasedElectronID-Spring15-25ns-V1-standalone-tight')"),
                electronID_veto   = cms.string("electronID('cutBasedElectronID-Spring15-25ns-V1-standalone-veto')"),
                electronID_mva_medium        = cms.string("electronID('mvaEleID-Spring15-25ns-nonTrig-V1-wp90')"),
                electronID_mva_tight        = cms.string("electronID('mvaEleID-Spring15-25ns-nonTrig-V1-wp80')"),
                electronID_mva_trig_medium        = cms.string("electronID('mvaEleID-Spring15-25ns-Trig-V1-wp90')"),
                electronID_mva_trig_tight        = cms.string("electronID('mvaEleID-Spring15-25ns-Trig-V1-wp80')"),
                electronID_heep   = cms.string("electronID('heepElectronID-HEEPV60')"),
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
                isTight = cms.string("tightJetID"),
                isTightLepVetoJetID   = cms.string("tightLepVetoJetID"),
            ),
            selections = cms.untracked.PSet(
                ),
            ),
        ),
                                
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
                electronID_snu   = cms.string("snuID"),                
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
        slimmedGenJets = cms.PSet(
            src = cms.InputTag("slimmedGenJets",""),
            exprs = cms.untracked.PSet(
                #partonpdgId = cms.string("partonPdgId"),
                #partonFlavour  = cms.string("partonFlavour"),
                ),
            selections = cms.untracked.PSet(),
            ),
        ),
                                

                    
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
                shiftedEup = cms.string("shiftedEnUp"),
                shiftedEdown = cms.string("shiftedEnDown"),
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
                energy   = cms.string("energy"),
                shiftedEnDown = cms.string("shiftedEnDown"),
                shiftedEnUp = cms.string("shiftedEnUp"),
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
                vtxMass = cms.string("vtxMass"),
                vtx3DVal = cms.string("vtx3DVal"),
                vtx3DSig = cms.string("vtx3DSig"),
                CVSInclV2 = cms.string("bDiscriminator('pfCombinedInclusiveSecondaryVertexV2BJetTags')"),
                chargedEmEnergyFraction = cms.string("chargedEmEnergyFraction"),
                shiftedEnDown = cms.string("shiftedEnDown"),
                shiftedEnUp = cms.string("shiftedEnUp"),
                smearedRes = cms.string("smearedRes"),
                smearedResDown = cms.string("smearedResDown"),
                smearedResUp = cms.string("smearedResUp"),
                PileupJetId = cms.string("pileupJetId"),
                ),
        ),


            jetsPuppi = cms.PSet(
            src = cms.InputTag("catJetsPuppi"),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                eta = cms.string("eta"),
                phi = cms.string("phi"),
                m   = cms.string("mass"),
                vtxMass = cms.string("vtxMass"),
                vtx3DVal = cms.string("vtx3DVal"),
                vtx3DSig = cms.string("vtx3DSig"),
                CVSInclV2 = cms.string("bDiscriminator('pfCombinedInclusiveSecondaryVertexV2BJetTags')"),
                partonFlavour = cms.string("partonFlavour"),
                hadronFlavour = cms.string("hadronFlavour"),
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
        metNoHF = cms.PSet(
            src = cms.InputTag("catMETsNoHF"),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                phi = cms.string("phi"),
                sumet = cms.string("sumEt"),
            ),
            selections = cms.untracked.PSet(),
        ),
        metPfMva = cms.PSet(
            src = cms.InputTag("catMETsPfMva"),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                phi = cms.string("phi"),
                sumet = cms.string("sumEt"),
            ),
            selections = cms.untracked.PSet(),
        ),
        metPuppi = cms.PSet(
            src = cms.InputTag("catMETsPuppi"),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                phi = cms.string("phi"),
                sumet = cms.string("sumEt"),
            ),
            selections = cms.untracked.PSet(),
        ),

        slimmedGenJets = cms.PSet(
            src = cms.InputTag("slimmedGenJets",""),
            #index = cms.untracked.int32(0),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                eta = cms.string("eta"),
                phi = cms.string("phi"),
                m   = cms.string("mass"),
                energy   = cms.string("energy"),
                ),
            selections = cms.untracked.PSet(),
            ),
        
        ),
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string("ntuple.root"),
)

#process.load("CATTools.CatProducer.pseudoTop_cff")
process.p = cms.Path(
    process.nEventsTotal*
    process.ntuple
)

