import os

def makeNtupleMakerData(sampledir,samplelist, outputdir, job, runtrig):

    ntuplemaker=open("../run_ntupleMaker_snu_data_cfg.py","r")
    config=""

    fread = open(samplelist, 'r')
    samplecounter=0
    for samples in fread:
        samplecounter+=1
    fread.close()

    for line in ntuplemaker:
        if "root://cms-xrd" in line:
            config+=makeConfigFile(sampledir,samplelist, samplecounter) +"\n"
        elif  "fileName = cms.string" in line:
                config+='fileName = cms.string("' + outputdir + '/ntuple' + str(job) +'.root"),'

        elif "runFullTrig" in line: 
            if runtrig:
                config+='runFullTrig= cms.bool(True),' +"\n"
            else:
                config+='runFullTrig= cms.bool(False),' +"\n"

        else:
            config+=line.strip() +"\n"

    return config

def makeNtupleMaker(sampledir,samplelist, outputdir, job, runfullgen):

    pfile="../run_ntupleMaker_snu_mc_cfg.py"
    if runfullgen == True:
         pfile="../run_ntupleMaker_snu_mc_fullgen_cfg.py"

    ntuplemaker=open(pfile, 'r')
    config=""

    fread = open(samplelist, 'r')
    samplecounter=0
    for samples in fread:
        samplecounter+=1
    fread.close()    
        
    for line in ntuplemaker:
        if "root://cms-xrd" in line:
            config+=makeConfigFile(sampledir,samplelist, samplecounter) +"\n"
        elif  "fileName = cms.string" in line:
                config+='fileName = cms.string("' + outputdir + '/ntuple' + str(job) +'.root"),'


        else:
            config+=line.strip() +"\n"
        
    return config 

def MakeFullNtupleMaker(sampledir,samplelist, count):
    config='import FWCore.ParameterSet.Config as cms\n'
    config+='import os\n'
    config+='\n'
    config+="process = cms.Process('Ana')\n"
    config+='\n'
    config+='process.load("FWCore.MessageLogger.MessageLogger_cfi")\n'    
    config+='\n'
    config+='process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )\n'
    config+='process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )\n'
    config+='process.MessageLogger.cerr.FwkReport.reportEvery = 10000\n'
    config+='\n'
    config+='process.source = cms.Source("PoolSource",\n'
    config+='          fileNames = cms.untracked.vstring("\n'
    config+=makeConfigFile(sampledir,samplelist, count)
    config+=')\n'
    config+=')\n'

    config+='\n'
    config+='\n'

    config+='process.nEventsTotal = cms.EDProducer("EventCountProducer")\n'
    config+='process.ntuple = cms.EDAnalyzer("GenericNtupleMaker",\n'
    config+='    failureMode = cms.untracked.string("keep"),\n'
    config+='    eventCounters = cms.vstring("nEventsTotal"), \n'
    config+='int = cms.PSet(\n'
    config+='nVertex   = cms.PSet(src = cms.InputTag("recoEventInfo","pvN")),\n'
    config+='HLTDoubleMu = cms.PSet(src = cms.InputTag("recoEventInfo","HLTDoubleMu")),\n'
    config+='HLTDoubleEl = cms.PSet(src = cms.InputTag("recoEventInfo","HLTDoubleEl")),\n'
    config+='HLTMuEl = cms.PSet(src = cms.InputTag("recoEventInfo","HLTMuEl")),\n'
    config+='HLTSingleMu = cms.PSet(src = cms.InputTag("recoEventInfo","HLTSingleMu")),\n'
    config+='HLTSingleEl = cms.PSet(src = cms.InputTag("recoEventInfo","HLTSingleEl")),\n'
    config+='),\n'
    config+='double = cms.PSet(\n'
    config+='puWeight   = cms.PSet(src = cms.InputTag("pileupWeight")),\n'
    config+='    puWeightUp = cms.PSet(src = cms.InputTag("pileupWeight", "up")),\n'
    config+='    puWeightDn = cms.PSet(src = cms.InputTag("pileupWeight", "dn")),\n'
    config+='        pvX   = cms.PSet(src = cms.InputTag("recoEventInfo","pvX")),\n'
    config+='pvY   = cms.PSet(src = cms.InputTag("recoEventInfo","pvY")),\n'
    config+='pvZ   = cms.PSet(src = cms.InputTag("recoEventInfo","pvZ")),\n'
    config+='   ),\n'
    config+='doubles = cms.PSet(\n'
    config+='pdfWeight = cms.PSet(src = cms.InputTag("pdfWeight")),\n'
    config+='    ),\n'
    config+='cands = cms.PSet(\n'
    config+='muon = cms.PSet(\n'
    config+='src = cms.InputTag("catMuons"),\n'
    config+='\n'
    config+='            exprs = cms.untracked.PSet(\n'
    config+='                pt  = cms.string("pt"),\n'
    config+='                eta = cms.string("eta"),\n'
    config+='phi = cms.string("phi"),\n'
    config+='m   = cms.string("mass"),\n'
    config+='relIso03 = cms.string("relIso(0.3)"),\n'
    config+='                relIso04 = cms.string("relIso(0.4)"),\n'
    config+='                isTracker = cms.string("isTrackerMuon"),\n'
    config+='                isGlobal = cms.string("isGlobalMuon"),\n'
    config+='                isLoose = cms.string("isLooseMuon"),\n'
    config+='	   isTight = cms.string("isTightMuon"),\n'
    config+='                dxy = cms.string("dxy"),\n'
    config+='                normchi = cms.string("normalizedChi2"),\n'
    config+='validhits = cms.string("numberOfValidHits"),\n'
    config+='validmuonhits = cms.string("numberOfValidMuonHits"),\n'
    config+='dz = cms.string("dz"),\n'
    config+='   q = cms.string("charge"),\n'
    config+='            ),\n'
    config+='selections = cms.untracked.PSet(),\n'
    config+='),\n'
    config+='electrons = cms.PSet(\n'
    config+='src = cms.InputTag("catElectrons"),\n'
    config+='exprs = cms.untracked.PSet(\n'
    config+='pt  = cms.string("pt"),\n'
    config+='eta = cms.string("eta"),\n'
    config+='phi = cms.string("phi"),\n'
    config+='m   = cms.string("mass"),\n'
    config+='shiftedEnDown = cms.string("shiftedEnDown"),\n'
    config+='shiftedEnUp = cms.string("shiftedEnUp"),\n'
    config+='mcMatched = cms.string("mcMatched"),\n'
    config+='isPF = cms.string("isPF"),\n'
    config+='relIso03 = cms.string("relIso(0.3)"),\n'
    config+='relIso04 = cms.string("relIso(0.4)"),\n'
    config+='chIso03 = cms.string("chargedHadronIso(0.3)"),\n'
    config+='nhIso03 = cms.string("neutralHadronIso(0.3)"),\n'
    config+='phIso03 = cms.string("photonIso(0.3)"),\n'
    config+='puChIso03 = cms.string("puChargedHadronIso(0.3)"),\n'
    config+='chIso04 = cms.string("chargedHadronIso(0.4)"),\n'
    config+='nhIso04 = cms.string("neutralHadronIso(0.4)"),\n'
    config+='phIso04 = cms.string("photonIso(0.4)"),\n'
    config+='puChIso04 = cms.string("puChargedHadronIso(0.4)"),\n'
    config+='\n'
    config+='scEta = cms.string("scEta"),\n'
    config+='\n'
    config+='passConversionVeto = cms.string("passConversionVeto"),\n'
    config+='q = cms.string("charge"),\n'
    config+='),\n'
    config+='selections = cms.untracked.PSet(\n'
    config+='isPassBaseId = cms.string("passConversionVeto && isPF && gsfTrack.hitPattern.numberOfLostHits(\'MISSING_INNER_HITS\') <= 0"),\n'
    config+='),\n'
    config+='),\n'
    config+='jets = cms.PSet(\n'
    config+='src = cms.InputTag("catJets"),\n'
    config+='exprs = cms.untracked.PSet(\n'
    config+='pt  = cms.string("pt"),\n'
    config+='eta = cms.string("eta"),\n'
    config+='phi = cms.string("phi"),\n'
    config+='m   = cms.string("mass"),\n'
    config+='vtxMass = cms.string("vtxMass"),\n'
    config+='CSVInclV2 = cms.string("bDiscriminator(\'pfCombinedInclusiveSecondaryVertexV2BJetTags\')"),\n'
    config+='\n'
    config+='partonFlavour = cms.string("partonFlavour"),\n'
    config+='hadronFlavour = cms.string("hadronFlavour"),\n'
    config+='partonPdgId = cms.string("partonPdgId"),\n'
    config+='shiftedEnDown = cms.string("shiftedEnDown"),\n'
    config+='shiftedEnUp = cms.string("shiftedEnUp"),\n'
    config+='smearedRes = cms.string("smearedRes"),\n'
    config+='smearedResDown = cms.string("smearedResDown"),\n'
    config+='smearedResUp = cms.string("smearedResUp"),\n'
    config+='),\n'
    config+='selections = cms.untracked.PSet(\n'
    config+='isLoose = cms.string("LooseId"),\n'
    config+='isTight = cms.string("TightId"),\n'
    config+='isPFId = cms.string("pileupJetId"),\n'
    config+='),\n'
    config+='),\n'
    config+='met = cms.PSet(\n'
    config+='src = cms.InputTag("catMETs"),\n'
    config+='exprs = cms.untracked.PSet(\n'
    config+='pt  = cms.string("pt"),\n'
    config+='phi = cms.string("phi"),\n'
    config+='),\n'
    config+='selections = cms.untracked.PSet(),\n'
    config+='),\n'
    config+='\n'
    config+='slimmedGenJets = cms.PSet(\n'
    config+='src = cms.InputTag("slimmedGenJets",""),\n'
    config+='\n'
    config+='exprs = cms.untracked.PSet(\n'
    config+='pt  = cms.string("pt"),\n'
    config+='            eta = cms.string("eta"),\n'
    config+='phi = cms.string("phi"),\n'
    config+='m   = cms.string("mass"),\n'
    config+='),\n'
    config+='selections = cms.untracked.PSet(),\n'
    config+='),\n'
    config+=')\n'
    config+='\n'
    config+='process.TFileService = cms.Service("TFileService",\n'
    config+='fileName = cms.string("ntuple.root"),\n'
    config+=')\n'
    config+='\n'
    config+='process.load("CATTools.CatProducer.pseudoTop_cff")\n'
    config+='process.p = cms.Path(\n'
    config+='process.nEventsTotal*\n'
    config+='process.partonTop*\n'
    config+='    process.ntuple\n'
    config+=')\n'

    return config


def makeConfigFile(sampledir,samplelist, count):
    
    fr = open(samplelist, 'r')

    config=''
    counter=0

    for line in fr:
        counter+=1
        if not "/failed" in line:
            fileline="'root://cms-xrdr.sdfarm.kr:1094//" +  line.strip()+"'"
        if not counter ==  count :
            if not "/failed" in line:
                fileline+=","
            
        config+=fileline
    fr.close()     
    return config
