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
        'file:/cms/scratch/CAT/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/v7-3-2_RunIISpring15DR74-Asympt50ns_MCRUN2_74_V9A-v2/150805_203735/0000/catTuple_1.root','file:/cms/scratch/CAT/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/v7-3-2_RunIISpring15DR74-Asympt50ns_MCRUN2_74_V9A-v2/150805_203735/0000/catTuple_2.root'
      )
)

process.nEventsTotal = cms.EDProducer("EventCountProducer")
process.ntuple = cms.EDAnalyzer("GenericNtupleMaker",
    failureMode = cms.untracked.string("keep"), # choose one among keep/skip/error
    eventCounters = cms.vstring("nEventsTotal"), #"nEventsTotal", "nEventsClean", "nEventsPAT"),
    int = cms.PSet(
        nVertex   = cms.PSet(src = cms.InputTag("recoEventInfo","pvN")),
        HLTDoubleMu = cms.PSet(src = cms.InputTag("recoEventInfo","HLTDoubleMu")),
        HLTDoubleEl = cms.PSet(src = cms.InputTag("recoEventInfo","HLTDoubleEl")),
        HLTMuEl = cms.PSet(src = cms.InputTag("recoEventInfo","HLTMuEl")),
        HLTSingleMu = cms.PSet(src = cms.InputTag("recoEventInfo","HLTSingleMu")),
        HLTSingleEl = cms.PSet(src = cms.InputTag("recoEventInfo","HLTSingleEl")),
    ),
    double = cms.PSet(
        puWeight   = cms.PSet(src = cms.InputTag("pileupWeight")),
        puWeightUp = cms.PSet(src = cms.InputTag("pileupWeight", "up")),
        puWeightDn = cms.PSet(src = cms.InputTag("pileupWeight", "dn")),
        pvX   = cms.PSet(src = cms.InputTag("recoEventInfo","pvX")),
        pvY   = cms.PSet(src = cms.InputTag("recoEventInfo","pvY")),
        pvZ   = cms.PSet(src = cms.InputTag("recoEventInfo","pvZ")),
   ),
    doubles = cms.PSet(
        pdfWeight = cms.PSet(src = cms.InputTag("pdfWeight")),
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
                electronID_loose   = cms.string("electronID('cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-loose')"),
                electronID_medium   = cms.string("electronID('cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-medium')"),
                electronID_tight   = cms.string("electronID('cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-tight')"),
                electronID_veto   = cms.string("electronID('cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-veto')"),
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
                #rhoIso03 = cms.string("rho"),
                scEta = cms.string("scEta"),
#                dxy = cms.string("dxy"),
 #               dz = cms.string("dz"),
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
                #CSVInclV2 = cms.string("bDiscriminator('combinedInclusiveSecondaryVertexV2BJetTags')"),

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
        met = cms.PSet(
            src = cms.InputTag("catMETs"),
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

        pseudoTopJet = cms.PSet(
            src = cms.InputTag("pseudoTop","jets"),
            #index = cms.untracked.int32(0),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                eta = cms.string("eta"),
                phi = cms.string("phi"),
                m   = cms.string("mass"),
                pdgId = cms.string("pdgId"),
                q = cms.string("charge"),
                #status = cms.string("status"),
            ),
            selections = cms.untracked.PSet(),
        ),
        pseudoTopLepton = cms.PSet(
            src = cms.InputTag("pseudoTop","leptons"),
            #index = cms.untracked.int32(0),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                eta = cms.string("eta"),
                phi = cms.string("phi"),
                m   = cms.string("mass"),
                pdgId = cms.string("pdgId"),
                q = cms.string("charge"),
                #status = cms.string("status"),
            ),
            selections = cms.untracked.PSet(),
        ),
        pseudoTopNu = cms.PSet(
            src = cms.InputTag("pseudoTop","neutrinos"),
            #index = cms.untracked.int32(0),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                eta = cms.string("eta"),
                phi = cms.string("phi"),
                #m   = cms.string("mass"),
                pdgId = cms.string("pdgId"),
                #q = cms.string("charge"),
                #status = cms.string("status"),
            ),
            selections = cms.untracked.PSet(),
        ),
        pseudoTop = cms.PSet(
            src = cms.InputTag("pseudoTop"),
            #index = cms.untracked.int32(0),
            exprs = cms.untracked.PSet(
                pt  = cms.string("pt"),
                eta = cms.string("eta"),
                phi = cms.string("phi"),
                m   = cms.string("mass"),
                pdgId = cms.string("pdgId"),
                q = cms.string("charge"),
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
    process.partonTop*
    process.ntuple
)

