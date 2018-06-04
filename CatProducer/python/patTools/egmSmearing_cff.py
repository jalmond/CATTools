import FWCore.ParameterSet.Config as cms

## Energy smearing and scale correction
## https://twiki.cern.ch/twiki/bin/view/CMS/EGMSmearer

def enableElectronSmearing(process, runOnMC=True):
    process.load('EgammaAnalysis.ElectronTools.calibratedPatElectronsRun2_cfi')

    process.RandomNumberGeneratorService.calibratedPatElectrons = cms.PSet(
        initialSeed = cms.untracked.uint32(81),
        engineName = cms.untracked.string('TRandom3')
    )

    process.calibratedPatElectrons.isMC = runOnMC
    process.selectedElectrons = cms.EDFilter(
        "PATElectronSelector",
        src = cms.InputTag("slimmedElectrons"),
        cut = cms.string("pt > 5 && abs(eta)<2.5")
        )
    process.catElectrons.src = "calibratedPatElectrons"
    process.calibratedPatElectrons.electrons = cms.InputTag('selectedElectrons')
    
    return process

def enablePhotonSmearing(process, runOnMC=True):
    process.load('EgammaAnalysis.ElectronTools.calibratedPatPhotonsRun2_cfi')

    process.RandomNumberGeneratorService.calibratedPatPhotons = cms.PSet(
        initialSeed = cms.untracked.uint32(81),
        engineName = cms.untracked.string('TRandom3')
    )

    process.calibratedPatPhotons.isMC = runOnMC
    process.catPhotons.src = "calibratedPatPhotons"

    return process

