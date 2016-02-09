B1;95;0c#include "FWCore/Framework/interface/Frameworkfwd.h"
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

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"


#include "CommonTools/Utils/interface/StringObjectFunction.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

#include "CATTools/DataFormats/interface/MET.h"
#include "CATTools/DataFormats/interface/Muon.h"
#include "CATTools/DataFormats/interface/Electron.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"




#include "TTree.h"
#include "TH1F.h"

#include <boost/regex.hpp>


#include <memory>
#include <vector>
#include <string>

using namespace std;
using namespace edm;


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

  void analyze(const edm::Event& event, const edm::EventSetup& eventSetup) override;
  void endLuminosityBlock(const edm::LuminosityBlock& lumi, const edm::EventSetup& eventSetup) override;

private:
  edm::EDGetTokenT<reco::GenParticleCollection> mcLabel_;

  
  vector<cat::Muon> selectMuons(const edm::View<cat::Muon>* muons );
  vector<cat::Electron> selectElecs(const edm::View<cat::Electron>* elecs );
  
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
  edm::EDGetTokenT<pat::TriggerObjectStandAloneCollection> triggerObjects_;
  edm::EDGetTokenT<pat::PackedTriggerPrescales> triggerPrescales_;
  edm::EDGetTokenT<edm::TriggerResults> metFilterBitsPAT_;
  edm::EDGetTokenT<edm::TriggerResults> metFilterBitsRECO_;
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

  
  bool runFullTrig;
  
  TH1F* hNEvent_;

  TTree* tree_;
  int runNumber_, lumiNumber_, eventNumber_;
  double vertex_X, vertex_Y, vertex_Z;
  bool IsData_;


  bool Flag_HBHENoiseFilter, Flag_CSCTightHaloFilter, Flag_goodVertices, Flag_eeBadScFilter, Flag_EcalDeadCellTriggerPrimitiveFilter;

  double met_muonEn_Px_up, met_muonEn_Px_down, met_muonEn_Py_up, met_muonEn_Py_down;
  double met_electronEn_Px_up, met_electronEn_Px_down, met_electronEn_Py_up, met_electronEn_Py_down;
  double met_jetEn_Px_up, met_jetEn_Px_down, met_jetEn_Py_up, met_jetEn_Py_down;
  double met_jetRes_Px_up, met_jetRes_Px_down, met_jetRes_Py_up, met_jetRes_Py_down;  
  double met_unclusteredEn_Px_up, met_unclusteredEn_Px_down, met_unclusteredEn_Py_up, met_unclusteredEn_Py_down;
  double met_unclusteredEn_SumEt_down, met_unclusteredEn_SumEt_up, met_jetEn_SumEt_up, met_jetEn_SumEt_down, met_jetRes_SumEt_up, met_jetRes_SumEt_down;
  double met_unclusteredEn_Phi_up, met_unclusteredEn_Phi_down; 
  vector<std::string> vtrignames;
  vector<std::string> muon_trigmatch;
  vector<std::string> electron_trigmatch;
  vector<int> vtrigps;
  vector<int> gen_pdgid_;
  vector<int> gen_status_;
  vector<int> gen_motherindex_;
  vector<float> gen_energy_;
  vector<float> gen_eta_;
  vector<float> gen_phi_;
  vector<float> gen_pt_;

  edm::EDGetTokenT<cat::METCollection>      metToken_;
  edm::EDGetTokenT<edm::View<cat::Muon> >     muonToken_;
  edm::EDGetTokenT<edm::View<cat::Electron> > elecToken_;
  

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

GenericNtupleMakerSNU::GenericNtupleMakerSNU(const edm::ParameterSet& pset)
{
  std::string failureMode = pset.getUntrackedParameter<std::string>("failureMode", "keep");
  std::transform(failureMode.begin(), failureMode.end(), failureMode.begin(), ::tolower);
  if ( failureMode == "keep" ) failureMode_ = FAILUREMODE::KEEP;
  else if ( failureMode == "skip" ) failureMode_ = FAILUREMODE::SKIP;
  else if ( failureMode == "error" ) failureMode_ = FAILUREMODE::ERROR;
  else throw cms::Exception("ConfigError") << "select one from \"keep\", \"skip\", \"error\"\n";

  //  triggers_      = consumes<vector<pair<string, int> > >(pset.getParameter<edm::InputTag>("trigLabel"));
  mcLabel_   = consumes<reco::GenParticleCollection>(pset.getParameter<edm::InputTag>("genLabel"));
  triggerBits_ = consumes<edm::TriggerResults>(pset.getParameter<edm::InputTag>("triggerBits"));
  triggerObjects_ = consumes<pat::TriggerObjectStandAloneCollection>(pset.getParameter<edm::InputTag>("triggerObjects"));
  triggerPrescales_ =consumes<pat::PackedTriggerPrescales>(pset.getParameter<edm::InputTag>("triggerPrescales"));
  metToken_  = consumes<cat::METCollection>(pset.getParameter<edm::InputTag>("met"));
  muonToken_ = consumes<edm::View<cat::Muon> >(pset.getParameter<edm::InputTag>("muons"));
  elecToken_ = consumes<edm::View<cat::Electron> >(pset.getParameter<edm::InputTag>("electrons"));
  metFilterBitsPAT_ = consumes<edm::TriggerResults>(pset.getParameter<edm::InputTag>("metFilterBitsPAT"));
  metFilterBitsRECO_ = consumes<edm::TriggerResults>(pset.getParameter<edm::InputTag>("metFilterBitsRECO"));
  vtxToken_  = consumes<reco::VertexCollection >(pset.getParameter<edm::InputTag>("vertices"));

  
  runFullTrig = pset.getParameter<bool>("runFullTrig");
  
  if(runFullTrig) cout << "Running fulltrigger" << endl;
  else cout << "Not running full trigger" << endl;
  //// Test MET

  // Output histograms and tree
  edm::Service<TFileService> fs;
  tree_ = fs->make<TTree>("event", "event");

  
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
  tree_->Branch("met_unclusteredEn_Px_up", &met_unclusteredEn_Px_up, "met_unclusteredEn_Px_up/D");
  tree_->Branch("met_unclusteredEn_Py_up", &met_unclusteredEn_Py_up, "met_unclusteredEn_Py_up/D");
  tree_->Branch("met_unclusteredEn_Px_down", &met_unclusteredEn_Px_down, "met_unclusteredEn_Px_down/D");
  tree_->Branch("met_unclusteredEn_Py_down", &met_unclusteredEn_Py_down, "met_unclusteredEn_Py_down/D");
  tree_->Branch("met_unclusteredEn_SumEt_down", &met_unclusteredEn_SumEt_down, "met_unclusteredEn_SumEt_down/D");
  tree_->Branch("met_unclusteredEn_SumEt_up", &met_unclusteredEn_SumEt_up, "met_unclusteredEn_SumEt_up/D");
  tree_->Branch("met_jetEn_Px_up", &met_jetEn_Px_up, "met_jetEn_Px_up/D");
  tree_->Branch("met_jetEn_Py_up", &met_jetEn_Py_up, "met_jetEn_Py_up/D");
  tree_->Branch("met_jetEn_Px_down", &met_jetEn_Px_down, "met_jetEn_Px_down/D");
  tree_->Branch("met_jetEn_Py_down", &met_jetEn_Py_down, "met_jetEn_Py_down/D");
  tree_->Branch("met_jetEn_SumEt_up", &met_jetEn_SumEt_up, "met_jetEn_SumEt_up/D");
  tree_->Branch("met_jetEn_SumEt_down", &met_jetEn_SumEt_down, "met_jetEn_SumEt_down/D");
  tree_->Branch("met_jetRes_Px_up", &met_jetRes_Px_up, "met_jetRes_Px_up/D");
  tree_->Branch("met_jetRes_Py_up", &met_jetRes_Py_up, "met_jetRes_Py_up/D");
  tree_->Branch("met_jetRes_Px_down", &met_jetRes_Px_down, "met_jetRes_Px_down/D");
  tree_->Branch("met_jetRes_Py_down", &met_jetRes_Py_down, "met_jetRes_Py_down/D");
  tree_->Branch("met_jetRes_SumEt_up", &met_jetRes_SumEt_up, "met_jetRes_SumEt_up/D");
  tree_->Branch("met_jetRes_SumEt_down", &met_jetRes_SumEt_down, "met_jetRes_SumEt_down/D");

  tree_->Branch("IsData", &IsData_ , "IsData/O");
  tree_->Branch("HBHENoiseFilter", &Flag_HBHENoiseFilter , "HBHENoiseFilter/O");
  tree_->Branch("CSCTightHaloFilter", &Flag_CSCTightHaloFilter, "CSCTightHaloFilter/O");
  tree_->Branch("goodVertices", &Flag_goodVertices,"goodVertices/O");
  tree_->Branch("eeBadScFilter", &Flag_eeBadScFilter,"eeBadScFilter/O");
  tree_->Branch("EcalDeadCellTriggerPrimitiveFilter", &Flag_EcalDeadCellTriggerPrimitiveFilter,"EcalDeadCellTriggerPrimitiveFilter/O");

  
  tree_->Branch("muon_trigmatch", &muon_trigmatch);
  tree_->Branch("electron_trigmatch", &electron_trigmatch);


  tree_->Branch("vtrignames", &vtrignames);
  tree_->Branch("vtrigps", &vtrigps);

  tree_->Branch("gen_pt", &gen_pt_);
  tree_->Branch("gen_eta", &gen_eta_);
  tree_->Branch("gen_phi", &gen_phi_);
  tree_->Branch("gen_energy", &gen_energy_);
  tree_->Branch("gen_status", &gen_status_);
  tree_->Branch("gen_pdgid", &gen_pdgid_);
  tree_->Branch("gen_motherindex", &gen_motherindex_);

  

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
  

  edm::Handle<reco::VertexCollection> vertices;
  event.getByToken(vtxToken_, vertices);
  
  for (auto &vtx : *vertices){
    vertex_X = vtx.x(); 
    vertex_Y = vtx.y(); 
    vertex_Z = vtx.z();     
  }
  edm::Handle<edm::TriggerResults> triggerBits;
  edm::Handle<pat::TriggerObjectStandAloneCollection> triggerObjects;
  edm::Handle<pat::PackedTriggerPrescales> triggerPrescales;

  event.getByToken(triggerBits_, triggerBits);
  event.getByToken(triggerObjects_, triggerObjects);
  event.getByToken(triggerPrescales_, triggerPrescales);

  const edm::TriggerNames &trigNames = event.triggerNames(*triggerBits);  


  for( unsigned int i=0; i<trigNames.size(); ++i ){
    TString tname = TString(trigNames.triggerName(i));
    if (trigNames.triggerName(i).find("HLT_Ele") == 0 
	|| trigNames.triggerName(i).find("HLT_DoubleEle") == 0 
	|| trigNames.triggerName(i).find("HLT_IsoMu") == 0 
	|| trigNames.triggerName(i).find("HLT_TkMu") == 0 
	|| trigNames.triggerName(i).find("HLT_Mu") == 0 ){
      if(!(tname.Contains("Jpsi") 
	   || tname.Contains("NoFilters") 
	   || tname.Contains("Upsilon")
	   || tname.Contains("7p5")
	   || tname.Contains("Save")
	   || tname.Contains("dEta18") 
	   || tname.Contains("Photon"))) {
	if(runFullTrig){
	  vtrignames.push_back(trigNames.triggerName(i));
          if(triggerBits->accept(i)){
            int psValue = int(triggerBits->accept(i)) * triggerPrescales->getPrescaleForIndex(i);
            vtrigps.push_back(psValue);
          }
	  else  vtrigps.push_back(0);
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
  vtrignames_tomatch_muon.push_back("HLT_Mu17_Mu8_DZ_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu17_TkMu8_DZ_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v");
  vtrignames_tomatch_muon.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v");
  vtrignames_tomatch_muon.push_back("HLT_Ele12_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v");
  vtrignames_tomatch_muon.push_back("HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL_v");
  std::vector<string> vtrignames_tomatch_electron;
  vtrignames_tomatch_electron.push_back("HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL_v");
  vtrignames_tomatch_electron.push_back("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
  vtrignames_tomatch_electron.push_back("HLT_Ele12_CaloIdL_TrackIdL_IsoVL_v");
  vtrignames_tomatch_electron.push_back("HLT_Ele17_CaloIdL_TrackIdL_IsoVL_v");
  vtrignames_tomatch_electron.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
  vtrignames_tomatch_electron.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_");
  vtrignames_tomatch_electron.push_back("HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v");
  vtrignames_tomatch_electron.push_back("HLT_Ele27_eta2p1_WPLoose_Gsf_TriCentralPFJet30_v");
  

  
  edm::Handle<edm::View<cat::Muon> > muons;
  event.getByToken(muonToken_, muons);
  edm::Handle<edm::View<cat::Electron> > electrons;
  event.getByToken(elecToken_, electrons);
  
  edm::Handle<cat::METCollection> mets;         
  event.getByToken(metToken_, mets);

  met_unclusteredEn_Px_up  =  mets->front().unclusteredEnPx(1);  met_unclusteredEn_Px_down  =  mets->front().unclusteredEnPx(-1);
  met_unclusteredEn_Py_up  =  mets->front().unclusteredEnPy(1);  met_unclusteredEn_Py_down  =  mets->front().unclusteredEnPy(-1);
  met_unclusteredEn_SumEt_up  =  mets->front().unclusteredEnSumEt(1);  met_unclusteredEn_SumEt_down  =  mets->front().unclusteredEnSumEt(-1);
  met_unclusteredEn_Phi_up  =  mets->front().unclusteredEnPhi(1);  met_unclusteredEn_Phi_down  =  mets->front().unclusteredEnPhi(-1);

  met_jetEn_Px_up  =  mets->front().JetEnPx(1);  met_jetEn_Px_down  =  mets->front().JetEnPx(-1); 
  met_jetEn_Py_up  =  mets->front().JetEnPy(1);  met_jetEn_Py_down  =  mets->front().JetEnPy(-1); 
  met_jetEn_SumEt_up  =  mets->front().JetEnSumEt(1);  met_jetEn_SumEt_down  =  mets->front().JetEnSumEt(-1);
  
  met_jetRes_Px_up  =  mets->front().JetEnPx(1);  met_jetRes_Px_down  =  mets->front().JetEnPx(-1);
  met_jetRes_Py_up  =  mets->front().JetEnPy(1);  met_jetRes_Py_down  =  mets->front().JetEnPy(-1);
  met_jetRes_SumEt_up  =  mets->front().JetResSumEt(1);  met_jetRes_SumEt_down  =  mets->front().JetResSumEt(-1);

  float px_shift_muon_up(0.), px_shift_muon_down(0.), py_shift_muon_up(0.), py_shift_muon_down(0.), px_muon(0.), py_muon(0.), px_electron(0.), py_electron(0.) ;
  float  px_shift_electron_up(0.), px_shift_electron_down(0.), py_shift_electron_up(0.), py_shift_electron_down(0.);
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
  Flag_CSCTightHaloFilter=false;
  Flag_goodVertices=false;
  Flag_eeBadScFilter=false;
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
	if(metname.Contains("Flag_CSCTightHaloFilter")) Flag_CSCTightHaloFilter = true;
	if(metname.Contains("Flag_goodVertices")) Flag_goodVertices = true;
	if(metname.Contains("Flag_eeBadScFilter")) Flag_eeBadScFilter = true;
	if(metname.Contains("Flag_EcalDeadCellTriggerPrimitiveFilter")) Flag_EcalDeadCellTriggerPrimitiveFilter = true;
      }
    }
  }
  
  if(!event.isRealData()){
    edm::Handle<reco::GenParticleCollection> genParticles;
    event.getByToken(mcLabel_,genParticles);
    
    int counter=0;
    for( reco::GenParticleCollection::const_iterator it = genParticles->begin(); it != genParticles->end(); ++it , ++counter) {
      
      if(counter > 30) continue;
      gen_eta_.push_back( it->eta() );
      gen_phi_.push_back( it->phi() );
      gen_pt_.push_back( it->pt() );
      gen_energy_.push_back( it->energy() );
      gen_pdgid_.push_back( it->pdgId() );
      //gen_vx_.push_back( it->vx() );
      //vy->push_back( it->vy() );
      //vz->push_back( it->vz() );
      gen_status_.push_back( it->status() );
      
      int idx = -1;
      for( reco::GenParticleCollection::const_iterator mit = genParticles->begin(); mit != genParticles->end(); ++mit ) {
	if( it->mother()==&(*mit) ) {
	  idx = std::distance(genParticles->begin(),mit);
	  break;
	}
      }
      gen_motherindex_.push_back( idx );
    }
  }
    
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

      for ( size_t i=0, n=srcHandle->size(); i<n; ++i )
	{
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

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(GenericNtupleMakerSNU);

