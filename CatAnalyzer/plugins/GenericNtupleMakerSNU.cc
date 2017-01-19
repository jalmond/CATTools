#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/MergeableCounter.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Candidate/interface/LeafCandidate.h"

#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"


#include "CommonTools/Utils/interface/StringObjectFunction.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

#include "CATTools/CommonTools/interface/AnalysisHelper.h"


#include "CATTools/DataFormats/interface/MET.h"
#include "CATTools/DataFormats/interface/Muon.h"
#include "CATTools/DataFormats/interface/Jet.h"
#include "CATTools/DataFormats/interface/FatJet.h"
#include "CATTools/DataFormats/interface/Electron.h"
#include "CATTools/DataFormats/interface/GenJet.h"
#include "CATTools/DataFormats/interface/GenWeights.h"


#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "CATTools/DataFormats/interface/Trigger.h"


#include "CATTools/CatAnalyzer/src/rochcor2016.h"
#include "CATTools/CatAnalyzer/src/RoccoR.h"


#include "TTree.h"
#include "TH1F.h"

#include <boost/regex.hpp>


#include <memory>
#include <vector>
#include <string>
//#include <stdlib.h> 

using namespace std;
using namespace edm;
using namespace cat;


typedef std::vector<float> vfloat;

template<typename T, typename S>
class PairConsumers
{
public:
  typedef edm::ParameterSet PSet;
  
  void init(const edm::ParameterSet& gpset, const string psetName, edm::ConsumesCollector && iC, TTree* tree)
  {
    if ( !gpset.existsAs<PSet>(psetName) ) return;
    const PSet pset = gpset.getParameter<PSet>(psetName);
    const auto names = pset.getParameterNamesForType<PSet>();
    for ( auto& name : names )
      {
	const auto ipset = pset.getParameter<PSet>(name);
	tokens_.push_back(iC.consumes<std::pair<T,S> >(ipset.getParameter<edm::InputTag>("src")));
	values_.push_back(new std::pair<T,S>);
	tree->Branch(name.c_str(), values_.back());
      }
    const auto labels = pset.getParameterNamesForType<edm::InputTag>();
    for ( auto& name : labels )
      {
	tokens_.push_back(iC.consumes<std::pair<T,S> >(pset.getParameter<edm::InputTag>(name)));
	values_.push_back(new std::pair<T,S>);
	tree->Branch(name.c_str(), values_.back());
      }
  }

  int load(const edm::Event& event)
  {
    int nFailure = 0;
    for ( size_t i=0, n=tokens_.size(); i<n; ++i )
      {
	edm::Handle<std::pair<T,S> > handle;
	event.getByToken(tokens_[i], handle);
	if ( handle.isValid() )
	  {
	    //values_[i]->insert(values_[i]->begin(), handle->begin(), handle->end());
	  }
	else
	  {
	    ++nFailure;
	  }
      }

    return nFailure;
  }

  void clear()
  {
    for ( auto& v : values_ ) v->clear();
  }

private:
  std::vector<edm::EDGetTokenT<std::pair<T,S> > > tokens_;
  std::vector<std::pair<T,S>*> values_;

};

template<typename T>
class VectorConsumers
{
public:
  typedef edm::ParameterSet PSet;

  void init(const edm::ParameterSet& gpset, const string psetName, edm::ConsumesCollector && iC, TTree* tree)
  {
    if ( !gpset.existsAs<PSet>(psetName) ) return;
    const PSet pset = gpset.getParameter<PSet>(psetName);
    const auto names = pset.getParameterNamesForType<PSet>();
    for ( auto& name : names )
    {
      const auto ipset = pset.getParameter<PSet>(name);
      tokens_.push_back(iC.consumes<std::vector<T> >(ipset.getParameter<edm::InputTag>("src")));
      values_.push_back(new std::vector<T>);
      tree->Branch(name.c_str(), values_.back());
    }
    const auto labels = pset.getParameterNamesForType<edm::InputTag>();
    for ( auto& name : labels )
    {
      tokens_.push_back(iC.consumes<std::vector<T> >(pset.getParameter<edm::InputTag>(name)));
      values_.push_back(new std::vector<T>);
      tree->Branch(name.c_str(), values_.back());
    }
  }

  int load(const edm::Event& event)
  {
    int nFailure = 0;
    for ( size_t i=0, n=tokens_.size(); i<n; ++i )
    {
      edm::Handle<std::vector<T> > handle;
      event.getByToken(tokens_[i], handle);
      if ( handle.isValid() )
      {
        values_[i]->insert(values_[i]->begin(), handle->begin(), handle->end());
      }
      else
      {
        ++nFailure;
      }
    }

    return nFailure;
  }

  void clear()
  {
    for ( auto& v : values_ ) v->clear();
  }

private:
  std::vector<edm::EDGetTokenT<std::vector<T> > > tokens_;
  std::vector<std::vector<T>*> values_;

};

template<typename T>
class FlatConsumers
{
public:
  typedef edm::ParameterSet PSet;

  void init(const edm::ParameterSet& gpset, const string psetName, edm::ConsumesCollector && iC,
            TTree* tree, const char* typeNameStr)
  {
    if ( !gpset.existsAs<PSet>(psetName) ) return;
    const PSet pset = gpset.getParameter<PSet>(psetName);
    const auto names = pset.getParameterNamesForType<PSet>();
    for ( auto& name : names )
    {
      const auto ipset = pset.getParameter<PSet>(name);
      tokens_.push_back(iC.consumes<T>(ipset.getParameter<edm::InputTag>("src")));
      values_.push_back(new T);
      tree->Branch(name.c_str(), values_.back(), (name+"/"+typeNameStr).c_str());
    }
    const auto labels = pset.getParameterNamesForType<edm::InputTag>();
    for ( auto& name : labels )
    {
      tokens_.push_back(iC.consumes<T>(pset.getParameter<edm::InputTag>(name)));
      values_.push_back(new T);
      tree->Branch(name.c_str(), values_.back(), (name+"/"+typeNameStr).c_str());
    }
  }

  int load(const edm::Event& event)
  {
    int nFailure = 0;
    for ( size_t i=0, n=tokens_.size(); i<n; ++i )
    {
      edm::Handle<T> handle;
      event.getByToken(tokens_[i], handle);
      if ( handle.isValid() ) *values_[i] = *handle;
      else
      {
        *values_[i] = -999;
        ++nFailure;
      }
    }

    return nFailure;
  }

private:
  std::vector<edm::EDGetTokenT<T> > tokens_;
  std::vector<T*> values_;

};


class GenericNtupleMakerSNU : public edm::EDAnalyzer
{
public:
  GenericNtupleMakerSNU(const edm::ParameterSet& pset);
  ~GenericNtupleMakerSNU();

  void analyze(const edm::Event& event, const edm::EventSetup& eventSetup) override;
  void endLuminosityBlock(const edm::LuminosityBlock& lumi, const edm::EventSetup& eventSetup) override;
  std::vector<const reco::Candidate *> getAncestors(const reco::Candidate &c);
  bool hasBottom(const reco::Candidate &c);
  bool hasCharm(const reco::Candidate &c);
  const reco::Candidate* lastBHadron(const reco::Candidate &c);
  const reco::Candidate* lastCHadron(const reco::Candidate &c);

private:
  edm::EDGetTokenT<reco::GenParticleCollection> mcLabel_;
  edm::EDGetTokenT<reco::GenJetCollection> genjet_;

  
  vector<cat::Muon> selectMuons(const edm::View<cat::Muon>* muons );
  vector<cat::Jet> selectJets(const edm::View<cat::Jet>* jets );
  vector<cat::FatJet> selectFatJets(const edm::View<cat::FatJet>* fatjets );
  vector<cat::Electron> selectElecs(const edm::View<cat::Electron>* electrons );
  
  typedef edm::ParameterSet PSet;
  typedef std::vector<double> vdouble;
  typedef std::vector<int> vint;
  typedef std::vector<bool> vbool;
  typedef std::vector<std::string> strings;
  

  std::vector<std::vector<vdouble*> > candVars_;
  typedef edm::View<reco::LeafCandidate> CandView;
  typedef edm::ValueMap<double> Vmap;
  typedef edm::EDGetTokenT<CandView> CandToken;
  typedef edm::EDGetTokenT<Vmap> VmapToken;
  
  edm::EDGetTokenT<edm::TriggerResults> triggerBits_;
  edm::EDGetTokenT<edm::TriggerResults> triggerBits2_;

  edm::EDGetTokenT<pat::TriggerObjectStandAloneCollection> triggerObjects_;
  edm::EDGetTokenT<pat::PackedTriggerPrescales> triggerPrescales_;

  edm::EDGetTokenT<edm::TriggerResults> metFilterBitsPAT_;
  edm::EDGetTokenT<edm::TriggerResults> metFilterBitsRECO_;

  edm::EDGetTokenT<cat::TriggerNames> trigNamesToken_;
  edm::EDGetTokenT<cat::TriggerBits> trigBitsToken_;
  
  edm::EDGetTokenT<reco::VertexCollection >   vtxToken_;
  
  /// Testing MET
  std::vector< std::pair < std::string, std::string> >metFilterNames_;
  
  /// CAT objects
  std::vector<CandToken> candTokens_;
  std::vector<std::vector<VmapToken> > vmapTokens_;

  std::vector<edm::EDGetTokenT<edm::MergeableCounter> > eventCounterTokens_;
  
  FlatConsumers<bool> boolCSet_;
  FlatConsumers<int> intCSet_;
  FlatConsumers<double> doubleCSet_;
  FlatConsumers<float> floatCSet_;

  //FlatConsumers<std::string> stringCSet_;
  VectorConsumers<bool> vboolCSet_;
  VectorConsumers<int> vintCSet_;
  VectorConsumers<double> vdoubleCSet_;
  VectorConsumers<float> vfloatCSet_;

  VectorConsumers<std::string> vstringCSet_;


  typedef StringObjectFunction<reco::Candidate,true> CandFtn;
  typedef StringCutObjectSelector<reco::Candidate,true> CandSel;

  std::vector<int> indices_;
  std::vector<std::vector<CandFtn> > exprs_;
  std::vector<std::vector<CandSel> > selectors_;

  /// Process specific variables
  bool runFullTrig;
  bool keepAllGen;
  bool makeSlim;
  bool store_allweights;
  TH1F* hNEvent_;

  TTree* tree_;

  ///// variables to store for sktrees

  int runNumber_, lumiNumber_, eventNumber_;
  double vertex_X, vertex_Y, vertex_Z;
  bool IsData_;

  std::string CatVersion_;


  
  //// Bools 
  /// Muons  8
  vector<bool> muon_isTrackerMuon, muon_isGlobalMuon, muon_isLooseMuon, muon_isMediumMuon, muon_isTightMuon, muon_isSoftMuon, muon_mcMatched, muon_isPFMuon;
  /// Electrons 13
  vector<bool>  electrons_electronID_loose,electrons_electronID_medium,electrons_electronID_tight,electrons_electronID_veto,electrons_electronID_mva_medium,electrons_electronID_mva_tight,electrons_electronID_mva_trig_medium,electrons_electronID_mva_trig_tight,electrons_electronID_heep,  electrons_mcMatched,electrons_isPF,electrons_passConversionVeto,electrons_isTrigMVAValid;
  /// Jest 3
  vector<bool>  jets_looseJetID,jets_tightJetID,jets_tightLepVetoJetID;
  vector<bool>  fatjets_looseJetID,fatjets_tightJetID,fatjets_tightLepVetoJetID;
  
  /////// ints
  ///// muon 6
  std::vector<int>  muon_numberOfValidHits, muon_numberOfValidMuonHits,muon_numberOfMatchedStations,muon_numberOfValidPixelHits,muon_trackerLayersWithMeasurement,muon_charge;
  
  /// electrons 2
  std::vector<int>  electrons_snuID, electrons_charge;

  /// jets 4
  std::vector<int>  jets_partonFlavour,jets_hadronFlavour,jets_partonPdgId,jets_vtxNtracks;
  std::vector<int>  fatjets_partonFlavour,fatjets_hadronFlavour,fatjets_partonPdgId,fatjets_vtxNtracks;

//// double
  //// muon  15
  std::vector<double>  muon_x,muon_y,muon_z, muon_relIso03,muon_relIso04, muon_dxy,muon_sigdxy, muon_normchi, muon_dz, muon_shiftedEup,muon_shiftedEdown;
  
  std::vector<double> muon_pt, muon_eta,muon_phi, muon_m, muon_energy;
  std::vector<double> muon_roch_pt, muon_roch_eta,muon_roch_phi, muon_roch_m, muon_roch_energy;

  //// electrons  26
  std::vector<double>   electrons_x,electrons_y,electrons_z, electrons_relIso03,electrons_relIso04,electrons_shiftedEnDown, electrons_shiftedEnUp, electrons_absIso03, electrons_absIso04,electrons_chIso03, electrons_nhIso03, electrons_phIso03, electrons_puChIso03, electrons_chIso04, electrons_nhIso04, electrons_phIso04, electrons_puChIso04, electrons_scEta, electrons_dxy,electrons_sigdxy, electrons_dz, electrons_isGsfCtfScPixChargeConsistent;
  
  std::vector<double> electrons_pt, electrons_eta,electrons_phi, electrons_m, electrons_energy;

  //// jets  21
  std::vector<double>   jets_vtxMass, jets_vtx3DVal, jets_vtx3DSig, jets_CSVInclV2, jets_JetProbBJet, jets_CMVAV2, jets_chargedEmEnergyFraction, jets_shiftedEnDown, jets_shiftedEnUp, jets_smearedRes, jets_smearedResDown, jets_smearedResUp, jets_PileupJetId, jets_iCSVCvsL,jets_CCvsLT, jets_CCvsBT;
  std::vector<double>   fatjets_vtxMass, fatjets_vtx3DVal, fatjets_vtx3DSig, fatjets_CSVInclV2, fatjets_JetProbBJet, fatjets_CMVAV2, fatjets_chargedEmEnergyFraction, fatjets_shiftedEnDown, fatjets_shiftedEnUp, fatjets_smearedRes, fatjets_smearedResDown, fatjets_smearedResUp, fatjets_PileupJetId, fatjets_iCSVCvsL,fatjets_CCvsLT, fatjets_CCvsBT , fatjets_tau1, fatjets_tau2,fatjets_tau3, fatjets_prunedmass, fatjets_softdropmass;
  
  std::vector<double> jets_pt, jets_eta,jets_phi, jets_m, jets_energy;
  std::vector<double> fatjets_pt, fatjets_eta,fatjets_phi, fatjets_m, fatjets_energy;
  std::vector<double> fatjets_puppi_pt, fatjets_puppi_eta,fatjets_puppi_phi, fatjets_puppi_m;
  std::vector<double> fatjets_puppi_tau1, fatjets_puppi_tau2,fatjets_puppi_tau3;;

  bool Flag_HBHENoiseIsoFilter, Flag_HBHENoiseFilter, Flag_CSCTightHaloFilter, Flag_goodVertices, Flag_eeBadScFilter, Flag_EcalDeadCellTriggerPrimitiveFilter,  Flag_globalTightHalo2016Filter;

  double met_muonEn_Px_up, met_muonEn_Px_down, met_muonEn_Py_up, met_muonEn_Py_down;
  double met_electronEn_Px_up, met_electronEn_Px_down, met_electronEn_Py_up, met_electronEn_Py_down;

  std::vector<std::string> vtrignames;
  std::vector<std::string> muon_trigmatch;
  std::vector<std::string> electron_trigmatch;
  std::vector<int> vtrigps;
  std::vector<int> gen_pdgid_;
  std::vector<int> gen_status_;
  std::vector<int> gen_motherindex_;
  std::vector<int> GenJet_pdgid_;
  std::vector<float> gen_energy_;
  std::vector<float> gen_eta_;
  std::vector<float> gen_phi_;
  std::vector<float> gen_pt_;

  std::vector<bool>  gen_isprompt_;
  std::vector<bool>  gen_isdecayedleptonhadron_;
  std::vector<bool>  gen_istaudecayproduct_;
  std::vector<bool>  gen_isprompttaudecayproduct_;
  std::vector<bool>  gen_isdirecthadrondecayproduct_;
 
  std::vector<bool>  gen_ishardprocess_;
  std::vector<bool>  gen_fromhardprocess_;
  std::vector<bool>  gen_fromhardprocess_beforeFSR_;
  
  /// Gen Info stored as float to save space
  std::vector<float> GenJet_eta_;
  std::vector<float> GenJet_pt_;
  std::vector<float> GenJet_phi_;
  std::vector<float> GenJet_energy_;
  std::vector<float> GenJet_emf_;
  std::vector<float> GenJet_hadf_;

  std::vector<float> ScaleWeight_;
  std::vector<float> PDFWeight_;

  float genWeightQ_;
  float genWeightX1_;
  float genWeightX2_;
  int genWeight_id1_;
  int genWeight_id2_;
  float genWeight_;
  float lheWeight_;

  rochcor2016 *rmcor;

  edm::EDGetTokenT<cat::METCollection>      metToken_;
  edm::EDGetTokenT<edm::View<cat::Muon> >     muonToken_;
  edm::EDGetTokenT<edm::View<cat::Jet> >     jetToken_;
  edm::EDGetTokenT<edm::View<cat::FatJet> >     fatjetToken_;
  edm::EDGetTokenT<edm::View<cat::Electron> > elecToken_;
  edm::EDGetTokenT<cat::GenWeights>              genWeightToken_;
  edm::EDGetTokenT<vfloat> pdfweightsToken_, scaleupweightsToken_, scaledownweightsToken_;


  /// bool
  std::vector<std::vector<vbool*> > cand_boolVars_;
  typedef edm::View<reco::LeafCandidate> Cand_boolView;
  typedef edm::ValueMap<bool> Vmap_bool;
  typedef edm::EDGetTokenT<CandView> Cand_boolToken;
  typedef edm::EDGetTokenT<Vmap_bool> Vmap_boolToken;

  std::vector<CandToken> cand_boolTokens_;  
  std::vector<std::vector<Vmap_boolToken> > vmap_boolTokens_;

  typedef StringObjectFunction<reco::Candidate,true> Cand_boolFtn;
  typedef StringCutObjectSelector<reco::Candidate,true> Cand_boolSel;

  std::vector<std::vector<CandFtn> > exprs_bool_;
  std::vector<std::vector<CandSel> > selectors_bool_;
  std::vector<int> indices_bool_;
  
  /// ints
  std::vector<std::vector<vint*> > cand_intVars_;
  typedef edm::View<reco::LeafCandidate> Cand_intView;
  typedef edm::ValueMap<int> Vmap_int;
  typedef edm::EDGetTokenT<CandView> Cand_intToken;
  typedef edm::EDGetTokenT<Vmap_int> Vmap_intToken;

  std::vector<CandToken> cand_intTokens_;
  std::vector<std::vector<Vmap_intToken> > vmap_intTokens_;



  typedef StringObjectFunction<reco::Candidate,true> Cand_intFtn;
  typedef StringCutObjectSelector<reco::Candidate,true> Cand_intSel;

  std::vector<std::vector<CandFtn> > exprs_int_;
  std::vector<std::vector<CandSel> > selectors_int_;
  std::vector<int> indices_int_;
  

  struct FAILUREMODE
  {
    enum { KEEP, SKIP, ERROR };
  };
  int failureMode_;
};

GenericNtupleMakerSNU::~GenericNtupleMakerSNU(){
  
  delete rmcor;
}

GenericNtupleMakerSNU::GenericNtupleMakerSNU(const edm::ParameterSet& pset)
{
  std::string failureMode = pset.getUntrackedParameter<std::string>("failureMode", "keep");
  std::transform(failureMode.begin(), failureMode.end(), failureMode.begin(), ::tolower);
  if ( failureMode == "keep" ) failureMode_ = FAILUREMODE::KEEP;
  else if ( failureMode == "skip" ) failureMode_ = FAILUREMODE::SKIP;
  else if ( failureMode == "error" ) failureMode_ = FAILUREMODE::ERROR;
  else throw cms::Exception("ConfigError") << "select one from \"keep\", \"skip\", \"error\"\n";

  //  triggers_      = consumes<vector<pair<string, int> > >(pset.getParameter<edm::InputTag>("trigLabel"));
  genjet_    =consumes<reco::GenJetCollection>(pset.getParameter<edm::InputTag>("genjet"));
  mcLabel_   = consumes<reco::GenParticleCollection>(pset.getParameter<edm::InputTag>("genLabel"));

  triggerBits_ = consumes<edm::TriggerResults>(pset.getParameter<edm::InputTag>("triggerBits"));
  triggerBits2_ = consumes<edm::TriggerResults>(pset.getParameter<edm::InputTag>("triggerBits2"));

  triggerObjects_ = consumes<pat::TriggerObjectStandAloneCollection>(pset.getParameter<edm::InputTag>("triggerObjects"));
  triggerPrescales_ =consumes<pat::PackedTriggerPrescales>(pset.getParameter<edm::InputTag>("triggerPrescales"));

  //  trigNamesToken_ = consumes<cat::TriggerNames, edm::InLumi>(pset.getParameter<edm::InputTag>("src"));
  //trigBitsToken_ = consumes<cat::TriggerBits>(pset.getParameter<edm::InputTag>("src"));


  metToken_  = consumes<cat::METCollection>(pset.getParameter<edm::InputTag>("met"));
  muonToken_ = consumes<edm::View<cat::Muon> >(pset.getParameter<edm::InputTag>("muons"));
  elecToken_ = consumes<edm::View<cat::Electron> >(pset.getParameter<edm::InputTag>("electrons"));
  jetToken_ = consumes<edm::View<cat::Jet> >(pset.getParameter<edm::InputTag>("jets"));
  fatjetToken_ = consumes<edm::View<cat::FatJet> >(pset.getParameter<edm::InputTag>("fatjets"));
  metFilterBitsPAT_ = consumes<edm::TriggerResults>(pset.getParameter<edm::InputTag>("metFilterBitsPAT"));
  metFilterBitsRECO_ = consumes<edm::TriggerResults>(pset.getParameter<edm::InputTag>("metFilterBitsRECO"));
  vtxToken_  = consumes<reco::VertexCollection >(pset.getParameter<edm::InputTag>("vertices"));
  /// new weights
  genWeightToken_       = consumes<cat::GenWeights>              (pset.getParameter<edm::InputTag>("genWeightLabel"));

  pdfweightsToken_ = consumes<vfloat>(pset.getParameter<edm::InputTag>("pdfweights"));
  scaleupweightsToken_ = consumes<vfloat>(pset.getParameter<edm::InputTag>("scaleupweights"));
  scaledownweightsToken_ = consumes<vfloat>(pset.getParameter<edm::InputTag>("scaledownweights"));

  rmcor = new rochcor2016();

  //// bool to specify job
  runFullTrig = pset.getParameter<bool>("runFullTrig");
  keepAllGen = pset.getParameter<bool>("keepAllGen");
  makeSlim = pset.getParameter<bool>("makeSlim");
  store_allweights = pset.getParameter<bool>("allweights");
  if(runFullTrig) cout << "Running fulltrigger" << endl;
  else cout << "Not running full trigger" << endl;
  //// Test MET


  // Output histograms and tree
  edm::Service<TFileService> fs;
  tree_ = fs->make<TTree>("event", "event");
  
  /// I = int, i = UInt, F=Double, O=bool
  tree_->Branch("run"  , &runNumber_  , "run/I"  );
  tree_->Branch("lumi" , &lumiNumber_ , "lumi/I" );
  tree_->Branch("event", &eventNumber_, "event/I");
  
  tree_->Branch("vertex_X", &vertex_X, "vertex_X/D");
  tree_->Branch("vertex_Y", &vertex_Y, "vertex_Y/D");
  tree_->Branch("vertex_Z", &vertex_Z, "vertex_Z/D");

  /// Set MET systematic variables
  tree_->Branch("met_muonEn_Px_up", &met_muonEn_Px_up, "met_muonEn_Px_up/D");
  tree_->Branch("met_muonEn_Py_up", &met_muonEn_Py_up, "met_muonEn_Py_up/D");
  tree_->Branch("met_muonEn_Px_down", &met_muonEn_Px_down, "met_muonEn_Px_down/D");
  tree_->Branch("met_muonEn_Py_down", &met_muonEn_Py_down, "met_muonEn_Py_down/D");
  tree_->Branch("met_electronEn_Px_up", &met_electronEn_Px_up, "met_electronEn_Px_up/D");
  tree_->Branch("met_electronEn_Py_up", &met_electronEn_Py_up, "met_electronEn_Py_up/D");
  tree_->Branch("met_electronEn_Px_down", &met_electronEn_Px_down, "met_electronEn_Px_down/D");
  tree_->Branch("met_electronEn_Py_down", &met_electronEn_Py_down, "met_electronEn_Py_down/D");

  /// Do not need to give classname of buffsize for string
  tree_->Branch("CatVersion", &CatVersion_);  

  tree_->Branch("IsData", &IsData_ , "IsData/O");
  tree_->Branch("HBHENoiseFilter", &Flag_HBHENoiseFilter , "HBHENoiseFilter/O");
  tree_->Branch("HBHENoiseIsoFilter", &Flag_HBHENoiseIsoFilter , "HBHENoiseIsoFilter/O");
  tree_->Branch("CSCTightHaloFilter", &Flag_CSCTightHaloFilter, "CSCTightHaloFilter/O");
  tree_->Branch("goodVertices", &Flag_goodVertices,"goodVertices/O");
  tree_->Branch("eeBadScFilter", &Flag_eeBadScFilter,"eeBadScFilter/O");
  tree_->Branch("EcalDeadCellTriggerPrimitiveFilter", &Flag_EcalDeadCellTriggerPrimitiveFilter,"EcalDeadCellTriggerPrimitiveFilter/O");
  tree_->Branch("Flag_globalTightHalo2016Filter",&Flag_globalTightHalo2016Filter,"Flag_globalTightHalo2016Filter/O");

  tree_->Branch("genWeightQ", &genWeightQ_, "genWeightQ/F");
  tree_->Branch("genWeightX1", &genWeightX1_,"genWeightX1/F");
  tree_->Branch("genWeightX2", &genWeightX2_,"genWeightX2/F");
  tree_->Branch("genWeight_id1", &genWeight_id1_,"genWeight_id1/I");
  tree_->Branch("genWeight_id2", &genWeight_id2_,"genWeight_id2/I");
  tree_->Branch("genWeight", &genWeight_,"genWeight/F");
  tree_->Branch("lheWeight", &lheWeight_,"lheWeight/F");

  /// Vectors (do not need to give classname but will)

  //// strings
  tree_->Branch("muon_trigmatch", "std::vector<std::string>", &muon_trigmatch);
  tree_->Branch("electron_trigmatch","std::vector<std::string>", &electron_trigmatch);
  tree_->Branch("vtrignames","std::vector<std::string>", &vtrignames);

  //// bools
  
  tree_->Branch("muon_isTracker",  "std::vector<bool>", &muon_isTrackerMuon);
  tree_->Branch("muon_isGlobal",  "std::vector<bool>", &muon_isGlobalMuon);
  tree_->Branch("muon_isLoose",  "std::vector<bool>", &muon_isLooseMuon);
  tree_->Branch("muon_isMedium",  "std::vector<bool>", &muon_isMediumMuon);
  tree_->Branch("muon_isTight",  "std::vector<bool>", &muon_isTightMuon);
  tree_->Branch("muon_isSoft",  "std::vector<bool>", &muon_isSoftMuon);
  tree_->Branch("muon_matched",  "std::vector<bool>", &muon_mcMatched);
  tree_->Branch("muon_isPF",  "std::vector<bool>", &muon_isPFMuon);

  tree_->Branch("electrons_electronID_loose",  "std::vector<bool>", &electrons_electronID_loose);
  tree_->Branch("electrons_electronID_medium",  "std::vector<bool>", &electrons_electronID_medium);
  tree_->Branch("electrons_electronID_tight",  "std::vector<bool>", &electrons_electronID_tight);
  tree_->Branch("electrons_electronID_veto",  "std::vector<bool>", &electrons_electronID_veto);
  tree_->Branch("electrons_electronID_mva_medium",  "std::vector<bool>", &electrons_electronID_mva_medium);
  tree_->Branch("electrons_electronID_mva_tight",  "std::vector<bool>", &electrons_electronID_mva_tight);
  tree_->Branch("electrons_electronID_mva_trig_medium",  "std::vector<bool>", &electrons_electronID_mva_trig_medium);
  tree_->Branch("electrons_electronID_mva_trig_tight",  "std::vector<bool>", &electrons_electronID_mva_trig_tight);
  tree_->Branch("electrons_electronID_heep",  "std::vector<bool>", &electrons_electronID_heep);
  tree_->Branch("electrons_mcMatched",  "std::vector<bool>", &electrons_mcMatched);
  tree_->Branch("electrons_isPF",  "std::vector<bool>", &electrons_isPF);
  tree_->Branch("electrons_passConversionVeto",  "std::vector<bool>", &electrons_passConversionVeto);
  tree_->Branch("electrons_isTrigMVAValid",  "std::vector<bool>", &electrons_isTrigMVAValid);


  /// Jets                                                                                                                                                                          
  tree_->Branch("jets_isLoose",  "std::vector<bool>", &jets_looseJetID);
  tree_->Branch("jets_isTight",  "std::vector<bool>", &jets_tightJetID);
  tree_->Branch("jets_isTightLepVetoJetID",  "std::vector<bool>", &jets_tightLepVetoJetID);
  tree_->Branch("fatjets_isLoose",  "std::vector<bool>", &fatjets_looseJetID);
  tree_->Branch("fatjets_isTight",  "std::vector<bool>", &fatjets_tightJetID);
  tree_->Branch("fatjets_isTightLepVetoJetID",  "std::vector<bool>", &jets_tightLepVetoJetID);

  tree_->Branch("gen_isprompt", "std::vector<bool>", &gen_isprompt_);
  tree_->Branch("gen_isdecayedleptonhadron", "std::vector<bool>", &gen_isdecayedleptonhadron_);
  tree_->Branch("gen_istaudecayproduct","std::vector<bool>", &gen_istaudecayproduct_);
  tree_->Branch("gen_isprompttaudecayproduct", "std::vector<bool>",&gen_isprompttaudecayproduct_);
  tree_->Branch("gen_isdirecthadrondecayproduct", "std::vector<bool>",&gen_isdirecthadrondecayproduct_);
  tree_->Branch("gen_ishardprocess", "std::vector<bool>",&gen_ishardprocess_);
  tree_->Branch("gen_fromhardprocess","std::vector<bool>", &gen_fromhardprocess_);
  tree_->Branch("gen_fromhardprocess_beforeFSR", "std::vector<bool>",&gen_fromhardprocess_beforeFSR_);



  /////// ints                                                                                                                                                                      

  tree_->Branch("vtrigps", "std::vector<int>", &vtrigps);
  ///// muon                                                                                                                                                                        
  tree_->Branch("muon_validhits",  "std::vector<int>", &muon_numberOfValidHits);
  tree_->Branch("muon_validmuonhits",  "std::vector<int>", &muon_numberOfValidMuonHits);
  tree_->Branch("muon_matchedstations",  "std::vector<int>", &muon_numberOfMatchedStations);
  tree_->Branch("muon_validpixhits",  "std::vector<int>", &muon_numberOfValidPixelHits);
  tree_->Branch("muon_trackerlayers",  "std::vector<int>", &muon_trackerLayersWithMeasurement);
  tree_->Branch("muon_q",  "std::vector<int>", &muon_charge);
  /// electrons                                                                                                                                                                     
  tree_->Branch("electrons_electronID_snu",  "std::vector<int>", &electrons_snuID);
  tree_->Branch("electrons_q",  "std::vector<int>", &electrons_charge);

  /// jets                                                                                                                                                                          
  
  tree_->Branch("jets_partonFlavour",  "std::vector<int>", &jets_partonFlavour);
  tree_->Branch("jets_hadronFlavour",  "std::vector<int>", &jets_hadronFlavour);
  tree_->Branch("jets_partonPdgId",  "std::vector<int>", &jets_partonPdgId);
  tree_->Branch("jets_vtxNtracks",  "std::vector<int>", &jets_vtxNtracks);
  tree_->Branch("fatjets_partonFlavour",  "std::vector<int>", &fatjets_partonFlavour);
  tree_->Branch("fatjets_hadronFlavour",  "std::vector<int>", &fatjets_hadronFlavour);
  tree_->Branch("fatjets_partonPdgId",  "std::vector<int>", &fatjets_partonPdgId);
  tree_->Branch("fatjets_vtxNtracks",  "std::vector<int>", &fatjets_vtxNtracks);



  tree_->Branch("gen_status", "std::vector<int>",  &gen_status_);
  tree_->Branch("gen_pdgid", "std::vector<int>", &gen_pdgid_);
  tree_->Branch("gen_motherindex", "std::vector<int>", &gen_motherindex_);

  tree_->Branch("genjet_pdgid", "std::vector<int>", &GenJet_pdgid_);


  //// doubles                                                                                                                                                                       
  //// muon                                                                                                                                                                         

  tree_->Branch("muon_x",  "std::vector<double>", &muon_x);
  tree_->Branch("muon_y",  "std::vector<double>", &muon_y);
  tree_->Branch("muon_z",  "std::vector<double>", &muon_z);
  tree_->Branch("muon_pt",  "std::vector<double>", &muon_pt);
  tree_->Branch("muon_eta",  "std::vector<double>", &muon_eta);
  tree_->Branch("muon_phi",  "std::vector<double>", &muon_phi);
  tree_->Branch("muon_m",  "std::vector<double>", &muon_m);
  tree_->Branch("muon_energy",  "std::vector<double>", &muon_energy);

  tree_->Branch("muon_roch_pt",  "std::vector<double>", &muon_roch_pt);
  tree_->Branch("muon_roch_eta",  "std::vector<double>", &muon_roch_eta);
  tree_->Branch("muon_roch_phi",  "std::vector<double>", &muon_roch_phi);
  tree_->Branch("muon_roch_m",  "std::vector<double>", &muon_roch_m);
  tree_->Branch("muon_roch_energy",  "std::vector<double>", &muon_roch_energy);
  tree_->Branch("muon_dxy",  "std::vector<double>", &muon_dxy);
  tree_->Branch("muon_sigdxy",  "std::vector<double>", &muon_sigdxy);
  tree_->Branch("muon_dz",  "std::vector<double>", &muon_dz);
  tree_->Branch("muon_normchi" ,  "std::vector<double>", &muon_normchi);
  tree_->Branch("muon_relIso03",  "std::vector<double>", &muon_relIso03);
  tree_->Branch("muon_relIso04",  "std::vector<double>", &muon_relIso04);
  tree_->Branch("muon_shiftedEdown",  "std::vector<double>", &muon_shiftedEdown);
  tree_->Branch("muon_shiftedEup",  "std::vector<double>", &muon_shiftedEup);


  //// electronss                                                                                                                                                                    
  tree_->Branch("electrons_x",  "std::vector<double>", &electrons_x);
  tree_->Branch("electrons_y",  "std::vector<double>", &electrons_y);
  tree_->Branch("electrons_z",  "std::vector<double>", &electrons_z);
  tree_->Branch("electrons_pt",  "std::vector<double>", &electrons_pt);
  tree_->Branch("electrons_eta",  "std::vector<double>", &electrons_eta);
  tree_->Branch("electrons_phi",  "std::vector<double>", &electrons_phi);
  tree_->Branch("electrons_m",  "std::vector<double>", &electrons_m);
  tree_->Branch("electrons_energy",  "std::vector<double>", &electrons_energy);
  tree_->Branch("electrons_relIso03",  "std::vector<double>", &electrons_relIso03);
  tree_->Branch("electrons_relIso04",  "std::vector<double>", &electrons_relIso04);
  tree_->Branch("electrons_shiftedEnDown",  "std::vector<double>", &electrons_shiftedEnDown);
  tree_->Branch("electrons_shiftedEnUp",  "std::vector<double>", &electrons_shiftedEnUp);
  tree_->Branch("electrons_absIso03",  "std::vector<double>", &electrons_absIso03);
  tree_->Branch("electrons_absIso04",  "std::vector<double>", &electrons_absIso04);
  tree_->Branch("electrons_chIso03",  "std::vector<double>", &electrons_chIso03);
  tree_->Branch("electrons_chIso04",  "std::vector<double>", &electrons_chIso04);
  tree_->Branch("electrons_nhIso03",  "std::vector<double>", &electrons_nhIso03);
  tree_->Branch("electrons_nhIso04",  "std::vector<double>", &electrons_nhIso04);
  tree_->Branch("electrons_phIso03",  "std::vector<double>", &electrons_phIso03);
  tree_->Branch("electrons_phIso04",  "std::vector<double>", &electrons_phIso04);
  tree_->Branch("electrons_scEta",  "std::vector<double>", &electrons_scEta);
  tree_->Branch("electrons_dxy",  "std::vector<double>", &electrons_dxy);
  tree_->Branch("electrons_sigdxy",  "std::vector<double>", &electrons_sigdxy);
  tree_->Branch("electrons_dz",  "std::vector<double>", &electrons_dz);
  tree_->Branch("electrons_isGsfCtfScPixChargeConsistent",  "std::vector<double>", &electrons_isGsfCtfScPixChargeConsistent);
  tree_->Branch("electrons_puChIso03",  "std::vector<double>", &electrons_puChIso03);
  tree_->Branch("electrons_puChIso04",  "std::vector<double>", &electrons_puChIso04);

  //// jets                                                                                                                                                                         
  tree_->Branch("jets_pt",  "std::vector<double>", &jets_pt);
  tree_->Branch("jets_eta",  "std::vector<double>", &jets_eta);
  tree_->Branch("jets_phi",  "std::vector<double>", &jets_phi);
  tree_->Branch("jets_m",  "std::vector<double>", &jets_m);
  tree_->Branch("jets_energy",  "std::vector<double>", &jets_energy);
  tree_->Branch("jets_vtxMass",  "std::vector<double>", &jets_vtxMass);
  tree_->Branch("jets_vtx3DVal",  "std::vector<double>", &jets_vtx3DVal);
  tree_->Branch("jets_vtx3DSig",  "std::vector<double>", &jets_vtx3DSig);
  tree_->Branch("jets_CSVInclV2",  "std::vector<double>", &jets_CSVInclV2);
  tree_->Branch("jets_iCSVCvsL",  "std::vector<double>", &jets_iCSVCvsL);
  tree_->Branch("jets_CCvsLT",  "std::vector<double>", &jets_CCvsLT);
  tree_->Branch("jets_CCvsBT",  "std::vector<double>", &jets_CCvsBT);
  tree_->Branch("jets_JetProbBJet",  "std::vector<double>", &jets_JetProbBJet);
  tree_->Branch("jets_CMVAV2",  "std::vector<double>", &jets_CMVAV2);
  tree_->Branch("jets_chargedEmEnergyFraction",  "std::vector<double>", &jets_chargedEmEnergyFraction);
  tree_->Branch("jets_shiftedEnDown",  "std::vector<double>", &jets_shiftedEnDown);
  tree_->Branch("jets_shiftedEnUp",  "std::vector<double>", &jets_shiftedEnUp);
  tree_->Branch("jets_smearedRes",  "std::vector<double>", &jets_smearedRes);
  tree_->Branch("jets_smearedResDown",  "std::vector<double>", &jets_smearedResDown);
  tree_->Branch("jets_smearedResUp",  "std::vector<double>", &jets_smearedResUp);
  tree_->Branch("jets_PileupJetId",  "std::vector<double>", &jets_PileupJetId);


  tree_->Branch("fatjets_pt",  "std::vector<double>", &fatjets_pt);
  tree_->Branch("fatjets_eta",  "std::vector<double>", &fatjets_eta);
  tree_->Branch("fatjets_phi",  "std::vector<double>", &fatjets_phi);
  tree_->Branch("fatjets_m",  "std::vector<double>", &fatjets_m);
  tree_->Branch("fatjets_energy",  "std::vector<double>", &fatjets_energy);
  tree_->Branch("fatjets_vtxMass",  "std::vector<double>", &fatjets_vtxMass);
  tree_->Branch("fatjets_vtx3DVal",  "std::vector<double>", &fatjets_vtx3DVal);
  tree_->Branch("fatjets_vtx3DSig",  "std::vector<double>", &fatjets_vtx3DSig);
  tree_->Branch("fatjets_CSVInclV2",  "std::vector<double>", &fatjets_CSVInclV2);
  tree_->Branch("fatjets_iCSVCvsL",  "std::vector<double>", &fatjets_iCSVCvsL);
  tree_->Branch("fatjets_CCvsLT",  "std::vector<double>", &fatjets_CCvsLT);
  tree_->Branch("fatjets_CCvsBT",  "std::vector<double>", &fatjets_CCvsBT);
  tree_->Branch("fatjets_JetProbBJet",  "std::vector<double>", &fatjets_JetProbBJet);
  tree_->Branch("fatjets_CMVAV2",  "std::vector<double>", &fatjets_CMVAV2);
  tree_->Branch("fatjets_chargedEmEnergyFraction",  "std::vector<double>", &fatjets_chargedEmEnergyFraction);
  tree_->Branch("fatjets_shiftedEnDown",  "std::vector<double>", &fatjets_shiftedEnDown);
  tree_->Branch("fatjets_shiftedEnUp",  "std::vector<double>", &fatjets_shiftedEnUp);
  tree_->Branch("fatjets_smearedRes",  "std::vector<double>", &fatjets_smearedRes);
  tree_->Branch("fatjets_smearedResDown",  "std::vector<double>", &fatjets_smearedResDown);
  tree_->Branch("fatjets_smearedResUp",  "std::vector<double>", &fatjets_smearedResUp);
  tree_->Branch("fatjets_PileupJetId",  "std::vector<double>", &fatjets_PileupJetId);
  
  tree_->Branch("fatjets_tau1",  "std::vector<double>", &fatjets_tau1);
  tree_->Branch("fatjets_tau2",  "std::vector<double>", &fatjets_tau2);
  tree_->Branch("fatjets_tau3",  "std::vector<double>", &fatjets_tau3);
  tree_->Branch("fatjets_prunedmass",  "std::vector<double>", &fatjets_prunedmass);
  tree_->Branch("fatjets_softdropmass",  "std::vector<double>", &fatjets_softdropmass);

  tree_->Branch("fatjets_puppi_pt",  "std::vector<double>", &fatjets_puppi_pt);
  tree_->Branch("fatjets_puppi_eta",  "std::vector<double>", &fatjets_puppi_eta);
  tree_->Branch("fatjets_puppi_phi",  "std::vector<double>", &fatjets_puppi_phi);
  tree_->Branch("fatjets_puppi_m",  "std::vector<double>", &fatjets_puppi_m);
  tree_->Branch("fatjets_puppi_tau1",  "std::vector<double>", &fatjets_puppi_tau1);
  tree_->Branch("fatjets_puppi_tau2",  "std::vector<double>", &fatjets_puppi_tau2);
  tree_->Branch("fatjets_puppi_tau3",  "std::vector<double>", &fatjets_puppi_tau3);

  tree_->Branch("gen_pt", "std::vector<float>", &gen_pt_);
  tree_->Branch("gen_eta", "std::vector<float>",  &gen_eta_);
  tree_->Branch("gen_phi", "std::vector<float>",  &gen_phi_);
  tree_->Branch("gen_energy",  "std::vector<float>", &gen_energy_);

  tree_->Branch("genjet_pt", "std::vector<float>", &GenJet_pt_);
  tree_->Branch("genjet_eta", "std::vector<float>",&GenJet_eta_);
  tree_->Branch("genjet_phi", "std::vector<float>",&GenJet_phi_);
  tree_->Branch("genjet_energy", "std::vector<float>",&GenJet_energy_);
  tree_->Branch("genjet_emf", "std::vector<float>",&GenJet_emf_);
  tree_->Branch("genjet_hadf", "std::vector<float>",&GenJet_hadf_);

  tree_->Branch("ScaleWeights", "std::vector<float>", &ScaleWeight_);
  tree_->Branch("PDFWeights", "std::vector<float>",&PDFWeight_);


  boolCSet_.init(pset, "bool", consumesCollector(), tree_, "O");
  intCSet_.init(pset, "int", consumesCollector(), tree_, "I");
  doubleCSet_.init(pset, "double", consumesCollector(), tree_, "D");
  floatCSet_.init(pset, "float", consumesCollector(), tree_, "F");

  //stringCSet_.init(pset, "string", consumesCollector(), tree_, "F");
  vboolCSet_.init(pset, "bools", consumesCollector(), tree_);
  vintCSet_.init(pset, "ints", consumesCollector(), tree_);
  vdoubleCSet_.init(pset, "doubles", consumesCollector(), tree_);
  vfloatCSet_.init(pset, "floats", consumesCollector(), tree_);
  vstringCSet_.init(pset, "strings", consumesCollector(), tree_);

  
  for ( auto& hltPath : pset.getParameter<strings>("metFilterNames") ){
    //    produces<bool >( hltPath );
    metFilterNames_.push_back(std::make_pair("Flag_"+hltPath, hltPath));
  }

  
  PSet candPSets = pset.getParameter<PSet>("cands");
  const strings candNames = candPSets.getParameterNamesForType<PSet>();
  for ( auto& candName : candNames )
    {
      PSet candPSet = candPSets.getParameter<PSet>(candName);
      
      edm::InputTag candToken = candPSet.getParameter<edm::InputTag>("src");
      candTokens_.push_back(consumes<CandView>(candToken));
      exprs_.push_back(std::vector<CandFtn>());
      selectors_.push_back(std::vector<CandSel>());
      vmapTokens_.push_back(std::vector<VmapToken>());
      candVars_.push_back(std::vector<vdouble*>());
      const string candTokenName = candToken.label();
      indices_.push_back(candPSet.getUntrackedParameter<int>("index", -1));
      const PSet exprSets = candPSet.getUntrackedParameter<PSet>("exprs", PSet());
      for ( auto& exprName : exprSets.getParameterNamesForType<string>() )
	{
	  const string expr = exprSets.getParameter<string>(exprName);
	  candVars_.back().push_back(new vdouble);
	  exprs_.back().push_back(CandFtn(expr));
	  
	  tree_->Branch((candName+"_"+exprName).c_str(), candVars_.back().back());
	}
      const PSet selectionSets = candPSet.getUntrackedParameter<PSet>("seletions", PSet());
      for ( auto& selectionName : selectionSets.getParameterNamesForType<string>() )
	{
	  const string selection = selectionSets.getParameter<string>(selectionName);
	  candVars_.back().push_back(new vdouble);
	  selectors_.back().push_back(CandSel(selection));

	  tree_->Branch((candName+"_"+selectionName).c_str(), candVars_.back().back());
	}
      const strings vmapNames = candPSet.getUntrackedParameter<strings>("vmaps", strings());
      for ( auto& vmapName : vmapNames )
	{
	  candVars_.back().push_back(new vdouble);

	  edm::InputTag vmapToken(candTokenName, vmapName);
	  vmapTokens_.back().push_back(consumes<Vmap>(vmapToken));

	  tree_->Branch((candName+"_"+vmapName).c_str(), candVars_.back().back());
	}
    }

  

  /// bool
  PSet cand_boolPSets = pset.getParameter<PSet>("cands_bool");
  const strings cand_boolNames = cand_boolPSets.getParameterNamesForType<PSet>();

    
  for ( auto& cand_boolName : cand_boolNames )
    {
      PSet cand_boolPSet = cand_boolPSets.getParameter<PSet>(cand_boolName);

      edm::InputTag cand_boolToken = cand_boolPSet.getParameter<edm::InputTag>("src");
      cand_boolTokens_.push_back(consumes<CandView>(cand_boolToken));

      exprs_bool_.push_back(std::vector<CandFtn>());
      selectors_bool_.push_back(std::vector<CandSel>());
      vmap_boolTokens_.push_back(std::vector<Vmap_boolToken>());
      cand_boolVars_.push_back(std::vector<vbool*>());

      const string cand_boolTokenName = cand_boolToken.label();
      indices_bool_.push_back(cand_boolPSet.getUntrackedParameter<int>("index", -1));
      const PSet exprSets = cand_boolPSet.getUntrackedParameter<PSet>("exprs", PSet());

      for ( auto& exprName : exprSets.getParameterNamesForType<string>() )
	{
	  const string expr = exprSets.getParameter<string>(exprName);
	  cand_boolVars_.back().push_back(new vbool);
	  exprs_bool_.back().push_back(CandFtn(expr));

	  tree_->Branch((cand_boolName+"_"+exprName).c_str(), cand_boolVars_.back().back());
	}
      const PSet selectionSets = cand_boolPSet.getUntrackedParameter<PSet>("seletions", PSet());
      for ( auto& selectionName : selectionSets.getParameterNamesForType<string>() )
	{
	  const string selection = selectionSets.getParameter<string>(selectionName);
	  cand_boolVars_.back().push_back(new vbool);
	  selectors_bool_.back().push_back(CandSel(selection));

	  tree_->Branch((cand_boolName+"_"+selectionName).c_str(), cand_boolVars_.back().back());
	}
      const strings vmap_boolNames = cand_boolPSet.getUntrackedParameter<strings>("vmap_bools", strings());
      for ( auto& vmap_boolName : vmap_boolNames )
	{
	  cand_boolVars_.back().push_back(new vbool);

	  edm::InputTag vmap_boolToken(cand_boolTokenName, vmap_boolName);
	  vmap_boolTokens_.back().push_back(consumes<Vmap_bool>(vmap_boolToken));
	  
	  tree_->Branch((cand_boolName+"_"+vmap_boolName).c_str(), cand_boolVars_.back().back());
	}
    }


  /// int
  PSet cand_intPSets = pset.getParameter<PSet>("cands_int");
  const strings cand_intNames = cand_intPSets.getParameterNamesForType<PSet>();


  for ( auto& cand_intName : cand_intNames )
    {
      PSet cand_intPSet = cand_intPSets.getParameter<PSet>(cand_intName);

      edm::InputTag cand_intToken = cand_intPSet.getParameter<edm::InputTag>("src");
      cand_intTokens_.push_back(consumes<CandView>(cand_intToken));

      exprs_int_.push_back(std::vector<CandFtn>());
      selectors_int_.push_back(std::vector<CandSel>());
      vmap_intTokens_.push_back(std::vector<Vmap_intToken>());
      cand_intVars_.push_back(std::vector<vint*>());

      const string cand_intTokenName = cand_intToken.label();
      indices_int_.push_back(cand_intPSet.getUntrackedParameter<int>("index", -1));
      const PSet exprSets = cand_intPSet.getUntrackedParameter<PSet>("exprs", PSet());

      for ( auto& exprName : exprSets.getParameterNamesForType<string>() )
        {
          const string expr = exprSets.getParameter<string>(exprName);
          cand_intVars_.back().push_back(new vint);
          exprs_int_.back().push_back(CandFtn(expr));

          tree_->Branch((cand_intName+"_"+exprName).c_str(), cand_intVars_.back().back());
        }
      const PSet selectionSets = cand_intPSet.getUntrackedParameter<PSet>("seletions", PSet());
      for ( auto& selectionName : selectionSets.getParameterNamesForType<string>() )
        {
          const string selection = selectionSets.getParameter<string>(selectionName);
          cand_intVars_.back().push_back(new vint);
          selectors_int_.back().push_back(CandSel(selection));

          tree_->Branch((cand_intName+"_"+selectionName).c_str(), cand_intVars_.back().back());
        }
      const strings vmap_intNames = cand_intPSet.getUntrackedParameter<strings>("vmap_ints", strings());
      for ( auto& vmap_intName : vmap_intNames )
        {
          cand_intVars_.back().push_back(new vint);

	  edm::InputTag vmap_intToken(cand_intTokenName, vmap_intName);
          vmap_intTokens_.back().push_back(consumes<Vmap_int>(vmap_intToken));

          tree_->Branch((cand_intName+"_"+vmap_intName).c_str(), cand_intVars_.back().back());
        }
    }

  const strings eventCounters = pset.getParameter<strings>("eventCounters");

  const size_t nEventCounter = eventCounters.size();
  hNEvent_ = fs->make<TH1F>("hNEvent", "NEvent", nEventCounter, 0, nEventCounter);
  for ( size_t i=0; i<nEventCounter; ++i )
  {
    hNEvent_->GetXaxis()->SetBinLabel(i+1, eventCounters[i].c_str());
    eventCounterTokens_.push_back(consumes<edm::MergeableCounter, edm::InLumi>(edm::InputTag(eventCounters[i])));
  }

}

void GenericNtupleMakerSNU::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  typedef edm::View<reco::LeafCandidate> Cands;
  
  int nFailure = 0;

  //rmcor = new rochcor2016();
  
  //// In case config is wrong set false for data
  if(event.isRealData()) store_allweights=false;
  
  /// since v7-6-4 store genweights using CAT class
  if(!event.isRealData()) {
    edm::Handle<cat::GenWeights> genWeightHandle;
    event.getByToken(genWeightToken_, genWeightHandle);
    
    /// only store pdf/scale weights for specific MC
    if(store_allweights){  
      
      edm::Handle<vfloat> pdfweightsHandle;
      event.getByToken(pdfweightsToken_, pdfweightsHandle);
      for ( auto& w : *pdfweightsHandle )  PDFWeight_.push_back(w);
      
      edm::Handle<vfloat> scaleupweightsHandle;
      event.getByToken(scaleupweightsToken_,scaleupweightsHandle);
      for ( auto& w :*scaleupweightsHandle) ScaleWeight_.push_back(w);
      
      edm::Handle<vfloat> scaledownweightsHandle;
      event.getByToken(scaledownweightsToken_,scaledownweightsHandle);
      for ( auto& w :*scaledownweightsHandle) ScaleWeight_.push_back(w);
    }
  
    /// store these for all MC
    genWeightQ_ = genWeightHandle->qScale();
    genWeightX1_ = genWeightHandle->x1();
    genWeightX2_ = genWeightHandle->x2();
    genWeight_id1_ = genWeightHandle->id1();
    genWeight_id2_ = genWeightHandle->id2();
    genWeight_ = genWeightHandle->genWeight();
    lheWeight_ = genWeightHandle->lheWeight();
  }
  else{
    genWeightQ_ =  -999.;
    genWeightX1_ =  -999.;
    genWeightX2_ = -999.;
    genWeight_id1_ = -999;
    genWeight_id2_ = -999;
    genWeight_ = -999.;
    lheWeight_ = -999.;
  }// end of signal weights
  
  ////// Fill vertex information
  edm::Handle<reco::VertexCollection> vertices;
  event.getByToken(vtxToken_, vertices);
  
  for (auto &vtx : *vertices){
    vertex_X = vtx.x(); 
    vertex_Y = vtx.y(); 
    vertex_Z = vtx.z();     
  }


  //// Fill Trigger Information
  //// Fill Trigger Information
  edm::Handle<edm::TriggerResults> triggerBits;
  edm::Handle<pat::TriggerObjectStandAloneCollection> triggerObjects;
  edm::Handle<pat::PackedTriggerPrescales> triggerPrescales;


  if (! event.getByToken(triggerBits2_, triggerBits)){
    event.getByToken(triggerBits_, triggerBits);
  }
  
  event.getByToken(triggerObjects_, triggerObjects);
  event.getByToken(triggerPrescales_, triggerPrescales);

  const edm::TriggerNames &trigNames = event.triggerNames(*triggerBits);  

  // save filter info                                                                                                                                                            //  const auto trigNames = trigNamesHandle->names();
  //  for ( int index=0, nTrig=trigNames.size(); index<nTrig; ++index ) {
  for( unsigned int i=0; i<trigNames.size(); ++i ){
    TString tname = TString(trigNames.triggerName(i));

    if( (! (trigNames.triggerName(i).find("HLT_DoublePhoton") == 0 || trigNames.triggerName(i).find("HLT_Photon") == 0))
	 && tname.Contains("Photon")) continue;
    if (trigNames.triggerName(i).find("HLT_Ele") == 0 
	|| trigNames.triggerName(i).find("HLT_DoubleEle") == 0 
	|| trigNames.triggerName(i).find("HLT_IsoMu") == 0 
	|| trigNames.triggerName(i).find("HLT_TkMu") == 0 
	|| trigNames.triggerName(i).find("HLT_Mu") == 0
	|| trigNames.triggerName(i).find("HLT_DiMu") == 0
	|| trigNames.triggerName(i).find("HLT_TripleMu_") == 0
	|| trigNames.triggerName(i).find("HLT_Photon") == 0 
	|| trigNames.triggerName(i).find("HLT_DoublePhoton") == 0){
      if(!(tname.Contains("Jpsi") 
	   || tname.Contains("NoFilters") 
	   || tname.Contains("Upsilon")
	   || tname.Contains("7p5")
	   || tname.Contains("Save")
	   || tname.Contains("R9Id")
	   || tname.Contains("PFMET")
	   || tname.Contains("PFHT")
	   || tname.Contains("NoHE")
	   || tname.Contains("HE10")
	   || tname.Contains("PFJet50")
	   || tname.Contains("Boost")
	   || tname.Contains("LooseIso")
	   || tname.Contains("MediumIso")
	   || tname.Contains("Mass")
	   || tname.Contains("Central")
	   || tname.Contains("MW")
	   || tname.Contains("EBOnly_VBF")
	   || tname.Contains("dEta18"))) {
	if(runFullTrig){
	  if( (!(tname.Contains("PF")|| tname.Contains("WP"))) || (tname.Contains("PFJet")|| tname.Contains("WPLoose") )) {
	    vtrignames.push_back(trigNames.triggerName(i));
	    if(triggerBits->accept(i)){
	      int psValue = int(triggerBits->accept(i)) * triggerPrescales->getPrescaleForIndex(i);
	      vtrigps.push_back(psValue);
	    }
	    else  vtrigps.push_back(0);
	  }
	}
	else if(!(tname.Contains("PF")|| tname.Contains("WP"))) {
	  vtrignames.push_back(trigNames.triggerName(i));
	  if(triggerBits->accept(i)){
	    int psValue = int(triggerBits->accept(i)) * triggerPrescales->getPrescaleForIndex(i);
	    vtrigps.push_back(psValue);
	  }
	  else  vtrigps.push_back(0);
	}
      }
    }
  }

  std::vector<string> vtrignames_tomatch_muon;
  vtrignames_tomatch_muon.push_back("HLT_IsoMu24_eta2p1_v");
  vtrignames_tomatch_muon.push_back("HLT_Ele25_WPTight_Gsf_v");
  vtrignames_tomatch_muon.push_back("HLT_Ele27_WPTight_Gsf_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu17_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu20_v");
  vtrignames_tomatch_muon.push_back("HLT_TkMu20_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu8_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu17_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu24_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu8_TrkIsoVVL_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu17_TrkIsoVVL_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu24_TrkIsoVVL_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu27_v");
  vtrignames_tomatch_muon.push_back("HLT_IsoMu20_v");
  vtrignames_tomatch_muon.push_back("HLT_IsoTkMu20_v");
  vtrignames_tomatch_muon.push_back("HLT_IsoMu22_v");
  vtrignames_tomatch_muon.push_back("HLT_IsoTkMu22_v");
  vtrignames_tomatch_muon.push_back("HLT_IsoMu24_v");
  vtrignames_tomatch_muon.push_back("HLT_IsoTkMu24_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu17_Mu8_DZ_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu17_TkMu8_DZ_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v");
  vtrignames_tomatch_muon.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v");
  vtrignames_tomatch_muon.push_back("HLT_Ele12_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL_v");
  vtrignames_tomatch_muon.push_back("HLT_DiMu9_Ele9_CaloIdL_TrackIdL_v");
  vtrignames_tomatch_muon.push_back("HLT_TripleMu_12_10_5_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu8_DiEle12_CaloIdL_TrackIdL_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu17_Mu8_SameSign_DZ_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
  std::vector<string> vtrignames_tomatch_electron;
  vtrignames_tomatch_electron.push_back("HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL_v");
  vtrignames_tomatch_electron.push_back("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
  vtrignames_tomatch_electron.push_back("HLT_Ele32_eta2p1_WPTight_Gsf_v");
  vtrignames_tomatch_electron.push_back("HLT_Ele12_CaloIdL_TrackIdL_IsoVL_v");
  vtrignames_tomatch_electron.push_back("HLT_Ele17_CaloIdL_TrackIdL_IsoVL_v");
  vtrignames_tomatch_electron.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
  vtrignames_tomatch_electron.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_");
  vtrignames_tomatch_electron.push_back("HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v");
  vtrignames_tomatch_electron.push_back("HLT_Ele27_eta2p1_WPLoose_Gsf_v");
  vtrignames_tomatch_electron.push_back("HLT_Ele27_eta2p1_WPLoose_Gsf_TriCentralPFJet30_v");
  vtrignames_tomatch_electron.push_back("HLT_DiMu9_Ele9_CaloIdL_TrackIdL_v");
  vtrignames_tomatch_electron.push_back("HLT_Mu8_DiEle12_CaloIdL_TrackIdL_v");
  vtrignames_tomatch_electron.push_back("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v");
  vtrignames_tomatch_electron.push_back("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
  vtrignames_tomatch_electron.push_back("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v");
  vtrignames_tomatch_electron.push_back("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v");
  vtrignames_tomatch_electron.push_back("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_v");
  vtrignames_tomatch_electron.push_back("HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v");
  vtrignames_tomatch_electron.push_back("HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v");
  //vtrignames_tomatch_muon.push_back(CatVersion_);  

  ////////// Fill MET/Muon/Electron variables
  edm::Handle<edm::View<cat::Muon> > muons;
  event.getByToken(muonToken_, muons);
  edm::Handle<edm::View<cat::Electron> > electrons;
  event.getByToken(elecToken_, electrons);
  
  edm::Handle<edm::View<cat::Jet> > jets;
  event.getByToken(jetToken_, jets);
  
  
  edm::Handle<edm::View<cat::FatJet> > fatjets;
  event.getByToken(fatjetToken_, fatjets);


  double el_pt_min= 9.;
  double el_eta_max= 3.;
  double mu_pt_min= 9.;
  double mu_eta_max= 3.;
  double j_pt_min= 15.;
  double j_eta_max= 5.;
  if(!makeSlim){
    el_pt_min= 0.;
    el_eta_max= 10.;
    mu_pt_min= 0.;
    mu_eta_max= 10.;
    j_pt_min= 0.;
    j_eta_max= 10.;
  }
  for (auto el : *electrons) {

    if(el.pt() < el_pt_min) continue;
    if(fabs(el.eta()) > el_eta_max) continue;
    if(el.pt() != el.pt()) continue;

    electrons_electronID_loose.push_back(el.electronID("cutBasedElectronID-Summer16-80X-V1-loose"));
    electrons_electronID_medium.push_back(el.electronID("cutBasedElectronID-Summer16-80X-V1-medium"));
    electrons_electronID_tight.push_back(el.electronID("cutBasedElectronID-Summer16-80X-V1-tight"));
    electrons_electronID_veto.push_back(el.electronID("cutBasedElectronID-Summer16-80X-V1-veto"));
    electrons_electronID_mva_medium.push_back(el.electronID("EleID-Spring16-GeneralPurpose-V1-wp90"));
    electrons_electronID_mva_tight.push_back(el.electronID("EleID-Spring16-GeneralPurpose-V1-wp80"));
    electrons_electronID_mva_trig_medium.push_back(el.electronID("EleID-Spring16-GeneralPurpose-V1-wp90"));
    electrons_electronID_mva_trig_tight.push_back(el.electronID("EleID-Spring16-GeneralPurpose-V1-wp80"));
    electrons_electronID_heep.push_back(el.electronID("heepElectronID-HEEPV60"));

    electrons_mcMatched.push_back(el.mcMatched());
    electrons_isPF.push_back(el.isPF());
    electrons_passConversionVeto.push_back(el.passConversionVeto());
    electrons_isTrigMVAValid.push_back(el.isTrigMVAValid());
    electrons_snuID.push_back(el.snuID()); 
    electrons_charge.push_back(el.charge());
    electrons_x.push_back(el.vx());
    electrons_y.push_back(el.vy());
    electrons_z.push_back(el.vz());
    electrons_pt.push_back(el.pt());
    electrons_eta.push_back(el.eta());
    electrons_phi.push_back(el.phi());
    electrons_m.push_back(el.mass()); 
    electrons_energy.push_back(el.energy()); 
    electrons_relIso03.push_back(el.relIso(0.3));
    electrons_relIso04.push_back(el.relIso(0.4));
    electrons_shiftedEnDown.push_back(el.shiftedEnDown()); 
    electrons_shiftedEnUp.push_back(el.shiftedEnUp()); 
    electrons_absIso03.push_back(el.absIso(0.3)); 
    electrons_absIso04.push_back(el.absIso(0.4));
    electrons_chIso03.push_back(el.chargedHadronIso(0.3));
    electrons_nhIso03.push_back(el.neutralHadronIso(0.3)); 
    electrons_phIso03.push_back(el.photonIso(0.3)); 
    electrons_puChIso03.push_back(el.puChargedHadronIso(0.3)); 
    electrons_chIso04.push_back(el.chargedHadronIso(0.4)); 
    electrons_nhIso04.push_back(el.neutralHadronIso(0.4)); 
    electrons_phIso04.push_back(el.photonIso(0.4)); 
    electrons_puChIso04.push_back(el.puChargedHadronIso(0.4)); 
    electrons_scEta.push_back(el.scEta()); 
    electrons_dxy.push_back(el.dxy()); 
    electrons_sigdxy.push_back(el.ipsignificance());
    electrons_dz.push_back(el.dz());
    electrons_isGsfCtfScPixChargeConsistent.push_back(el.isGsfCtfScPixChargeConsistent());
    
  }
  

  
  for (auto mu : *muons) {

    cat::Muon cat_mu(mu);

    if(mu.pt() < mu_pt_min) continue;
    if(fabs(mu.eta()) > mu_eta_max) continue;

    TLorentzVector tmu(cat_mu.tlv());
    float qter = 1.0;
    
    if (!event.isRealData())       rmcor->momcor_mc(tmu, cat_mu.charge(), cat_mu.trackerLayersWithMeasurement(), qter);
    else       rmcor->momcor_data(tmu, cat_mu.charge(), 0, qter);
    
    cat_mu.setP4(mu.p4() * tmu.E()/cat_mu.tlv().E());
    
    muon_roch_pt.push_back(cat_mu.pt());
    muon_roch_eta.push_back(cat_mu.eta());
    muon_roch_phi.push_back(cat_mu.phi());
    muon_roch_m.push_back(cat_mu.mass());
    muon_roch_energy.push_back(cat_mu.energy());

    muon_isTrackerMuon.push_back(mu.isTrackerMuon());
    muon_isGlobalMuon.push_back(mu.isGlobalMuon()); 
    muon_isLooseMuon.push_back(mu.isLooseMuon()); 
    muon_isMediumMuon.push_back(mu.isMediumMuon());
    muon_isTightMuon.push_back(mu.isTightMuon());
    muon_isSoftMuon.push_back(mu.isSoftMuon()); 
    muon_mcMatched.push_back(mu.mcMatched()); 
    muon_isPFMuon.push_back(mu.isPFMuon());
    muon_numberOfValidHits.push_back(mu.numberOfValidHits());
    muon_numberOfValidMuonHits.push_back(mu.numberOfValidMuonHits());
    muon_numberOfMatchedStations.push_back(mu.numberOfMatchedStations());
    muon_numberOfValidPixelHits.push_back(mu.numberOfValidPixelHits());
    muon_trackerLayersWithMeasurement.push_back(mu.trackerLayersWithMeasurement());
    muon_charge.push_back(mu.charge());
    muon_x.push_back(mu.vx());
    muon_y.push_back(mu.vy());
    muon_z.push_back(mu.vz());
    muon_pt.push_back(mu.pt());
    muon_eta.push_back(mu.eta());
    muon_phi.push_back(mu.phi()); 
    muon_m.push_back(mu.mass());
    muon_energy.push_back(mu.energy()); 
    muon_relIso03.push_back(mu.relIso(0.3));
    muon_relIso04.push_back(mu.relIso(0.4));
    muon_dxy.push_back(mu.dxy());
    muon_sigdxy.push_back(mu.ipsignificance());
    muon_normchi.push_back(mu.normalizedChi2());
    muon_dz.push_back(mu.dz()); 
    muon_shiftedEup.push_back(mu.shiftedEnUp());
    muon_shiftedEdown.push_back(mu.shiftedEnDown());
    
  }
  
  for (auto jt : *jets) {
    if(jt.pt() < j_pt_min) continue;
    if(fabs(jt.eta()) > j_eta_max) continue;
    

    jets_looseJetID.push_back(jt.looseJetID());
    jets_tightJetID.push_back(jt.tightJetID());
    jets_tightLepVetoJetID.push_back(jt.tightLepVetoJetID());

    jets_partonFlavour.push_back(jt.partonFlavour());
    jets_hadronFlavour.push_back(jt.hadronFlavour());
    jets_partonPdgId.push_back(jt.partonPdgId());
    jets_vtxNtracks.push_back(jt.vtxNtracks());
    
    jets_pt.push_back(jt.pt()); 
    jets_eta.push_back(jt.eta());
    jets_phi.push_back(jt.phi());
    jets_m.push_back(jt.mass());
    jets_energy.push_back(jt.energy());
    jets_vtxMass.push_back(jt.vtxMass()); 
    jets_vtx3DVal.push_back(jt.vtx3DVal()); 
    jets_vtx3DSig.push_back(jt.vtx3DSig()); 
    jets_CSVInclV2.push_back(jt.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")); 
    jets_iCSVCvsL.push_back(jt.bDiscriminator("inclusiveCandidateSecondaryVerticesCvsL"));
    jets_CCvsLT.push_back(jt.bDiscriminator("pfCombinedCvsLJetTags"));
    jets_CCvsBT.push_back(jt.bDiscriminator("pfCombinedCvsBJetTags"));
    jets_JetProbBJet.push_back(jt.bDiscriminator("pfJetProbabilityBJetTags")); 
    jets_CMVAV2.push_back(jt.bDiscriminator("pfCombinedMVAV2BJetTags")); 
    jets_chargedEmEnergyFraction.push_back(jt.chargedEmEnergyFraction()); 
    jets_shiftedEnDown .push_back(jt.shiftedEnDown()); 
    jets_shiftedEnUp.push_back(jt.shiftedEnUp()); 
    jets_smearedRes.push_back(jt.smearedRes()); 
    jets_smearedResDown.push_back(jt.smearedResDown()); 
    jets_smearedResUp.push_back(jt.smearedResUp()); 
    jets_PileupJetId.push_back(jt.pileupJetId()); 


  }
  
  for (auto jt : *fatjets) {
    if(jt.pt() < j_pt_min) continue;
    if(fabs(jt.eta()) > j_eta_max) continue;


    fatjets_looseJetID.push_back(jt.looseJetID());
    fatjets_tightJetID.push_back(jt.tightJetID());
    fatjets_tightLepVetoJetID.push_back(jt.tightLepVetoJetID());

    fatjets_partonFlavour.push_back(jt.partonFlavour());
    fatjets_hadronFlavour.push_back(jt.hadronFlavour());
    fatjets_partonPdgId.push_back(jt.partonPdgId());
    fatjets_vtxNtracks.push_back(jt.vtxNtracks());

    fatjets_pt.push_back(jt.pt());
    fatjets_eta.push_back(jt.eta());
    fatjets_phi.push_back(jt.phi());
    fatjets_m.push_back(jt.mass());
    fatjets_energy.push_back(jt.energy());
    fatjets_vtxMass.push_back(jt.vtxMass());
    fatjets_vtx3DVal.push_back(jt.vtx3DVal());
    fatjets_vtx3DSig.push_back(jt.vtx3DSig());
    fatjets_CSVInclV2.push_back(jt.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));
    fatjets_iCSVCvsL.push_back(jt.bDiscriminator("inclusiveCandidateSecondaryVerticesCvsL"));
    fatjets_CCvsLT.push_back(jt.bDiscriminator("pfCombinedCvsLJetTags"));
    fatjets_CCvsBT.push_back(jt.bDiscriminator("pfCombinedCvsBJetTags"));
    fatjets_JetProbBJet.push_back(jt.bDiscriminator("pfJetProbabilityBJetTags"));
    fatjets_CMVAV2.push_back(jt.bDiscriminator("pfCombinedMVAV2BJetTags"));
    fatjets_chargedEmEnergyFraction.push_back(jt.chargedEmEnergyFraction());
    fatjets_shiftedEnDown .push_back(jt.shiftedEnDown());
    fatjets_shiftedEnUp.push_back(jt.shiftedEnUp());
    fatjets_smearedRes.push_back(jt.smearedRes());
    fatjets_smearedResDown.push_back(jt.smearedResDown());
    fatjets_smearedResUp.push_back(jt.smearedResUp());
    fatjets_PileupJetId.push_back(jt.pileupJetId());

    fatjets_tau1.push_back(jt.tau1());
    fatjets_tau2.push_back(jt.tau2());
    fatjets_tau3.push_back(jt.tau3());
    fatjets_prunedmass.push_back(jt.prunedmass());
    fatjets_softdropmass.push_back(jt.softdropmass());

    fatjets_puppi_pt.push_back(jt.puppi_pt());
    fatjets_puppi_eta.push_back(jt.puppi_eta());
    fatjets_puppi_phi.push_back(jt.puppi_phi());
    fatjets_puppi_m.push_back(jt.puppi_mass());
    fatjets_puppi_tau1.push_back(jt.puppi_tau1());
    fatjets_puppi_tau2.push_back(jt.puppi_tau2());
    fatjets_puppi_tau3.push_back(jt.puppi_tau3());


  }


  edm::Handle<cat::METCollection> mets;         
  event.getByToken(metToken_, mets);

  
  double px_shift_muon_up(0.), px_shift_muon_down(0.), py_shift_muon_up(0.), py_shift_muon_down(0.), px_muon(0.), py_muon(0.), px_electron(0.), py_electron(0.) ;
  double  px_shift_electron_up(0.), px_shift_electron_down(0.), py_shift_electron_up(0.), py_shift_electron_down(0.);
  for (auto mu : *muons) {
    std::string mutrig= "SKTriggerMatching[muon]:";

    for(unsigned int i =0; i< vtrignames_tomatch_muon.size(); i++){
      for (pat::TriggerObjectStandAlone trigObj : *triggerObjects) { 
	trigObj.unpackPathNames(trigNames);
	std::vector<std::string> pathNamesAll  = trigObj.pathNames(false);
	for (unsigned h = 0, n = pathNamesAll.size(); h < n; ++h) {
	  if ( pathNamesAll[h].find(vtrignames_tomatch_muon.at(i)) == 0 ){
	    if (trigObj.hasPathName( pathNamesAll[h], true, true )){
	      // found trigger
	      if ( reco::deltaR(trigObj, mu) < 0.1){
		mutrig+= vtrignames_tomatch_muon.at(i);
	      }
	    }
	  }
	}
      }
    }
    muon_trigmatch.push_back(mutrig);
    px_muon += mu.px();
    py_muon += mu.py();
    px_shift_muon_up += mu.shiftedEnUp() *mu.px();
    px_shift_muon_down += mu.shiftedEnDown()*mu.px();
    py_shift_muon_up += mu.shiftedEnUp() *mu.py();
    py_shift_muon_down += mu.shiftedEnDown()*mu.py();
    
  }
  

  for (auto el : *electrons) {
    if(el.pt() != el.pt()) continue;
    std::string eltrig= "SKTriggerMatching:";
    for(unsigned int i =0; i< vtrignames_tomatch_electron.size(); i++){
      for (pat::TriggerObjectStandAlone trigObj : *triggerObjects) {
	trigObj.unpackPathNames(trigNames);
	std::vector<std::string> pathNamesAll  = trigObj.pathNames(false);
	for (unsigned h = 0, n = pathNamesAll.size(); h < n; ++h) {
	  if ( pathNamesAll[h].find(vtrignames_tomatch_electron.at(i)) == 0 ){
	    if (trigObj.hasPathName( pathNamesAll[h], true, true )){
	            
	      if ( reco::deltaR(trigObj, el) < 0.1){
		eltrig+= vtrignames_tomatch_electron.at(i);
	      }
	    }
	  }
	}
      }
    }
    electron_trigmatch.push_back(eltrig);
    px_electron += el.px();
    py_electron += el.py();
    px_shift_electron_up += el.shiftedEnUp() *el.px();
    px_shift_electron_down += el.shiftedEnDown()*el.px();
    py_shift_electron_up += el.shiftedEnUp()*el.py();
    py_shift_electron_down += el.shiftedEnDown()*el.py();
  }
  
  met_muonEn_Px_up =  mets->front().px() + px_muon - px_shift_muon_up; 
  met_muonEn_Px_down =  mets->front().px() + px_muon - px_shift_muon_down; 
  met_muonEn_Py_up =  mets->front().py() + py_muon - py_shift_muon_up;
  met_muonEn_Py_down =  mets->front().py() + py_muon - py_shift_muon_down;
  met_electronEn_Px_up =  mets->front().px() + px_electron - px_shift_electron_up;
  met_electronEn_Px_down =  mets->front().px() + px_electron - px_shift_electron_down;
  met_electronEn_Py_up =  mets->front().py() + py_electron - py_shift_electron_up;
  met_electronEn_Py_down =  mets->front().py() + py_electron - py_shift_electron_down;


  // save filter info
  Flag_HBHENoiseFilter=false;
  Flag_HBHENoiseIsoFilter=false;
  Flag_CSCTightHaloFilter=false;
  Flag_goodVertices=false;
  Flag_eeBadScFilter=false;
  Flag_globalTightHalo2016Filter=false;
  Flag_EcalDeadCellTriggerPrimitiveFilter=false;

  edm::Handle<edm::TriggerResults> metFilterBits;
  if (!event.getByToken(metFilterBitsPAT_, metFilterBits)){
    event.getByToken(metFilterBitsRECO_, metFilterBits);
  }

  const edm::TriggerNames &metFilterNames = event.triggerNames(*metFilterBits);
  for ( auto& hltPath : metFilterNames_ ){
    unsigned int trigIndex = metFilterNames.triggerIndex(hltPath.first);
    if ( trigIndex < metFilterBits->size() ){
      if ( metFilterBits->accept(trigIndex) ){
	TString metname = TString(hltPath.first);

	if(metname.Contains("Flag_HBHENoiseFilter")) Flag_HBHENoiseFilter=true;
	if(metname.Contains("Flag_HBHENoiseIsoFilter")) Flag_HBHENoiseIsoFilter=true;
	if(metname.Contains("Flag_CSCTightHaloFilter")) Flag_CSCTightHaloFilter = true;
	if(metname.Contains("Flag_goodVertices")) Flag_goodVertices = true;
	if(metname.Contains("Flag_eeBadScFilter")) Flag_eeBadScFilter = true;
	if(metname.Contains("Flag_EcalDeadCellTriggerPrimitiveFilter")) Flag_EcalDeadCellTriggerPrimitiveFilter = true;
	if(metname.Contains("Flag_globalTightHalo2016Filter"))Flag_globalTightHalo2016Filter = true;

      }
    }
  }


  ///// Fill GENParticle Info
  if(!event.isRealData()){
    edm::Handle<reco::GenParticleCollection> genParticles;
    event.getByToken(mcLabel_,genParticles);
    
    int counter=0;

    for( reco::GenParticleCollection::const_iterator it = genParticles->begin(); it != genParticles->end(); ++it , ++counter) {      

      if(!keepAllGen && counter > 30) continue;

      gen_eta_.push_back( it->eta() );
      gen_phi_.push_back( it->phi() );
      gen_pt_.push_back( it->pt() );
      gen_energy_.push_back( it->energy() );
      gen_pdgid_.push_back( it->pdgId() );
      gen_status_.push_back( it->status() );
      
      // https://indico.cern.ch/event/459797/contributions/1961581/attachments/1181555/1800214/mcaod-Feb15-2016.pdf
      /// https://cmssdt.cern.ch/SDT/doxygen/CMSSW_7_4_12_patch4/doc/html/d5/dd4/classreco_1_1GenParticle.html#a81efe019b5fc56363973c98a24c7fddb
      gen_isprompt_.push_back( it->statusFlags().isPrompt() );
      gen_isdecayedleptonhadron_.push_back( it->statusFlags().isDecayedLeptonHadron() );
      gen_istaudecayproduct_.push_back( it->statusFlags().isTauDecayProduct() );
      gen_isprompttaudecayproduct_.push_back( it->statusFlags().isPromptTauDecayProduct() );
      gen_isdirecthadrondecayproduct_.push_back( it->statusFlags().isDirectHadronDecayProduct() );
      gen_ishardprocess_.push_back( it->isHardProcess() );                ///  { return statusFlags_.isHardProcess(); }
      gen_fromhardprocess_.push_back( it->statusFlags().fromHardProcess() );	  //{ return statusFlags_.isDecayedLeptonHadron() && statusFlags_.fromHardProcess(); }
      gen_fromhardprocess_beforeFSR_.push_back( it->fromHardProcessBeforeFSR() ); //
      
      //gen_isdirectfromtau_.push_back( it->statusFlags().isDirectTauDecayProduct() );
      //gen_ispromptdirectfromtau_.push_back( it->statusFlags().isDirectPromptTauDecayProduct() );                                                                                                                                                                             
	
      int idx = -1;
      for( reco::GenParticleCollection::const_iterator mit = genParticles->begin(); mit != genParticles->end(); ++mit ) {
	if( it->mother()==&(*mit) ) {
	  idx = std::distance(genParticles->begin(),mit);
	  break;
	}
      }
      
      gen_motherindex_.push_back( idx);
    }
   
    
    // Fill  GenJet Info
    edm::Handle<reco::GenJetCollection> genjet;
    event.getByToken(genjet_, genjet);
    
    auto_ptr<vector<cat::GenJet> >  out(new vector<cat::GenJet>());
    
    for (const reco::GenJet & aGenJet : *genjet) {
      if ( aGenJet.pt() < 20. || std::abs(aGenJet.eta()) > 2.5 ) continue;
      
      cat::GenJet aCatGenJet(aGenJet);
      cat::MCParticle matched;
      reco::Jet::Constituents jc = aGenJet.getJetConstituents();
      //if B-Hadron matched, always assign B-Hadron
      for ( reco::Jet::Constituents::const_iterator itr = jc.begin(); itr != jc.end(); ++itr ){
	if (itr->isAvailable()){
	  const reco::Candidate* mcpart = dynamic_cast<const reco::Candidate*>(itr->get());
	  const reco::Candidate* lastB = lastBHadron(*mcpart);
	  if (lastB){
	    matched = cat::MCParticle(*lastB);
	    break;
	  }
	}
      }
      if (std::abs(matched.pdgId()) != 5){
	//if only no B-Hadron matched, assign C-Hadron
	for ( reco::Jet::Constituents::const_iterator itr = jc.begin(); itr != jc.end(); ++itr ){
	  if (itr->isAvailable()){
	    const reco::Candidate* mcpart = dynamic_cast<const reco::Candidate*>(itr->get());
	    const reco::Candidate* lastC = lastCHadron(*mcpart);
	    if (lastC){
	      matched = cat::MCParticle(*lastC);
	      break;
	    }
	  }
	}
      }
      
      GenJet_eta_.push_back(aGenJet.eta());
      GenJet_pt_.push_back(aGenJet.pt());
      GenJet_phi_.push_back(aGenJet.phi());
      GenJet_energy_.push_back(aGenJet.energy());
      GenJet_emf_.push_back(aGenJet.emEnergy()/aGenJet.energy());
      GenJet_hadf_.push_back(aGenJet.hadEnergy()/aGenJet.energy());
      GenJet_pdgid_.push_back(matched.pdgId());
    }
  }
  /// Fill EventInfo
    

  CatVersion_  = "v8-0-2";

  IsData_      = event.isRealData();
  runNumber_   = event.run();
  lumiNumber_  = event.luminosityBlock();
  eventNumber_ = event.id().event();


  nFailure += boolCSet_.load(event);
  nFailure += intCSet_.load(event);
  nFailure += doubleCSet_.load(event);
  nFailure += floatCSet_.load(event);
  //nFailure += stringCSet_.load(event);
  nFailure += vboolCSet_.load(event);
  nFailure += vintCSet_.load(event);
  nFailure += vdoubleCSet_.load(event);
  nFailure += vfloatCSet_.load(event);
  nFailure += vstringCSet_.load(event);

  const size_t nCand = candTokens_.size();
  for ( size_t iCand=0; iCand < nCand; ++iCand )
  {
    edm::Handle<CandView> srcHandle;
    event.getByToken(candTokens_[iCand], srcHandle);
    if ( !srcHandle.isValid() )
    {
      ++nFailure;
      continue;
    }
  
    const int index = indices_[iCand];
    const std::vector<CandFtn>& exprs = exprs_[iCand];
    const std::vector<CandSel>& selectors = selectors_[iCand];
    std::vector<VmapToken>& vmapTokens = vmapTokens_[iCand];
    const size_t nExpr = exprs.size();
    const size_t nSels = selectors.size();
    const size_t nVmap = vmapTokens.size();
    std::vector<edm::Handle<edm::ValueMap<double> > > vmapHandles(nVmap);
    for ( size_t iVar=0; iVar<nVmap; ++iVar )
    {
      event.getByToken(vmapTokens[iVar], vmapHandles[iVar]);
    }

    for ( size_t i=0, n=srcHandle->size(); i<n; ++i )
    {
      if ( index >= 0 and int(i) != index ) continue;
      edm::Ref<CandView> candRef(srcHandle, i);

      for ( size_t j=0; j<nExpr; ++j )
      {
        const double val = exprs[j](*candRef);
        candVars_[iCand][j]->push_back(val);
      }
      for ( size_t j=0; j<nSels; ++j )
      {
        const double val = selectors[j](*candRef);
        candVars_[iCand][j+nExpr]->push_back(val);
      }
      for ( size_t j=0; j<nVmap; ++j )
      {
        double val = 0;
        if ( vmapHandles[j].isValid() ) val = (*vmapHandles[j])[candRef];
        candVars_[iCand][j+nExpr+nSels]->push_back(val);
      }
    }
  }



  /// bools
  const size_t nCand_bool = cand_boolTokens_.size();
  for ( size_t iCand_bool=0; iCand_bool < nCand_bool; ++iCand_bool )
    {
      edm::Handle<CandView> srcHandle;
      event.getByToken(cand_boolTokens_[iCand_bool], srcHandle);
      if ( !srcHandle.isValid() )
	{
	  ++nFailure;
	  continue;
	}
      
      const int index = indices_bool_[iCand_bool];
      const std::vector<CandFtn>& exprs = exprs_bool_[iCand_bool];
      const std::vector<CandSel>& selectors = selectors_bool_[iCand_bool];
      
      std::vector<Vmap_boolToken>& vmap_boolTokens = vmap_boolTokens_[iCand_bool];
      const size_t nExpr = exprs.size();
      const size_t nSels = selectors.size();
      const size_t nVmap_bool = vmap_boolTokens.size();
      std::vector<edm::Handle<edm::ValueMap<bool> > > vmap_boolHandles(nVmap_bool);
      
      for ( size_t iVar=0; iVar<nVmap_bool; ++iVar )
	{
	  event.getByToken(vmap_boolTokens[iVar], vmap_boolHandles[iVar]);
	}
      
      for ( size_t i=0, n=srcHandle->size(); i<n; ++i )	{
	if ( index >= 0 and int(i) != index ) continue;
	edm::Ref<CandView> cand_boolRef(srcHandle, i);
	
	for ( size_t j=0; j<nExpr; ++j )
	  {
	    const bool val = exprs[j](*cand_boolRef);
	    cand_boolVars_[iCand_bool][j]->push_back(val);
	  }
	
	for ( size_t j=0; j<nSels; ++j )
	  {
	    const bool val = selectors[j](*cand_boolRef);
	    cand_boolVars_[iCand_bool][j+nExpr]->push_back(val);
	  }
	
	for ( size_t j=0; j<nVmap_bool; ++j )
	  {
	    bool val = 0;
	    if ( vmap_boolHandles[j].isValid() ) val = (*vmap_boolHandles[j])[cand_boolRef];
	    cand_boolVars_[iCand_bool][j+nExpr+nSels]->push_back(val);
	  }
      }
    }
  



  /// int                                                                                                                                                                                                                                                                 
  const size_t nCand_int = cand_intTokens_.size();
  for ( size_t iCand_int=0; iCand_int < nCand_int; ++iCand_int )
    {
      edm::Handle<CandView> srcHandle;
      event.getByToken(cand_intTokens_[iCand_int], srcHandle);
      if ( !srcHandle.isValid() )
        {
          ++nFailure;
          continue;
        }

      const int index = indices_int_[iCand_int];
      const std::vector<CandFtn>& exprs = exprs_int_[iCand_int];
      const std::vector<CandSel>& selectors = selectors_int_[iCand_int];

      std::vector<Vmap_intToken>& vmap_intTokens = vmap_intTokens_[iCand_int];
      const size_t nExpr = exprs.size();
      const size_t nSels = selectors.size();
      const size_t nVmap_int = vmap_intTokens.size();
      std::vector<edm::Handle<edm::ValueMap<int> > > vmap_intHandles(nVmap_int);

      for ( size_t iVar=0; iVar<nVmap_int; ++iVar )
        {
          event.getByToken(vmap_intTokens[iVar], vmap_intHandles[iVar]);
        }

      for ( size_t i=0, n=srcHandle->size(); i<n; ++i )
        {
          if ( index >= 0 and int(i) != index ) continue;
	  edm::Ref<CandView> cand_intRef(srcHandle, i);

          for ( size_t j=0; j<nExpr; ++j )
            {
              const int val = exprs[j](*cand_intRef);
              cand_intVars_[iCand_int][j]->push_back(val);
            }

          for ( size_t j=0; j<nSels; ++j )
            {
              const int val = selectors[j](*cand_intRef);
              cand_intVars_[iCand_int][j+nExpr]->push_back(val);
            }

          for ( size_t j=0; j<nVmap_int; ++j )
            {
              int val = 0;
              if ( vmap_intHandles[j].isValid() ) val = (*vmap_intHandles[j])[cand_intRef];
              cand_intVars_[iCand_int][j+nExpr+nSels]->push_back(val);
            }
        }
    }

  
  if ( nFailure == 0 or failureMode_ == FAILUREMODE::KEEP ) tree_->Fill();
  else if ( failureMode_ == FAILUREMODE::ERROR )
  {
    edm::LogError("GenericNtupleMakerSNU") << "Failed to get " << nFailure << " items";
    throw cms::Exception("DataError") << "Cannot get object from data";
  }
  //else if ( failureMode_ == FAILUREMODE::SKIP ); // don't fill and continue memory cleanup

  // Clear up after filling tree
  vboolCSet_.clear();
  vintCSet_.clear();
  vdoubleCSet_.clear();
  vfloatCSet_.clear();
  vstringCSet_.clear();
    
  vtrignames.clear();
  vtrigps.clear();
  muon_trigmatch.clear();
  electron_trigmatch.clear();
  gen_pt_.clear();
  gen_motherindex_.clear();
  gen_eta_.clear();
  gen_phi_.clear();
  gen_energy_.clear();
  gen_status_.clear();
  gen_pdgid_.clear();

  gen_isprompt_.clear();
  gen_isdecayedleptonhadron_.clear();
  gen_istaudecayproduct_.clear();
  gen_isprompttaudecayproduct_.clear();
  gen_isdirecthadrondecayproduct_.clear();

  gen_ishardprocess_.clear();
  gen_fromhardprocess_.clear();
  gen_fromhardprocess_beforeFSR_.clear();

  ScaleWeight_.clear();
  PDFWeight_.clear();

  GenJet_pt_.clear();
  GenJet_eta_.clear();
  GenJet_phi_.clear();
  GenJet_energy_.clear();
  GenJet_emf_.clear();
  GenJet_hadf_.clear();
  GenJet_pdgid_.clear();
  

  /// Jets                                                                                                                                                                                                                       
  jets_looseJetID.clear();
  jets_tightJetID.clear();
  jets_tightLepVetoJetID.clear();

  jets_partonFlavour.clear();
  jets_hadronFlavour.clear();
  jets_partonPdgId.clear();
  jets_vtxNtracks.clear();

  jets_pt.clear();
  jets_eta.clear();
  jets_phi.clear();
  jets_m.clear();
  jets_energy.clear();
  jets_vtxMass.clear();
  jets_vtx3DVal.clear();
  jets_vtx3DSig.clear();
  jets_CSVInclV2.clear();
  jets_iCSVCvsL.clear();
  jets_CCvsLT.clear();
  jets_CCvsBT.clear();
  jets_JetProbBJet.clear();
  jets_CMVAV2.clear();
  jets_chargedEmEnergyFraction.clear();
  jets_shiftedEnDown .clear();
  jets_shiftedEnUp.clear();
  jets_smearedRes.clear();
  jets_smearedResDown.clear();
  jets_smearedResUp.clear();
  jets_PileupJetId.clear();


  fatjets_looseJetID.clear();
  fatjets_tightJetID.clear();
  fatjets_tightLepVetoJetID.clear();

  fatjets_partonFlavour.clear();
  fatjets_hadronFlavour.clear();
  fatjets_partonPdgId.clear();
  fatjets_vtxNtracks.clear();

  fatjets_pt.clear();
  fatjets_eta.clear();
  fatjets_phi.clear();
  fatjets_m.clear();
  fatjets_energy.clear();
  fatjets_vtxMass.clear();
  fatjets_vtx3DVal.clear();
  fatjets_vtx3DSig.clear();
  fatjets_CSVInclV2.clear();
  fatjets_iCSVCvsL.clear();
  fatjets_CCvsLT.clear();
  fatjets_CCvsBT.clear();
  fatjets_JetProbBJet.clear();
  fatjets_CMVAV2.clear();
  fatjets_chargedEmEnergyFraction.clear();
  fatjets_shiftedEnDown .clear();
  fatjets_shiftedEnUp.clear();
  fatjets_smearedRes.clear();
  fatjets_smearedResDown.clear();
  fatjets_smearedResUp.clear();
  fatjets_PileupJetId.clear();

  fatjets_tau1.clear();
  fatjets_tau2.clear();
  fatjets_tau3.clear();
  fatjets_prunedmass.clear();
  fatjets_softdropmass.clear();

  fatjets_puppi_pt.clear();
  fatjets_puppi_eta.clear();
  fatjets_puppi_phi.clear();
  fatjets_puppi_m.clear();
  fatjets_puppi_tau1.clear();
  fatjets_puppi_tau2.clear();
  fatjets_puppi_tau3.clear();

  muon_isTrackerMuon.clear();
  muon_isGlobalMuon.clear();
  muon_isLooseMuon.clear();
  muon_isMediumMuon.clear();
  muon_isTightMuon.clear();
  muon_isSoftMuon.clear();
  muon_mcMatched.clear();
  muon_isPFMuon.clear();
  muon_numberOfValidHits.clear();
  muon_numberOfValidMuonHits.clear();
  muon_numberOfMatchedStations.clear();
  muon_numberOfValidPixelHits.clear();
  muon_trackerLayersWithMeasurement.clear();
  muon_charge.clear();
  muon_x.clear();
  muon_y.clear();
  muon_z.clear();

  muon_roch_pt.clear();
  muon_roch_eta.clear();
  muon_roch_phi.clear();
  muon_roch_m.clear();
  muon_roch_energy.clear();
  muon_pt.clear();
  muon_eta.clear();
  muon_phi.clear();
  muon_m.clear();
  muon_energy.clear();
  muon_relIso03.clear();
  muon_relIso04.clear();
  muon_dxy.clear();
  muon_sigdxy.clear();
  muon_normchi.clear();
  muon_dz.clear();
  muon_shiftedEup.clear();
  muon_shiftedEdown.clear();
 
  electrons_electronID_loose.clear();
  electrons_electronID_medium.clear();
  electrons_electronID_tight.clear();
  electrons_electronID_veto.clear();
  electrons_electronID_mva_medium.clear();
  electrons_electronID_mva_tight.clear();
  electrons_electronID_mva_trig_medium.clear();
  electrons_electronID_mva_trig_tight.clear();
  electrons_electronID_heep.clear();
  electrons_mcMatched.clear();
  electrons_isPF.clear();
  electrons_passConversionVeto.clear();
  electrons_isTrigMVAValid.clear();
  electrons_snuID.clear();
  electrons_charge.clear();
  electrons_x.clear();
  electrons_y.clear();
  electrons_z.clear();
  electrons_pt.clear();
  electrons_eta.clear();
  electrons_phi.clear();
  electrons_m.clear();
  electrons_energy.clear();
  electrons_relIso03.clear();
  electrons_relIso04.clear();
  electrons_shiftedEnDown.clear();
  electrons_shiftedEnUp.clear();
  electrons_absIso03.clear();
  electrons_absIso04.clear();
  electrons_chIso03.clear();
  electrons_nhIso03.clear();
  electrons_phIso03.clear();
  electrons_puChIso03.clear();
  electrons_chIso04.clear();
  electrons_nhIso04.clear();
  electrons_phIso04.clear();
  electrons_puChIso04.clear();
  electrons_scEta.clear();
  electrons_dxy.clear();
  electrons_sigdxy.clear();
  electrons_dz.clear();
  electrons_isGsfCtfScPixChargeConsistent.clear();

  //delete rmcor;


  for ( size_t iCand=0; iCand<nCand; ++iCand )
  {
    const size_t nVar = candVars_[iCand].size();
    for ( size_t iVar=0; iVar<nVar; ++iVar )
    {
      candVars_[iCand][iVar]->clear();
    }
  }

  for ( size_t iCand_bool=0; iCand_bool<nCand_bool; ++iCand_bool )
    {
      const size_t nVar = cand_boolVars_[iCand_bool].size();
      for ( size_t iVar=0; iVar<nVar; ++iVar )
	{
	  cand_boolVars_[iCand_bool][iVar]->clear();
	}
    }
  
  for ( size_t iCand_int=0; iCand_int<nCand_int; ++iCand_int )
    {
      const size_t nVar = cand_intVars_[iCand_int].size();
      for ( size_t iVar=0; iVar<nVar; ++iVar )
        {
          cand_intVars_[iCand_int][iVar]->clear();
        }
    }

}

void GenericNtupleMakerSNU::endLuminosityBlock(const edm::LuminosityBlock& lumi, const edm::EventSetup& eventSetup)
{
  for ( size_t i=0, n=eventCounterTokens_.size(); i<n; ++i )
  {
    edm::Handle<edm::MergeableCounter> eventCounterHandle;
    if ( lumi.getByToken(eventCounterTokens_[i], eventCounterHandle) )
    {
      hNEvent_->Fill(i, double(eventCounterHandle->value));
    }
  }
}
 
 

std::vector<const reco::Candidate *> GenericNtupleMakerSNU::getAncestors(const reco::Candidate &c)
{
  vector<const reco::Candidate *> moms;
  if( c.numberOfMothers() == 1 ) {
    const reco::Candidate * dau = &c;
    const reco::Candidate * mom = c.mother();
    while ( dau->numberOfMothers() == 1) {
      moms.push_back( dau );
      dau = mom ;
      mom = dau->mother();
    }
  }
  return moms;
}

bool GenericNtupleMakerSNU::hasBottom(const reco::Candidate &c)
{
  int code1;
  int code2;
  bool tmpHasBottom = false;
  code1 = (int)( ( abs(c.pdgId() ) / 100)%10 );
  code2 = (int)( ( abs(c.pdgId() ) /1000)%10 );
  if ( code1 == 5 || code2 == 5) tmpHasBottom = true;
  return tmpHasBottom;
}

bool GenericNtupleMakerSNU::hasCharm(const reco::Candidate &c)
{
  int code1;
  int code2;
  bool tmpHasCharm = false;
  code1 = (int)( ( abs(c.pdgId() ) / 100)%10 );
  code2 = (int)( ( abs(c.pdgId() ) /1000)%10 );
  if ( code1 == 4 || code2 == 4) tmpHasCharm = true;
  return tmpHasCharm;
}


const reco::Candidate* GenericNtupleMakerSNU::lastBHadron(const reco::Candidate & c)
{
  const reco::Candidate * out = 0;
  vector<const reco::Candidate *> allParents = getAncestors( c );
  for( vector<const reco::Candidate *>::const_iterator aParent = allParents.begin();
       aParent != allParents.end();
       aParent ++ )
    {
      if( hasBottom(**aParent) ) out = *aParent;
    }
  return out;
}

const reco::Candidate* GenericNtupleMakerSNU::lastCHadron(const reco::Candidate & c)
{
  const reco::Candidate * out = 0;
  vector<const reco::Candidate *> allParents = getAncestors( c );
  for( vector<const reco::Candidate *>::const_iterator aParent = allParents.begin();
       aParent != allParents.end();
       aParent ++ )
    {
      if( hasCharm(**aParent) ) out = *aParent;
    }
  
  return out;
}



#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(GenericNtupleMakerSNU);


//  LocalWords:  isPFMuon
