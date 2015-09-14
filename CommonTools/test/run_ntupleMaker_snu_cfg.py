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
        "root://cms-xrdr.sdfarm.kr:1094///xrd/store/group/CAT/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/v7-4-1_RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v3/150910_121351/0000/catTuple_366.root"
      )
)

process.nEventsTotal = cms.EDProducer("EventCountProducer")
process.ntuple = cms.EDAnalyzer("GenericNtupleMaker",
    failureMode = cms.untracked.string("keep"), # choose one among keep/skip/error
    eventCounters = cms.vstring("nEventsTotal"), #"nEventsTotal", "nEventsClean", "nEventsPAT"),


    int = cms.PSet(
        nGoodPV           =  cms.InputTag("catVertex"   , "nGoodPV"),
        nPV               =  cms.InputTag("catVertex"   , "nPV"    ),

        pdfWeightId1 =   cms.PSet(src = cms.InputTag("pdfWeight", "id1" )),
        pdfWeightId2 =   cms.PSet(src = cms.InputTag("pdfWeight", "id2" )),
        
        nTrueInteraction  =   cms.PSet(src = cms.InputTag("pileupWeight", "nTrueInteraction" )),

        # HLT to be added below in the same file.
    ),
    float = cms.PSet(
        puWeight   = cms.InputTag("pileupWeight"),
        puWeightUp = cms.InputTag("pileupWeight", "up"),
        puWeightDn = cms.InputTag("pileupWeight", "dn"),

        pdfWeightQ  = cms.InputTag("pdfWeight", "Q" ),
        pdfWeightX1 = cms.InputTag("pdfWeight", "x1"),
        pdfWeightX2 = cms.InputTag("pdfWeight", "x2"),
    ),
    bool = cms.PSet(
        csctighthalofilter= cms.InputTag("catTrigger", "CSCTightHaloFilter"),
        ecalDCTRF   = cms.InputTag("catTrigger", "EcalDeatCellTriggerPrimitiveFilter"),
        eeBadScF     = cms.InputTag("catTrigger", "eeBadScFilter"),
        HNHENF = cms.InputTag("catTrigger", "HBHENoiseFilter"),
        

        hlt_el17_el12 =  cms.InputTag("catTrigger",  "HLTEle17Ele12CaloIdLTrackIdLIsoVLDZ"),
        hlt_el23_el12 =  cms.InputTag("catTrigger",  "HLTEle23Ele12CaloIdLTrackIdLIsoVL"),
        hlt_el23_el12dz =  cms.InputTag("catTrigger",  "HLTEle23Ele12CaloIdLTrackIdLIsoVLDZ"),
        hlt_ele27eta2p1 =  cms.InputTag("catTrigger",  "HLTEle27eta2p1WPLooseGsfTriCentralPFJet30"),
        hlt_el12 =  cms.InputTag("catTrigger", "HLTEle12CaloIdTrackIdIsoVL"),
        hlt_el17 =  cms.InputTag("catTrigger", "HLTEle17CaloIdTrackIdIsoVL"),
        hlt_el16_el12_8 =  cms.InputTag("catTrigger",  "HLT_Ele16_Ele12_Ele8_CaloIdLTrackIdL"),
        hlt_2el33 =  cms.InputTag("catTrigger", "HLTDoubleEle33CaloIdGsfTrkIdVL"),
        
        hlt_mu17_mu8 =  cms.InputTag("catTrigger",    "HLTMu17TrkIsoVVLMu8TrkIsoVVLDZ"),
        hlt_mu17_tkmu8 =  cms.InputTag("catTrigger",    "HLTMu17TrkIsoVVLTkMu8TrkIsoVVLDZ"),
        
        hlt_mu17_el12  =  cms.InputTag("catTrigger", "HLTMu17TrkIsoVVLEle12CaloIdLTrackIdLIsoVL"),
        hlt_mu8_el17 = cms.InputTag("catTrigger", "HLTMu8TrkIsoVVLEle17CaloIdLTrackIdLIsoVL"),
       
        ),        
        
                                

    floats = cms.PSet(
        pdfWeight = cms.InputTag("pdfWeight"),
    ),
    cands = cms.PSet(
        muon = cms.PSet(
            src = cms.InputTag("catMuons"),
            #index = cms.untracked.int32(0),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                eta = cms.string("eta"),
                phi = cms.string("phi"),
                m   = cms.string("mass"),
                energy   = cms.string("energy"),
                #relIso = cms.string("relIso"),
                ispf = cms.string("isPFMuon"),
                relIso03 = cms.string("relIso(0.3)"),
                relIso04 = cms.string("relIso(0.4)"),
                isTracker = cms.string("isTrackerMuon"),
                isGlobal = cms.string("isGlobalMuon"),
                isLoose = cms.string("isLooseMuon"),
                isTight = cms.string("isTightMuon"),
                dxy = cms.string("dxy"),
                normchi = cms.string("normalizedChi2"),
                validhits = cms.string("numberOfValidHits"),
                validmuonhits = cms.string("numberOfValidMuonHits"),
                matchedstations = cms.string("numberOfMatchedStations"),
                validpixhits = cms.string("numberOfValidPixelHits"),
                trackerlayers= cms.string("trackerLayersWithMeasurement"),
                dz = cms.string("dz"),
                q = cms.string("charge"),
                matched = cms.string("mcMatched"),
                shiftedEup = cms.string("shiftedEnUp"),
                shiftedEdown = cms.string("shiftedEnDown"),
            ),
            selections = cms.untracked.PSet(),
        ),
        electrons = cms.PSet(
            src = cms.InputTag("catElectrons"),
            #index = cms.untracked.int32(0),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                eta = cms.string("eta"),
                phi = cms.string("phi"),
                m   = cms.string("mass"),
                energy   = cms.string("energy"),
                electronID_loose   = cms.string("electronID('cutBasedElectronID-Spring15-25ns-V1-standalone-loose')"),
                electronID_medium   = cms.string("electronID('cutBasedElectronID-Spring15-25ns-V1-standalone-medium')"),
                electronID_tight   = cms.string("electronID('cutBasedElectronID-Spring15-25ns-V1-standalone-tight')"),
                electronID_veto   = cms.string("electronID('cutBasedElectronID-Spring15-25ns-V1-standalone-veto')"),
                electronID_snu   = cms.string("snuID"),
                shiftedEnDown = cms.string("shiftedEnDown"),
                shiftedEnUp = cms.string("shiftedEnUp"),
                mcMatched = cms.string("mcMatched"),
                isPF = cms.string("isPF"),
                relIso03 = cms.string("relIso(0.3)"),
                relIso04 = cms.string("relIso(0.4)"),
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
                passConversionVeto = cms.string("passConversionVeto"),
                q = cms.string("charge"),
#                isGsfCtfScPixChargeConsistent = cms.string("isGsfCtfScPixChargeConsistent"),
            ),
            selections = cms.untracked.PSet(
                isPassBaseId = cms.string("passConversionVeto && isPF && gsfTrack.hitPattern.numberOfLostHits('MISSING_INNER_HITS') <= 0"),
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
                CSVInclV2 = cms.string("bDiscriminator('pfCombinedInclusiveSecondaryVertexV2BJetTags')"),

                partonFlavour = cms.string("partonFlavour"),
                hadronFlavour = cms.string("hadronFlavour"),
                partonPdgId = cms.string("partonPdgId"),
                shiftedEnDown = cms.string("shiftedEnDown"),
                shiftedEnUp = cms.string("shiftedEnUp"),
                smearedRes = cms.string("smearedRes"),
                smearedResDown = cms.string("smearedResDown"),
                smearedResUp = cms.string("smearedResUp"),
                isLoose = cms.string("LooseId"),
                isTight = cms.string("TightId"),
                isPFId = cms.string("pileupJetId"),
                
            ),
            selections = cms.untracked.PSet(
                isLoose = cms.string("LooseId"),
                isTight = cms.string("TightId"),
                isPFId = cms.string("pileupJetId"),
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
                CSVInclV2 = cms.string("bDiscriminator('pfCombinedInclusiveSecondaryVertexV2BJetTags')"),
                partonFlavour = cms.string("partonFlavour"),
                hadronFlavour = cms.string("hadronFlavour"),
            ),
            selections = cms.untracked.PSet(
                isLoose = cms.string("LooseId"),
                isPFId = cms.string("pileupJetId"),
            ),
        ),

        met = cms.PSet(
            src = cms.InputTag("catMETs"),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                phi = cms.string("phi"),
            ),
            selections = cms.untracked.PSet(),
        ),
        metNoHF = cms.PSet(
            src = cms.InputTag("catMETsNoHF"),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                phi = cms.string("phi"),
            ),
            selections = cms.untracked.PSet(),
        ),
        metPfMva = cms.PSet(
            src = cms.InputTag("catMETsPfMva"),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                phi = cms.string("phi"),
            ),
            selections = cms.untracked.PSet(),
        ),
        metPuppi = cms.PSet(
            src = cms.InputTag("catMETsPuppi"),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                phi = cms.string("phi"),
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
                #pdgId = cms.string("pdgId"),
                #q = cms.string("charge"),
                #status = cms.string("status"),
            ),
            selections = cms.untracked.PSet(),
        ),

    ),
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string("ntuple.root"),
)

process.load("CATTools.CatProducer.pseudoTop_cff")
process.p = cms.Path(
    process.nEventsTotal*
    process.ntuple
)

