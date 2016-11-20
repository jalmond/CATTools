import FWCore.ParameterSet.Config as cms

catPhotons = cms.EDProducer("CATPhotonProducer",
    src = cms.InputTag("slimmedPhotons"),
    photonIDs = cms.vstring("cutBasedPhotonID-Spring15-25ns-V1-standalone-loose",
                            "cutBasedPhotonID-Spring15-25ns-V1-standalone-medium",
                            "cutBasedPhotonID-Spring15-25ns-V1-standalone-tight",
                            "mvaPhoID-Spring15-25ns-nonTrig-V2p1-wp90",
                            ),
    photonIDs_alt = cms.vstring("cutBasedPhotonID_Spring15_25ns_V1_standalone_loose",
                             "cutBasedPhotonID_Spring15_25ns_V1_standalone_medium",
                            "cutBasedPhotonID_Spring15_25ns_V1_standalone_tight",
                            "mvaPhoID_Spring15_25ns_nonTrig_V2p1_wp90",
                             ),

    rhoLabel = cms.InputTag("fixedGridRhoFastjetAll"),
    mcLabel = cms.InputTag("prunedGenParticles"),
    vertexLabel = cms.InputTag('catVertex'),
    beamLineSrc = cms.InputTag("offlineBeamSpot"),
    photonIDSources = cms.PSet()

)
