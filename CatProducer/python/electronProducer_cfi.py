import FWCore.ParameterSet.Config as cms

catElectrons = cms.EDProducer("CATElectronProducer",
    src = cms.InputTag("slimmedElectrons"),
    mcLabel = cms.InputTag("prunedGenParticles"),
    vertexLabel = cms.InputTag('catVertex'),
    beamLineSrc = cms.InputTag("offlineBeamSpot"),
    rhoLabel = cms.InputTag("fixedGridRhoAll"),
    electronIDSources = cms.PSet(),
    electronIDs = cms.vstring(
        "cutBasedElectronID_Spring15_25ns_V1_standalone_loose",
        "cutBasedElectronID_Spring15_25ns_V1_standalone_medium",
        "cutBasedElectronID_Spring15_25ns_V1_standalone_tight",
        "cutBasedElectronID_Spring15_25ns_V1_standalone_veto",
        "heepElectronID_HEEPV60",
        "mvaEleID_Spring15_25ns_nonTrig_V1_wp80",
        "mvaEleID_Spring15_25ns_nonTrig_V1_wp90",
        "mvaEleID_Spring15_25ns_Trig_V1_wp80",
        "mvaEleID_Spring15_25ns_Trig_V1_wp90"
        )
)
