#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"

#include "CATTools/DataFormats/interface/Muon.h"
#include "CATTools/DataFormats/interface/Electron.h"
#include "CATTools/DataFormats/interface/Jet.h"
#include "CATTools/DataFormats/interface/MET.h"
#include "CATTools/DataFormats/interface/GenTop.h"

#include "CATTools/CommonTools/interface/TTbarModeDefs.h"
#include "CATTools/CommonTools/interface/ScaleFactorEvaluator.h"
#include "CATTools/CatAnalyzer/interface/BTagScaleFactorEvaluators.h"

#include "CATTools/CommonTools/interface/AnalysisHelper.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "TTree.h"

using namespace std;
using namespace cat;

template<class T>
struct bigger_second
: std::binary_function<T,T,bool>
{
   inline bool operator()(const T& lhs, const T& rhs)
   {
      return lhs.second > rhs.second;
   }
};
typedef std::pair<int,float> data_t;

class TtbarBbbarDiLeptonAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit TtbarBbbarDiLeptonAnalyzer(const edm::ParameterSet&);
  ~TtbarBbbarDiLeptonAnalyzer();

  enum sys_e {sys_nom,  sys_jes_u, sys_jes_d, sys_jer_u, sys_jer_d,
//    sys_mu_u, sys_mu_d, sys_el_u, sys_el_d,
//    sys_mueff_u, sys_mueff_d, sys_eleff_u, sys_eleff_d,
//    sys_btag_u, sys_btag_d,
    nsys_e
  };


private:
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;

  void selectMuons(const cat::MuonCollection& muons, LeptonCollection& selmuons) const;
  void selectElecs(const cat::ElectronCollection& elecs, LeptonCollection& selelecs) const;
  //  cat::JetCollection selectJets(const cat::JetCollection& jets, const LeptonCollection& recolep) const;
  cat::JetCollection selectJets(const cat::JetCollection& jets, const LeptonCollection& recolep, TtbarBbbarDiLeptonAnalyzer::sys_e sys);
  cat::JetCollection selectBJets(const cat::JetCollection& jets, double workingpoint) const;
  const reco::Candidate* getLast(const reco::Candidate* p) const;
  const bool isLastP( const reco::GenParticle& p) const;
  float MuonSF(float pt, float eta);
  void book(TTree* tree);

  void resetBr();
  void resetBrGEN();
  void resetBrJets();

  ScaleFactorEvaluator muonSF_, elecSF_;

  edm::EDGetTokenT<int> recoFiltersToken_, nGoodVertexToken_, lumiSelectionToken_;
  edm::EDGetTokenT<float> genweightToken_, puweightToken_, puweightUpToken_, puweightDownToken_, genweightQToken_;
  edm::EDGetTokenT<int> trigTokenMUEL_, trigTokenMUMU_, trigTokenELEL_;

  edm::EDGetTokenT<cat::MuonCollection>     muonToken_;
  edm::EDGetTokenT<cat::ElectronCollection> elecToken_;
  edm::EDGetTokenT<cat::JetCollection>      jetToken_;
  edm::EDGetTokenT<cat::METCollection>      metToken_;
  edm::EDGetTokenT<reco::VertexCollection>   vtxToken_;
  edm::EDGetTokenT<int>          partonTop_channel_;
  edm::EDGetTokenT<vector<int> > partonTop_modes_;
  edm::EDGetTokenT<vector<float> > pdfWeightsToken_;
  edm::EDGetTokenT<reco::GenJetCollection> GenJetsToken_;
  edm::EDGetTokenT<reco::GenParticleCollection> GenParticlesToken_;

  edm::EDGetTokenT<cat::GenTopCollection> GenTopToken_;

  edm::EDGetTokenT<int> genTtbarIdToken_;
  edm::EDGetTokenT<int> genTtbarIdToken30_;
  edm::EDGetTokenT<int> genTtbarIdToken40_;

  edm::EDGetTokenT<reco::GenParticleCollection> partonTop_genParticles_;

  TTree * ttree_, * ttree2_;
  TTree * ttree3_, * ttree4_,* ttree5_,* ttree6_;
  int b_nvertex, b_step, b_channel;
  bool b_step1, b_step2, b_step3, b_step4, b_step5, b_step6, b_tri, b_filtered;
  float b_met, b_metphi;
  int b_njet30, b_nbjetL30, b_nbjetM30, b_nbjetT30;

  float b_lep1_pt, b_lep1_eta, b_lep1_phi, b_lep1_RelIso;
  float b_lep2_pt, b_lep2_eta, b_lep2_phi, b_lep2_RelIso;
  float b_ll_pt, b_ll_eta, b_ll_phi, b_ll_m;
  int   b_lep1_q, b_lep2_q;

  //for ttbb
  std::vector<float> b_jets_pt;
  std::vector<float> b_jets_eta;
  std::vector<float> b_jets_phi;
  std::vector<int>    b_jets_flavor;
  std::vector<float> b_jets_bDiscriminatorCSV;
  std::vector<int>    b_csvd_jetid;

  //mc
  std::vector<float> b_pdfWeights;
  float b_weight, b_puweight, b_puweightUp, b_puweightDown, b_weightQ;
  int b_partonChannel, b_partonMode1, b_partonMode2;
  float b_partonlep1_pt, b_partonlep1_eta;
  float b_partonlep2_pt, b_partonlep2_eta;
  bool b_partonInPhase, b_partonInPhaseJet, b_partonInPhaseLep;

  int b_genTtbarId, b_genTtbarId30, b_genTtbarId40;
  int b_NgenJet, b_NgenJet30, b_NgenJet40;

  //mc

  float  b_lepweight;
  float  b_csvweight;
  float  b_csvweight_JES_Up;
  float  b_csvweight_JES_Down;
  float  b_csvweight_LF_Up;
  float  b_csvweight_LF_Down;
  float  b_csvweight_HF_Up;
  float  b_csvweight_HF_Down;
  float  b_csvweight_HF_Stats1_Up;
  float  b_csvweight_HF_Stats1_Down;
  float  b_csvweight_HF_Stats2_Up;
  float  b_csvweight_HF_Stats2_Down;
  float  b_csvweight_LF_Stats1_Up;
  float  b_csvweight_LF_Stats1_Down;
  float  b_csvweight_LF_Stats2_Up;
  float  b_csvweight_LF_Stats2_Down;
  float  b_csvweight_Charm_Err1_Up;
  float  b_csvweight_Charm_Err1_Down;
  float  b_csvweight_Charm_Err2_Up;
  float  b_csvweight_Charm_Err2_Down;

  float b_csvl_sf, b_csvl_sfup, b_csvl_sfdw;
  float b_csvm_sf, b_csvm_sfup, b_csvm_sfdw;
  float b_csvt_sf, b_csvt_sfup, b_csvt_sfdw;
  /////////
  float  lepton1_pt       ;//  cms.string("lepton1().Pt()"),
  float  lepton1_eta      ;//  cms.string("lepton1().Eta()"),
  float  lepton1_phi      ;//  cms.string("lepton1().Phi()"),
  float  lepton2_pt       ;//  cms.string("lepton2().Pt()"),
  float  lepton2_eta      ;//  cms.string("lepton2().Eta()"),
  float  lepton2_phi      ;//  cms.string("lepton2().Phi()"),

  bool   allHadronic      ;//  cms.string("allHadronic"),

  bool   semiLeptonicM1     ;//  cms.string("semiLeptonic"),
  bool   semiLeptonic0     ;//  cms.string("semiLeptonic"),
  bool   semiLeptonicP1     ;//  cms.string("semiLeptonic"),

  bool   diLeptonicM1;
  bool   diLeptonic0;
  bool   diLeptonicP1;

  bool   diLeptonicMuoMuo ;//  cms.string("diLeptonicMuoMuo"),
  bool   diLeptonicMuoEle ;//  cms.string("diLeptonicMuoEle"),
  bool   diLeptonicEleEle ;//  cms.string("diLeptonicEleEle"),
  bool   diLeptonicTauMuo ;//  cms.string("diLeptonicTauMuo"),
  bool   diLeptonicTauEle ;//  cms.string("diLeptonicTauEle"),
  bool   diLeptonicTauTau ;//  cms.string("diLeptonicTauTau"),

  int    NbJets1           ;//  cms.string("NbJets(1)"),
  int    NbJets201         ;//  cms.string("NbJets20(1)"),
  int    NbJets251         ;//  cms.string("NbJets25(1)"),
  int    NbJets301         ;//  cms.string("NbJets30(1)"),
  int    NbJets401         ;//  cms.string("NbJets40(1)"),
  int    NaddbJets1        ;//  cms.string("NaddbJets(1)"),
  int    NaddbJets201      ;//  cms.string("NaddbJets20(1)"),
  int    NaddbJets401      ;//  cms.string("NaddbJets40(1)"),
  int    NcJets1           ;//  cms.string("NcJets(1)"),
  int    NcJets101         ;//  cms.string("NcJets10(1)"),
  int    NcJets151         ;//  cms.string("NcJets15(1)"),
  int    NcJets201         ;//  cms.string("NcJets20(1)"),
  int    NcJets251         ;//  cms.string("NcJets25(1)"),
  int    NcJets301         ;//  cms.string("NcJets30(1)"),
  int    NcJets401         ;//  cms.string("NcJets40(1)"),
  int    NbJets           ;//  cms.string("NbJets(0)"),
  int    NbJets20         ;//  cms.string("NbJets20(0)"),
  int    NbJets25         ;//  cms.string("NbJets25(0)"),
  int    NbJets30         ;//  cms.string("NbJets30(0)"),
  int    NbJets40         ;//  cms.string("NbJets40(0)"),
  int    NaddbJets        ;//  cms.string("NaddbJets(0)"),
  int    NaddbJets20      ;//  cms.string("NaddbJets20(0)"),
  int    NaddbJets40      ;//  cms.string("NaddbJets40(0)"),
  int    NcJets           ;//  cms.string("NcJets(0)"),
  int    NcJets10         ;//  cms.string("NcJets10(0)"),
  int    NcJets15         ;//  cms.string("NcJets15(0)"),
  int    NcJets20         ;//  cms.string("NcJets20(0)"),
  int    NcJets25         ;//  cms.string("NcJets25(0)"),
  int    NcJets30         ;//  cms.string("NcJets30(0)"),
  int    NcJets40         ;//  cms.string("NcJets40(0)"),
  int    NJets            ;//  cms.string("NJets"),
  int    NJets10          ;//  cms.string("NJets10"),
  int    NJets20          ;//  cms.string("NJets20"),
  int    NJets25          ;//  cms.string("NJets25"),
  int    NJets30          ;//  cms.string("NJets30"),
  int    NJets40          ;//  cms.string("NJets40"),
  int    NaddJets20       ;//  cms.string("NaddJets20"),
  int    NaddJets40       ;//  cms.string("NaddJets40"),
  int    NbQuarksTop      ;//  cms.string("NbQuarksTop"),
  int    NbQuarksNoTop    ;//  cms.string("NbQuarksNoTop"),
  int    NbQuarks         ;//  cms.string("NbQuarks"),
  int    NbQuarks20       ;//  cms.string("NbQuarks20"),
  int    NbQuarks40       ;//  cms.string("NbQuarks40"),
  int    NaddbQuarks20    ;//  cms.string("NaddbQuarks20"),
  int    NaddbQuarks40    ;//  cms.string("NaddbQuarks40"),
  int    NcQuarks         ;//  cms.string("NcQuarks"),

  int b_is3lep;

  const static int NCutflow = 10;
  std::vector<std::vector<int> > cutflow_;
  bool runOnMC_;
  CSVWeightEvaluator csvWeight;
};
//
// constructors and destructor
//
TtbarBbbarDiLeptonAnalyzer::TtbarBbbarDiLeptonAnalyzer(const edm::ParameterSet& iConfig)
{
  recoFiltersToken_ = consumes<int>(iConfig.getParameter<edm::InputTag>("recoFilters"));
  nGoodVertexToken_ = consumes<int>(iConfig.getParameter<edm::InputTag>("nGoodVertex"));
  lumiSelectionToken_ = consumes<int>(iConfig.getParameter<edm::InputTag>("lumiSelection"));
  genweightToken_ = consumes<float>(iConfig.getParameter<edm::InputTag>("genweight"));
  genweightQToken_ = consumes<float>(iConfig.getParameter<edm::InputTag>("genweightQ"));
  pdfWeightsToken_   = consumes<vector<float> >(iConfig.getParameter<edm::InputTag>("genweightPDF"));

  puweightToken_ = consumes<float>(iConfig.getParameter<edm::InputTag>("puweight"));
  puweightUpToken_ = consumes<float>(iConfig.getParameter<edm::InputTag>("puweightUp"));
  puweightDownToken_ = consumes<float>(iConfig.getParameter<edm::InputTag>("puweightDown"));

  trigTokenMUEL_ = consumes<int>(iConfig.getParameter<edm::InputTag>("trigMUEL"));
  trigTokenMUMU_ = consumes<int>(iConfig.getParameter<edm::InputTag>("trigMUMU"));
  trigTokenELEL_ = consumes<int>(iConfig.getParameter<edm::InputTag>("trigELEL"));

//  genTtbarIdToken_, NgenJet30Token_, genTtbarLeptonDecayToken_;
  genTtbarIdToken_  = consumes<int>(iConfig.getParameter<edm::InputTag>("genTtbarId"));
  genTtbarIdToken30_  = consumes<int>(iConfig.getParameter<edm::InputTag>("genTtbarId30"));
  genTtbarIdToken40_  = consumes<int>(iConfig.getParameter<edm::InputTag>("genTtbarId40"));
  GenJetsToken_     = consumes<reco::GenJetCollection>(iConfig.getParameter<edm::InputTag>("GenJets"));
  GenParticlesToken_     = consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("GenParticles"));

  GenTopToken_     = consumes<cat::GenTopCollection>(iConfig.getParameter<edm::InputTag>("GenTop"));
  //NgenJet30Token_           = consumes<int>(iConfig.getParameter<edm::InputTag>("NgenJet30"));
  //genTtbarLeptonDecayToken_ = consumes<int>(iConfig.getParameter<edm::InputTag>("genTtbarLeptonDecay"));

  muonToken_ = consumes<cat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons"));
  elecToken_ = consumes<cat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electrons"));
  jetToken_  = consumes<cat::JetCollection>(iConfig.getParameter<edm::InputTag>("jets"));
  metToken_  = consumes<cat::METCollection>(iConfig.getParameter<edm::InputTag>("mets"));
  vtxToken_  = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"));

  const auto elecSFSet = iConfig.getParameter<edm::ParameterSet>("elecSF");
  elecSF_.set(elecSFSet.getParameter<std::vector<double>>("pt_bins"),
              elecSFSet.getParameter<std::vector<double>>("eta_bins"),
              elecSFSet.getParameter<std::vector<double>>("values"),
              elecSFSet.getParameter<std::vector<double>>("errors"));

  partonTop_channel_ = consumes<int>(iConfig.getParameter<edm::InputTag>("partonTop_channel"));
  partonTop_modes_   = consumes<vector<int> >(iConfig.getParameter<edm::InputTag>("partonTop_modes"));
  partonTop_genParticles_   = consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("partonTop_genParticles"));

  usesResource("TFileService");
  edm::Service<TFileService> fs;
  ttree_ = fs->make<TTree>("nom", "nom");
  ttree2_ = fs->make<TTree>("nom2", "nom2");

  ttree3_ = fs->make<TTree>("nomJES_up", "nom2");
  ttree4_ = fs->make<TTree>("nomJES_dw", "nom2");
  ttree5_ = fs->make<TTree>("nomJER_up", "nom2");
  ttree6_ = fs->make<TTree>("nomJER_dw", "nom2");

  book(ttree_);
  book(ttree2_);
  book(ttree3_);
  book(ttree4_);
  book(ttree5_);
  book(ttree6_);

  for (int i = 0; i < NCutflow; i++) cutflow_.push_back({0,0,0,0});
}

void TtbarBbbarDiLeptonAnalyzer::book(TTree* tree){
  tree->Branch("nvertex", &b_nvertex, "nvertex/I");
  tree->Branch("step", &b_step, "step/I");
  tree->Branch("channel", &b_channel, "channel/I");
  tree->Branch("njet30", &b_njet30, "njet30/I");
  tree->Branch("nbjetL30", &b_nbjetL30, "nbjetL30/I");
  tree->Branch("nbjetM30", &b_nbjetM30, "nbjetM30/I");
  tree->Branch("nbjetT30", &b_nbjetT30, "nbjetT30/I");
  tree->Branch("step1", &b_step1, "step1/O");
  tree->Branch("step2", &b_step2, "step2/O");
  tree->Branch("step3", &b_step3, "step3/O");
  tree->Branch("step4", &b_step4, "step4/O");
  tree->Branch("step5", &b_step5, "step5/O");
  tree->Branch("step6", &b_step6, "step6/O");
  tree->Branch("tri", &b_tri, "tri/O");
  tree->Branch("filtered", &b_filtered, "filtered/O");
  tree->Branch("met", &b_met, "met/F");
  tree->Branch("metphi", &b_metphi, "metphi/F");

  tree->Branch("weight", &b_weight, "weight/F");
  tree->Branch("weightQ", &b_weightQ, "weightQ/F");
  tree->Branch("pdfWeihgts","std::vector<float>",&b_pdfWeights);

  tree->Branch("puweight", &b_puweight, "puweight/F");
  tree->Branch("puweightUp", &b_puweightUp, "puweightUp/F");
  tree->Branch("puweightDown", &b_puweightDown, "puweightDown/F");
  tree->Branch("lepweight", &b_lepweight, "lepweight/F");
  tree->Branch("genTtbarId", &b_genTtbarId, "genTtbarId/I");
  tree->Branch("genTtbarId30", &b_genTtbarId30, "genTtbarId30/I");
  tree->Branch("genTtbarId40", &b_genTtbarId40, "genTtbarId40/I");

  tree->Branch("NgenJet", &b_NgenJet, "NgenJet/I");
  tree->Branch("NgenJet30", &b_NgenJet30, "NgenJet30/I");
  tree->Branch("NgenJet40", &b_NgenJet40, "NgenJet40/I");


  tree->Branch("lep1_pt", &b_lep1_pt, "lep1_pt/F");
  tree->Branch("lep1_eta", &b_lep1_eta, "lep1_eta/F");
  tree->Branch("lep1_phi", &b_lep1_phi, "lep1_phi/F");
  tree->Branch("lep1_RelIso", &b_lep1_RelIso, "lep1_RelIso/F");
  tree->Branch("lep1_q", &b_lep1_q, "lep1_q/I");
  tree->Branch("lep2_pt", &b_lep2_pt, "lep2_pt/F");
  tree->Branch("lep2_eta", &b_lep2_eta, "lep2_eta/F");
  tree->Branch("lep2_phi", &b_lep2_phi, "lep2_phi/F");
  tree->Branch("lep2_RelIso", &b_lep2_RelIso, "lep2_RelIso/F");
  tree->Branch("lep2_q", &b_lep2_q, "lep2_q/I");

  tree->Branch("ll_pt", &b_ll_pt, "ll_pt/F");
  tree->Branch("ll_eta", &b_ll_eta, "ll_eta/F");
  tree->Branch("ll_phi", &b_ll_phi, "ll_phi/F");
  tree->Branch("ll_m", &b_ll_m, "ll_m/F");

  tree->Branch("parton_channel", &b_partonChannel, "parton_channel/I");
  tree->Branch("parton_mode1", &b_partonMode1, "parton_mode1/I");
  tree->Branch("partonlep1_pt", &b_partonlep1_pt, "partonlep1_pt/F");
  tree->Branch("partonlep1_eta", &b_partonlep1_eta, "partonlep1_eta/F");
  tree->Branch("partonlep2_pt", &b_partonlep2_pt, "partonlep2_pt/F");
  tree->Branch("partonlep2_eta", &b_partonlep2_eta, "partonlep2_eta/F");
  tree->Branch("parton_mode2", &b_partonMode2, "parton_mode2/I");
  tree->Branch("partonInPhase", &b_partonInPhase, "partonInPhase/O");
  tree->Branch("partonInPhaseLep", &b_partonInPhaseLep, "partonInPhaseLep/O");
  tree->Branch("partonInPhaseJet", &b_partonInPhaseJet, "partonInPhaseJet/O");

  tree->Branch("jets_pt","std::vector<float>",&b_jets_pt);
  tree->Branch("jets_eta","std::vector<float>",&b_jets_eta);
  tree->Branch("jets_phi","std::vector<float>",&b_jets_phi);
  tree->Branch("jets_flavor","std::vector<int>",&b_jets_flavor);
  tree->Branch("jets_bDiscriminatorCSV","std::vector<float>",&b_jets_bDiscriminatorCSV);
  tree->Branch("csvd_jetid","std::vector<int>",&b_csvd_jetid);

  tree->Branch("csvweight", &b_csvweight, "csvweight/F");
  tree->Branch("csvweight_JES_Up",          &b_csvweight_JES_Up,          "csvweight_JES_Up/F");
  tree->Branch("csvweight_JES_Down",        &b_csvweight_JES_Down,        "csvweight_JES_Down/F");
  tree->Branch("csvweight_LF_Up",           &b_csvweight_LF_Up,           "csvweight_LF_Up/F");
  tree->Branch("csvweight_LF_Down",         &b_csvweight_LF_Down,         "csvweight_LF_Down/F");
  tree->Branch("csvweight_HF_Up",           &b_csvweight_HF_Up,           "csvweight_HF_Up/F");
  tree->Branch("csvweight_HF_Down",         &b_csvweight_HF_Down,         "csvweight_HF_Down/F");
  tree->Branch("csvweight_HF_Stats1_Up",    &b_csvweight_HF_Stats1_Up,    "csvweight_HF_Stats1_Up/F");
  tree->Branch("csvweight_HF_Stats1_Down",  &b_csvweight_HF_Stats1_Down,  "csvweight_HF_Stats1_Down/F");
  tree->Branch("csvweight_HF_Stats2_Up",    &b_csvweight_HF_Stats2_Up,    "csvweight_HF_Stats2_Up/F");
  tree->Branch("csvweight_HF_Stats2_Down",  &b_csvweight_HF_Stats2_Down,  "csvweight_HF_Stats2_Down/F");
  tree->Branch("csvweight_LF_Stats1_Up",    &b_csvweight_LF_Stats1_Up,    "csvweight_LF_Stats1_Up/F");
  tree->Branch("csvweight_LF_Stats1_Down",  &b_csvweight_LF_Stats1_Down,  "csvweight_LF_Stats1_Down/F");
  tree->Branch("csvweight_LF_Stats2_Up",    &b_csvweight_LF_Stats2_Up,    "csvweight_LF_Stats2_Up/F");
  tree->Branch("csvweight_LF_Stats2_Down",  &b_csvweight_LF_Stats2_Down,  "csvweight_LF_Stats2_Down/F");
  tree->Branch("csvweight_Charm_Err1_Up",   &b_csvweight_Charm_Err1_Up,   "csvweight_Charm_Err1_Up/F");
  tree->Branch("csvweight_Charm_Err1_Down", &b_csvweight_Charm_Err1_Down, "csvweight_Charm_Err1_Down/F");
  tree->Branch("csvweight_Charm_Err2_Up",   &b_csvweight_Charm_Err2_Up,   "csvweight_Charm_Err2_Up/F");
  tree->Branch("csvweight_Charm_Err2_Down", &b_csvweight_Charm_Err2_Down, "csvweight_Charm_Err2_Down/F");


  tree->Branch("csvl_sf", &b_csvl_sf, "csvl_sf/F");
  tree->Branch("csvl_sfup", &b_csvl_sfup, "csvl_sfup/F");
  tree->Branch("csvl_sfdw", &b_csvl_sfdw, "csvl_sfdw/F");
  tree->Branch("csvm_sf", &b_csvm_sf, "csvm_sf/F");
  tree->Branch("csvm_sfup", &b_csvm_sfup, "csvm_sfup/F");
  tree->Branch("csvm_sfdw", &b_csvm_sfdw, "csvm_sfdw/F");
  tree->Branch("csvt_sf", &b_csvt_sf, "csvt_sf/F");
  tree->Branch("csvt_sfup", &b_csvt_sfup, "csvt_sfup/F");
  tree->Branch("csvt_sfdw", &b_csvt_sfdw, "csvt_sfdw/F");
/////////////////////////////
  tree->Branch("lepton1_pt",    &lepton1_pt   , "lepton1_pt/F");
  tree->Branch("lepton1_eta",   &lepton1_eta  , "lepton1_eta/F");
  tree->Branch("lepton1_phi",   &lepton1_phi  , "lepton1_phi/F");
  tree->Branch("lepton2_pt",    &lepton2_pt   , "lepton2_pt/F");
  tree->Branch("lepton2_eta",   &lepton2_eta  , "lepton2_eta/F");
  tree->Branch("lepton2_phi",   &lepton2_phi  , "lepton2_phi/F");

  tree->Branch("allHadronic",      &allHadronic        , "allHadronic/O");
  tree->Branch("semiLeptonicM1",     &semiLeptonicM1       , "semiLeptonicM1/O");
  tree->Branch("semiLeptonic0",     &semiLeptonic0       , "semiLeptonic0/O");
  tree->Branch("semiLeptonicP1",     &semiLeptonicP1       , "semiLeptonicP1/O");

  tree->Branch("diLeptonicM1", &diLeptonicM1   , "diLeptonicM1/O");
  tree->Branch("diLeptonic0", &diLeptonic0   , "diLeptonic0/O");
  tree->Branch("diLeptonicP1", &diLeptonicP1   , "diLeptonicP1/O");

  tree->Branch("diLeptonicMuoMuo", &diLeptonicMuoMuo   , "diLeptonicMuoMuo/O");
  tree->Branch("diLeptonicMuoEle", &diLeptonicMuoEle   , "diLeptonicMuoEle/O");
  tree->Branch("diLeptonicEleEle", &diLeptonicEleEle   , "diLeptonicEleEle/O");
  tree->Branch("diLeptonicTauMuo", &diLeptonicTauMuo   , "diLeptonicTauMuo/O");
  tree->Branch("diLeptonicTauEle", &diLeptonicTauEle   , "diLeptonicTauEle/O");
  tree->Branch("diLeptonicTauTau", &diLeptonicTauTau   , "diLeptonicTauTau/O");

  tree->Branch("NbJets1",         &NbJets1        , "NbJets1/I");
  tree->Branch("NbJets201",       &NbJets201      , "NbJets201/I");
  tree->Branch("NbJets251",       &NbJets251      , "NbJets251/I");
  tree->Branch("NbJets301",       &NbJets301      , "NbJets301/I");
  tree->Branch("NbJets401",       &NbJets401      , "NbJets401/I");
  tree->Branch("NaddbJets1",      &NaddbJets1     , "NaddbJets1/I");
  tree->Branch("NaddbJets201",    &NaddbJets201   , "NaddbJets201/I");
  tree->Branch("NaddbJets401",    &NaddbJets401   , "NaddbJets401/I");
  tree->Branch("NcJets1",         &NcJets1        , "NcJets1/I");
  tree->Branch("NcJets101",       &NcJets101      , "NcJets101/I");
  tree->Branch("NcJets151",       &NcJets151      , "NcJets151/I");
  tree->Branch("NcJets201",       &NcJets201      , "NcJets201/I");
  tree->Branch("NcJets251",       &NcJets251      , "NcJets251/I");
  tree->Branch("NcJets301",       &NcJets301      , "NcJets301/I");
  tree->Branch("NcJets401",       &NcJets401      , "NcJets401/I");
  tree->Branch("NbJets",          &NbJets         , "NbJets/I");
  tree->Branch("NbJets20",        &NbJets20       , "NbJets20/I");
  tree->Branch("NbJets25",        &NbJets25       , "NbJets25/I");
  tree->Branch("NbJets30",        &NbJets30       , "NbJets30/I");
  tree->Branch("NbJets40",        &NbJets40       , "NbJets40/I");
  tree->Branch("NaddbJets",       &NaddbJets      , "NaddbJets/I");
  tree->Branch("NaddbJets20",     &NaddbJets20    , "NaddbJets20/I");
  tree->Branch("NaddbJets40",     &NaddbJets40    , "NaddbJets40/I");
  tree->Branch("NcJets",          &NcJets         , "NcJets/I");
  tree->Branch("NcJets10",        &NcJets10       , "NcJets10/I");
  tree->Branch("NcJets15",        &NcJets15       , "NcJets15/I");
  tree->Branch("NcJets20",        &NcJets20       , "NcJets20/I");
  tree->Branch("NcJets25",        &NcJets25       , "NcJets25/I");
  tree->Branch("NcJets30",        &NcJets30       , "NcJets30/I");
  tree->Branch("NcJets40",        &NcJets40       , "NcJets40/I");
  tree->Branch("NJets",           &NJets          , "NJets/I");
  tree->Branch("NJets10",         &NJets10        , "NJets10/I");
  tree->Branch("NJets20",         &NJets20        , "NJets20/I");
  tree->Branch("NJets25",         &NJets25        , "NJets25/I");
  tree->Branch("NJets30",         &NJets30        , "NJets30/I");
  tree->Branch("NJets40",         &NJets40        , "NJets40/I");
  tree->Branch("NaddJets20",      &NaddJets20     , "NaddJets20/I");
  tree->Branch("NaddJets40",      &NaddJets40     , "NaddJets40/I");
  tree->Branch("NbQuarksTop",     &NbQuarksTop    , "NbQuarksTop/I");
  tree->Branch("NbQuarksNoTop",   &NbQuarksNoTop  , "NbQuarksNoTop/I");
  tree->Branch("NbQuarks",        &NbQuarks       , "NbQuarks/I");
  tree->Branch("NbQuarks20",      &NbQuarks20     , "NbQuarks20/I");
  tree->Branch("NbQuarks40",      &NbQuarks40     , "NbQuarks40/I");
  tree->Branch("NaddbQuarks20",   &NaddbQuarks20  , "NaddbQuarks20/I");
  tree->Branch("NaddbQuarks40",   &NaddbQuarks40  , "NaddbQuarks40/I");
  tree->Branch("NcQuarks",        &NcQuarks       , "NcQuarks/I");

  tree->Branch("is3lep", &b_is3lep, "is3lep/I");
}

TtbarBbbarDiLeptonAnalyzer::~TtbarBbbarDiLeptonAnalyzer()
{
  cout <<"cut flow         emu         ee         mumu"<< endl;
  for ( int i=0; i<NCutflow; ++i ) {
    cout <<"step "<< i << " "<< cutflow_[i][0] <<  " "<< cutflow_[i][1] << " " << cutflow_[i][2] << " " << cutflow_[i][3]<< endl;
  }
}

void TtbarBbbarDiLeptonAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  runOnMC_ = !iEvent.isRealData();
  resetBr();
  ////
  cutflow_[0][b_channel]++;


  edm::Handle<int> partonTop_channel;
  edm::Handle<reco::GenParticleCollection> genParticles;
  ////////////
  edm::Handle<cat::GenTopCollection> genTop;
  //std::cout << "!!reading genTop" << std::endl;
  if ( iEvent.getByToken(GenTopToken_, genTop)){
    //for (auto& genTop : genTops) {

    //std::cout << "reading genTop" << std::endl;
    lepton1_pt  =genTop->at(0).lepton1().Pt();
    lepton1_eta =genTop->at(0).lepton1().Eta();
    lepton1_phi =genTop->at(0).lepton1().Phi();
    lepton2_pt  =genTop->at(0).lepton2().Pt();
    lepton2_eta =genTop->at(0).lepton2().Eta();
    lepton2_phi =genTop->at(0).lepton2().Phi();

    allHadronic       =genTop->at(0).allHadronic();
    semiLeptonicM1    =genTop->at(0).semiLeptonic(-1);
    semiLeptonic0     =genTop->at(0).semiLeptonic(0);
    semiLeptonicP1    =genTop->at(0).semiLeptonic(1);

    diLeptonicM1 =genTop->at(0).diLeptonic(-1);
    diLeptonic0 =genTop->at(0).diLeptonic(0);
    diLeptonicP1 =genTop->at(0).diLeptonic(1);

    diLeptonicMuoMuo =genTop->at(0).diLeptonicMuoMuo();
    diLeptonicMuoEle =genTop->at(0).diLeptonicMuoEle();
    diLeptonicEleEle =genTop->at(0).diLeptonicEleEle();
    diLeptonicTauMuo =genTop->at(0).diLeptonicTauMuo();
    diLeptonicTauEle =genTop->at(0).diLeptonicTauEle();
    diLeptonicTauTau =genTop->at(0).diLeptonicTauTau();

    NbJets1           =genTop->at(0).NbJets(1);
    NbJets201         =genTop->at(0).NbJets20(1);
    NbJets251         =genTop->at(0).NbJets25(1);
    NbJets301         =genTop->at(0).NbJets30(1);
    NbJets401         =genTop->at(0).NbJets40(1);
    NaddbJets1        =genTop->at(0).NaddbJets(1);
    NaddbJets201      =genTop->at(0).NaddbJets20(1);
    NaddbJets401      =genTop->at(0).NaddbJets40(1);
    NcJets1           =genTop->at(0).NcJets(1);
    NcJets101         =genTop->at(0).NcJets10(1);
    NcJets151         =genTop->at(0).NcJets15(1);
    NcJets201         =genTop->at(0).NcJets20(1);
    NcJets251         =genTop->at(0).NcJets25(1);
    NcJets301         =genTop->at(0).NcJets30(1);
    NcJets401         =genTop->at(0).NcJets40(1);
    NbJets           =genTop->at(0).NbJets(0);
    NbJets20         =genTop->at(0).NbJets20(0);
    NbJets25         =genTop->at(0).NbJets25(0);
    NbJets30         =genTop->at(0).NbJets30(0);
    NbJets40         =genTop->at(0).NbJets40(0);
    NaddbJets        =genTop->at(0).NaddbJets(0);
    NaddbJets20      =genTop->at(0).NaddbJets20(0);
    NaddbJets40      =genTop->at(0).NaddbJets40(0);
    NcJets           =genTop->at(0).NcJets(0);
    NcJets10         =genTop->at(0).NcJets10(0);
    NcJets15         =genTop->at(0).NcJets15(0);
    NcJets20         =genTop->at(0).NcJets20(0);
    NcJets25         =genTop->at(0).NcJets25(0);
    NcJets30         =genTop->at(0).NcJets30(0);
    NcJets40         =genTop->at(0).NcJets40(0);
    NJets            =genTop->at(0).NJets();
    NJets10          =genTop->at(0).NJets10();
    NJets20          =genTop->at(0).NJets20();
    NJets25          =genTop->at(0).NJets25();
    NJets30          =genTop->at(0).NJets30();
    NJets40          =genTop->at(0).NJets40();
    NaddJets20       =genTop->at(0).NaddJets20();
    NaddJets40       =genTop->at(0).NaddJets40();
    NbQuarksTop      =genTop->at(0).NbQuarksTop();
    NbQuarksNoTop    =genTop->at(0).NbQuarksNoTop();
    NbQuarks         =genTop->at(0).NbQuarks();
    NbQuarks20       =genTop->at(0).NbQuarks20();
    NbQuarks40       =genTop->at(0).NbQuarks40();
    NaddbQuarks20    =genTop->at(0).NaddbQuarks20();
    NaddbQuarks40    =genTop->at(0).NaddbQuarks40();
    NcQuarks         =genTop->at(0).NcQuarks();
  }
  ////////////
  if ( iEvent.getByToken(partonTop_channel_, partonTop_channel)){
    edm::Handle<vector<int> > partonTop_modes;
    edm::Handle<reco::GenParticleCollection> partonTop_genParticles;
    iEvent.getByToken(partonTop_modes_, partonTop_modes);
    iEvent.getByToken(partonTop_genParticles_, partonTop_genParticles);
    if ( (*partonTop_modes).size() == 0 ) {
      b_partonMode1 = 0;
      b_partonMode2 = 0;
    }
    else if ( (*partonTop_modes).size() == 1 ) { b_partonMode2 = 0; }
    else{
      b_partonChannel = *partonTop_channel;
      b_partonMode1 = (*partonTop_modes)[0];
      b_partonMode2 = (*partonTop_modes)[1];
    }

    if ( !(partonTop_genParticles->empty()) ){

      // Get Top quark pairs
      const auto parton1 = &partonTop_genParticles->at(0);
      const auto parton2 = &partonTop_genParticles->at(1);
      // Get W and b quarks
      if ( parton1 and parton2 ) {
        const auto partonW1 = parton1->daughter(0);
        const auto partonB1 = parton1->daughter(1);
        const auto partonW2 = parton2->daughter(0);
        const auto partonB2 = parton2->daughter(1);

        if ( (partonB1->pt() > 30 && std::abs(partonB1->eta()) < 2.4) &&
            (partonB2->pt() > 30 && std::abs(partonB2->eta()) < 2.4))
          b_partonInPhaseJet = true;

        // Get W daughters
        if ( partonW1 and partonW2 and partonB1 and partonB2 ) {
          const auto partonW11 = partonW1->daughter(0);
          const auto partonW21 = partonW2->daughter(0);
          if ( (partonW11->pt() > 20 && std::abs(partonW11->eta()) < 2.4 && (std::abs(partonW11->pdgId()) == 11 || std::abs(partonW11->pdgId()) == 13) ) &&
              (partonW21->pt() > 20 && std::abs(partonW21->eta()) < 2.4 && (std::abs(partonW11->pdgId()) == 11 || std::abs(partonW11->pdgId()) == 13) ))
            b_partonInPhaseLep = true;

          // Fill lepton informations
          b_partonlep1_pt = partonW11->pt();
          b_partonlep1_eta = partonW11->eta();
          b_partonlep2_pt = partonW21->pt();
          b_partonlep2_eta = partonW21->eta();
        }
      }
      if (b_partonInPhaseJet && b_partonInPhaseLep) b_partonInPhase = true;
    }
  }
  else if(iEvent.getByToken(GenParticlesToken_, genParticles)){
    int is_partonInPhaseLep=0;
    for ( auto  aParticle = genParticles->begin(),end = genParticles->end(); aParticle != end; ++aParticle ) {
      const reco::GenParticle& p = *aParticle;
      if ( !(abs(p.pdgId()) > 21 && abs(p.pdgId()) << 26)  ) continue;
      bool isLast = isLastP(p);
      if(isLast != true) continue;
      unsigned int nDaughters = p.numberOfDaughters();
      for ( unsigned iDaughter=0; iDaughter<nDaughters; ++iDaughter ) {
        const reco::Candidate* daugh = p.daughter(iDaughter);
        if ( daugh->pt() > 20 && std::abs(daugh->eta()) < 2.4 && (std::abs(daugh->pdgId()) == 11 || std::abs(daugh->pdgId()) == 13) )
        {
          is_partonInPhaseLep++;
          //b_partonInPhaseLep = true;
        }
      }
    }
    if(is_partonInPhaseLep>1) b_partonInPhaseLep = true;
  }

  if (runOnMC_){
    edm::Handle<float> puweightHandle;
    iEvent.getByToken(puweightToken_, puweightHandle);
    b_puweight = *puweightHandle;
    edm::Handle<float> puweightHandleUp;
    iEvent.getByToken(puweightUpToken_, puweightHandleUp);
    b_puweightUp = *puweightHandleUp;
    edm::Handle<float> puweightHandleDown;
    iEvent.getByToken(puweightDownToken_, puweightHandleDown);
    b_puweightDown = *puweightHandleDown;

    edm::Handle<float> genweightHandle;
    iEvent.getByToken(genweightToken_, genweightHandle);
    b_weight = (*genweightHandle);

    edm::Handle<float> genweightQHandle;
    iEvent.getByToken(genweightQToken_, genweightQHandle);
    b_weightQ = (*genweightQHandle);

    edm::Handle< vector<float> > pdfWeightsHandle;
    iEvent.getByToken(pdfWeightsToken_, pdfWeightsHandle);
    for (const float & aPdfWeight : *pdfWeightsHandle)
    {
      b_pdfWeights.push_back(aPdfWeight);
    }


    //////
    edm::Handle<int> genTtbarIdHandle;
    if(iEvent.getByToken(genTtbarIdToken_, genTtbarIdHandle))     b_genTtbarId = *genTtbarIdHandle;

    edm::Handle<int> genTtbarIdHandle30;
    if(iEvent.getByToken(genTtbarIdToken30_, genTtbarIdHandle30)) b_genTtbarId30 = *genTtbarIdHandle30;

    edm::Handle<int> genTtbarIdHandle40;
    if(iEvent.getByToken(genTtbarIdToken40_, genTtbarIdHandle40)) b_genTtbarId40 = *genTtbarIdHandle40;

    edm::Handle<reco::GenJetCollection> genJets;
    iEvent.getByToken(GenJetsToken_, genJets);
    int nJet20 = 0, nJet30 = 0, nJet40 = 0;
    for (const reco::GenJet & aGenJet : *genJets)
    {
      if ( aGenJet.pt() < 20 || fabs(aGenJet.eta()) > 2.4 ) continue;
      nJet20++;
      if ( aGenJet.pt() < 30 || fabs(aGenJet.eta()) > 2.4 ) continue;
      nJet30++;
      if ( aGenJet.pt() < 40 || fabs(aGenJet.eta()) > 2.4 ) continue;
      nJet40++;
    }
    b_NgenJet   = nJet20;
    b_NgenJet30 = nJet30;
    b_NgenJet40 = nJet40;

  }

  edm::Handle<reco::VertexCollection> vertices;
  iEvent.getByToken(vtxToken_, vertices);
  if (vertices->empty()){ // skip the event if no PV found
    ttree_->Fill();
    return;
  }
  cutflow_[1][b_channel]++;

  // const reco::Vertex &PV = vertices->front();
  edm::Handle<int> nGoodVertexHandle;
  iEvent.getByToken(nGoodVertexToken_, nGoodVertexHandle);
  b_nvertex = *nGoodVertexHandle;

  edm::Handle<int> lumiSelectionHandle;
  iEvent.getByToken(lumiSelectionToken_, lumiSelectionHandle);
  if (!runOnMC_){
    if (*lumiSelectionHandle == 0) return;
  }

  edm::Handle<int> recoFiltersHandle;
  iEvent.getByToken(recoFiltersToken_, recoFiltersHandle);
  b_filtered = *recoFiltersHandle == 0 ? false : true;
  // if (!b_filtered){
  //   ttree_->Fill();
  //   return;
  // }
  cutflow_[2][b_channel]++;

  edm::Handle<cat::MuonCollection> muons;          iEvent.getByToken(muonToken_, muons);
  edm::Handle<cat::ElectronCollection> electrons;  iEvent.getByToken(elecToken_, electrons);
  edm::Handle<cat::JetCollection> jets;            iEvent.getByToken(jetToken_, jets);
  edm::Handle<cat::METCollection> mets;            iEvent.getByToken(metToken_, mets);

  // Find leptons and sort by pT
  LeptonCollection recolep;
  selectMuons(*muons, recolep);
  selectElecs(*electrons, recolep);

  if (recolep.size() < 2){
    ttree_->Fill();
    return;
  }
  cutflow_[3][b_channel]++;

  sort(recolep.begin(), recolep.end(), GtByCandPt());
  int lep1_idx=0, lep2_idx=1;
  //for emu
  if(std::abs(recolep[1].pdgId())==11 && std::abs(recolep[0].pdgId())==13){
    lep1_idx = 1; lep2_idx = 0;
  }

  const cat::Lepton& recolep1 = recolep[lep1_idx];
  const cat::Lepton& recolep2 = recolep[lep2_idx];

  // Determine channel
  const int pdgIdSum = std::abs(recolep1.pdgId()) + std::abs(recolep2.pdgId());
  if (pdgIdSum == 24) b_channel = CH_MUEL; // emu
  if (pdgIdSum == 22) b_channel = CH_ELEL; // ee
  if (pdgIdSum == 26) b_channel = CH_MUMU; // mumu

  // Trigger results
  edm::Handle<int> trigHandle;
  if      ( b_channel == CH_ELEL ) iEvent.getByToken(trigTokenELEL_, trigHandle);
  else if ( b_channel == CH_MUMU ) iEvent.getByToken(trigTokenMUMU_, trigHandle);
  else if ( b_channel == CH_MUEL ) iEvent.getByToken(trigTokenMUEL_, trigHandle);
  b_tri = *trigHandle;

  b_lep1_pt = recolep1.pt(); b_lep1_eta = recolep1.eta(); b_lep1_phi = recolep1.phi(); b_lep1_q = recolep1.charge();
  b_lep2_pt = recolep2.pt(); b_lep2_eta = recolep2.eta(); b_lep2_phi = recolep2.phi(); b_lep2_q = recolep2.charge();

  //LeptonWeight LepWeight;
  double sf1 = 1.0;
  double sf2 = 1.0;
  if (pdgIdSum == 24) {
    b_lep1_RelIso = recolep1.relIso();     b_lep2_RelIso = recolep2.relIso(0.4);
    sf1 =  elecSF_(b_lep1_pt, b_lep1_eta);
    sf2 =  MuonSF(b_lep2_pt, b_lep2_eta);
    //sf2 =  MuonSF_(std::abs(b_lep2_eta), b_lep2_pt);
  } // emu
  if (pdgIdSum == 22) {
    b_lep1_RelIso = recolep1.relIso();     b_lep2_RelIso = recolep2.relIso();
    sf1 = elecSF_(b_lep1_pt, b_lep1_eta);
    sf2 = elecSF_(b_lep2_pt, b_lep2_eta);
  } // ee
  if (pdgIdSum == 26) {
    b_lep1_RelIso = recolep1.relIso(0.4);  b_lep2_RelIso = recolep2.relIso(0.4);
    sf1 =  MuonSF(b_lep1_pt, b_lep1_eta);
    sf2 =  MuonSF(b_lep2_pt, b_lep2_eta);
    //sf1 =  MuonSF_(std::abs(b_lep1_eta), b_lep1_pt);
    //sf2 =  MuonSF_(std::abs(b_lep2_eta), b_lep2_pt);
  } // mumu

  if(runOnMC_) b_lepweight = sf1 * sf2;

  const auto tlv_ll = recolep1.p4()+recolep2.p4();
  b_ll_pt = tlv_ll.Pt(); b_ll_eta = tlv_ll.Eta(); b_ll_phi = tlv_ll.Phi(); b_ll_m = tlv_ll.M();

  //if (b_ll_m > 20. && recolep1.charge() * recolep2.charge() < 0){
  if (b_ll_m < 20. || recolep1.charge() * recolep2.charge() > 0){
    ttree_->Fill();
    return;
  }
  b_step1 = true;
  b_step = 1;
  cutflow_[4][b_channel]++;

  if ( (b_channel == CH_MUEL) || ((b_ll_m < 76) || (b_ll_m > 106)) ){
    b_step2 = true;
    b_step = 2;
    cutflow_[5][b_channel]++;
  }

  for (int sys = 0; sys < 5; ++sys){
    if (sys > 2 && !runOnMC_) break;
    if(sys>0 && (b_step3==false || b_step2==false || b_step1==false) ) break;

    //JetCollection&& selectedJets = selectJets(*jets, recolep);
    JetCollection&& selectedJets = selectJets(*jets, recolep, (sys_e)sys);
    JetCollection&& selectedBJetsL = selectBJets(selectedJets,0.605);
    JetCollection&& selectedBJetsM = selectBJets(selectedJets,0.890);
    JetCollection&& selectedBJetsT = selectBJets(selectedJets,0.970);

    int idx=0;
    std::map<int,float> mapJetBDiscriminator;
    for (auto jet1 = selectedJets.begin(), end = selectedJets.end(); jet1 != end; ++jet1){
      float bDisCSV= (float) jet1->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
      //float bDisCSV= (float) jet1->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTagsAK4PFPuppi");
      int flavor = jet1->partonFlavour();
      mapJetBDiscriminator[idx] = bDisCSV;
      idx++;
      //b_jets_pt.push_back(jet1->p4().pt()*jet1->smearedRes());
      b_jets_pt.push_back(jet1->p4().pt());
      b_jets_eta.push_back(jet1->p4().eta());
      b_jets_phi.push_back(jet1->p4().phi());
      b_jets_flavor.push_back(flavor);
      b_jets_bDiscriminatorCSV.push_back(bDisCSV);

      /*      if (runOnMC_) {
              cat::Jet::JETFLAV fla = cat::Jet::JETFLAV_LIGHT;
              if (abs(flavor)==4){ fla=cat::Jet::JETFLAV_C; }
              if (abs(flavor)==5){ fla=cat::Jet::JETFLAV_B; }
              b_csvl_sf = b_csvl_sf*(jet1->scaleFactorCSVv2(cat::Jet::BTAGCSV_LOOSE,0,fla));
              b_csvl_sfup = b_csvl_sfup*(jet1->scaleFactorCSVv2(cat::Jet::BTAGCSV_LOOSE,1,fla));
              b_csvl_sfdw = b_csvl_sfdw*(jet1->scaleFactorCSVv2(cat::Jet::BTAGCSV_LOOSE,-1,fla));

              b_csvm_sf = b_csvm_sf*(jet1->scaleFactorCSVv2(cat::Jet::BTAGCSV_MEDIUM,0,fla));
              b_csvm_sfup = b_csvm_sfup*(jet1->scaleFactorCSVv2(cat::Jet::BTAGCSV_MEDIUM,1,fla));
              b_csvm_sfdw = b_csvm_sfdw*(jet1->scaleFactorCSVv2(cat::Jet::BTAGCSV_MEDIUM,-1,fla));

              b_csvt_sf = b_csvt_sf*(jet1->scaleFactorCSVv2(cat::Jet::BTAGCSV_TIGHT,0,fla));
              b_csvt_sfup = b_csvt_sfup*(jet1->scaleFactorCSVv2(cat::Jet::BTAGCSV_TIGHT,1,fla));
              b_csvt_sfdw = b_csvt_sfdw*(jet1->scaleFactorCSVv2(cat::Jet::BTAGCSV_TIGHT,-1,fla));
              }*/
    }

    if (runOnMC_){
      //double csvWgtHF, csvWgtLF, csvWgtCF;
      for ( const auto& jet : selectedJets ) {
        b_csvweight         *= csvWeight(jet, CSVWeightEvaluator::CENTRAL);//);
        b_csvweight_JES_Up  *= csvWeight(jet, CSVWeightEvaluator::JES_UP);
        b_csvweight_JES_Down*= csvWeight(jet, CSVWeightEvaluator::JES_DN);
        b_csvweight_LF_Up   *= csvWeight(jet, CSVWeightEvaluator::LF_UP);
        b_csvweight_LF_Down *= csvWeight(jet, CSVWeightEvaluator::LF_DN);
        b_csvweight_HF_Up   *= csvWeight(jet, CSVWeightEvaluator::HF_UP);
        b_csvweight_HF_Down *= csvWeight(jet, CSVWeightEvaluator::HF_DN);
        b_csvweight_HF_Stats1_Up  *= csvWeight(jet, CSVWeightEvaluator::HFSTAT1_UP);
        b_csvweight_HF_Stats1_Down*= csvWeight(jet, CSVWeightEvaluator::HFSTAT1_DN);
        b_csvweight_HF_Stats2_Up  *= csvWeight(jet, CSVWeightEvaluator::HFSTAT2_UP);
        b_csvweight_HF_Stats2_Down*= csvWeight(jet, CSVWeightEvaluator::HFSTAT2_DN);
        b_csvweight_LF_Stats1_Up  *= csvWeight(jet, CSVWeightEvaluator::LFSTAT1_UP);
        b_csvweight_LF_Stats1_Down*= csvWeight(jet, CSVWeightEvaluator::LFSTAT1_DN);
        b_csvweight_LF_Stats2_Up  *= csvWeight(jet, CSVWeightEvaluator::LFSTAT2_UP);
        b_csvweight_LF_Stats2_Down*= csvWeight(jet, CSVWeightEvaluator::LFSTAT2_DN);
        b_csvweight_Charm_Err1_Up *= csvWeight(jet, CSVWeightEvaluator::CFERR1_UP);
        b_csvweight_Charm_Err1_Down*= csvWeight(jet, CSVWeightEvaluator::CFERR1_DN);
        b_csvweight_Charm_Err2_Up  *= csvWeight(jet, CSVWeightEvaluator::CFERR2_UP);
        b_csvweight_Charm_Err2_Down*= csvWeight(jet, CSVWeightEvaluator::CFERR2_DN);
      }
    }
    //csvd order
    std::vector<data_t> vecJetBDisc(mapJetBDiscriminator.begin(), mapJetBDiscriminator.end());
    std::sort(vecJetBDisc.begin(), vecJetBDisc.end(), bigger_second<data_t>());
    for ( const auto& x : vecJetBDisc ) b_csvd_jetid.push_back(x.first);

    const auto met = mets->front().p4();
    b_met = met.pt();
    b_metphi = met.phi();
    b_njet30 = selectedJets.size();
    b_nbjetL30 = selectedBJetsL.size();
    b_nbjetM30 = selectedBJetsM.size();
    b_nbjetT30 = selectedBJetsT.size();

    if ((b_channel == CH_MUEL) || (b_met > 40.)){
      b_step3 = true;
      if (b_step == 2){
        ++b_step;
        cutflow_[6][b_channel]++;
      }
    }

    if (selectedJets.size() >3 ){
      b_step4 = true;
      if (b_step == 3){
        ++b_step;
        cutflow_[7][b_channel]++;
      }
    }


    if (selectedBJetsM.size() > 1){
      b_step5 = true;
      if (b_step == 4){
        ++b_step;
        cutflow_[8][b_channel]++;
      }
    }

    if (selectedBJetsT.size() > 1){
      b_step6 = true;
      if (b_step == 5){
        ++b_step;
        cutflow_[9][b_channel]++;
      }
    }

    if(sys==0){
      ttree_->Fill();
      ttree2_->Fill();
    }else if (sys==1) ttree3_->Fill();
    else if (sys==2) ttree4_->Fill();
    else if (sys==3) ttree5_->Fill();
    else if (sys==4) ttree6_->Fill();

  }
}
const bool TtbarBbbarDiLeptonAnalyzer::isLastP( const reco::GenParticle& p) const
{

  bool out = true;

  int id = abs( p.pdgId() );

  unsigned int nDaughters = p.numberOfDaughters();
  for ( unsigned iDaughter=0; iDaughter<nDaughters; ++iDaughter ) {
    const reco::Candidate* daugh = p.daughter(iDaughter);
    if( abs(daugh->pdgId()) == id) {
      out = false;
      break;
    }
  }

  return out;
}
const reco::Candidate* TtbarBbbarDiLeptonAnalyzer::getLast(const reco::Candidate* p) const
{
  for ( size_t i=0, n=p->numberOfDaughters(); i<n; ++i )
  {
    const reco::Candidate* dau = p->daughter(i);
    if ( p->pdgId() == dau->pdgId() ) return getLast(dau);
  }
  return p;
}

void TtbarBbbarDiLeptonAnalyzer::selectMuons(const cat::MuonCollection& muons, LeptonCollection& selmuons) const
{
  for (auto& mu : muons) {
    if (mu.pt() < 20.) continue;
    if (std::abs(mu.eta()) > 2.4) continue;
    //if (!mu.isMediumMuon()) continue;
    if (!mu.isTightMuon()) continue;
    if (mu.relIso(0.4) > 0.15) continue;
    //printf("muon with pt %4.1f, POG loose id %d, tight id %d\n", mu.pt(), mu.isLooseMuon(), mu.isTightMuon());
    selmuons.push_back(mu);
  }
}

void TtbarBbbarDiLeptonAnalyzer::selectElecs(const cat::ElectronCollection& elecs, LeptonCollection& selelecs) const
{
  for (auto& el : elecs) {
    if (el.pt() < 20.) continue;
    if ((std::abs(el.scEta()) > 1.4442) && (std::abs(el.scEta()) < 1.566)) continue;
    if (std::abs(el.eta()) > 2.4) continue;
    //if (!el.electronID("cutBasedElectronID-Spring15-50ns-V1-standalone-medium")) continue;
    //if (el.electronID("cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-medium") == 0) continue;
    if (!el.electronID("mvaEleID-Spring15-25ns-Trig-V1-wp90")) continue;
    //if ( !el.electronID("cutBasedElectronID-Spring15-25ns-V1-standalone-medium") ) continue;
    //if (!el.passConversionVeto()) continue;
    //if (!el.isPF()) continue;
    if (el.relIso(0.3) > 0.12) continue;


    //printf("electron with pt %4.1f\n", el.pt());
    selelecs.push_back(el);
  }
}


cat::JetCollection TtbarBbbarDiLeptonAnalyzer::selectJets(const cat::JetCollection& jets, const LeptonCollection& recolep, sys_e sys)
{
  /*
     cat::JetCollection seljets;
     for (auto& jet : jets) {
  //if (jet.pt()*jet.smearedRes() < 30.) continue;
  if (jet.pt() < 30.) continue;
  if (std::abs(jet.eta()) > 2.4)  continue;
  if (!jet.LooseId()) continue;

  bool hasOverLap = false;
  for (auto lep : recolep){
  if (deltaR(jet.p4(),lep.p4()) < 0.4) hasOverLap = true;
  }
  if (hasOverLap) continue;
  // printf("jet with pt %4.1f\n", jet.pt());
  seljets.push_back(jet);
  }
  return seljets;
  */
  cat::JetCollection seljets;
  for (auto& j : jets) {
    cat::Jet jet(j);
    if (sys == sys_jes_u) jet.setP4(j.p4() * j.shiftedEnUp());
    if (sys == sys_jes_d) jet.setP4(j.p4() * j.shiftedEnDown());
    if (sys == sys_jer_u) jet.setP4(j.p4() * j.smearedResUp());
    if (sys == sys_jer_d) jet.setP4(j.p4() * j.smearedResDown());

    if (jet.pt() < 30.) continue;
    if (std::abs(jet.eta()) > 2.4)  continue;
    if (!jet.LooseId()) continue;

    bool hasOverLap = false;
    for (auto lep : recolep){
      if (deltaR(jet.p4(),lep.p4()) < 0.4) hasOverLap = true;
    }
    if (hasOverLap) continue;
    // printf("jet with pt %4.1f\n", jet.pt());
    //if (sys == sys_btag_u) b_btagweight *= jet.scaleFactorCSVv2(cat::Jet::BTAGCSV_LOOSE, 1);
    //else if (sys == sys_btag_d) b_btagweight *= jet.scaleFactorCSVv2(cat::Jet::BTAGCSV_LOOSE, -1);
    //else b_btagweight *= jet.scaleFactorCSVv2(cat::Jet::BTAGCSV_LOOSE, 0);
    ////
    b_csvl_sf   = b_csvl_sf   * jet.scaleFactorCSVv2(cat::Jet::BTAGCSV_LOOSE,0);
    b_csvl_sfup = b_csvl_sfup * jet.scaleFactorCSVv2(cat::Jet::BTAGCSV_LOOSE,1);
    b_csvl_sfdw = b_csvl_sfdw * jet.scaleFactorCSVv2(cat::Jet::BTAGCSV_LOOSE,-1);

    b_csvm_sf   = b_csvm_sf   * jet.scaleFactorCSVv2(cat::Jet::BTAGCSV_MEDIUM,0);
    b_csvm_sfup = b_csvm_sfup * jet.scaleFactorCSVv2(cat::Jet::BTAGCSV_MEDIUM,1);
    b_csvm_sfdw = b_csvm_sfdw * jet.scaleFactorCSVv2(cat::Jet::BTAGCSV_MEDIUM,-1);

    b_csvt_sf   = b_csvt_sf   * jet.scaleFactorCSVv2(cat::Jet::BTAGCSV_TIGHT,0);
    b_csvt_sfup = b_csvt_sfup * jet.scaleFactorCSVv2(cat::Jet::BTAGCSV_TIGHT,1);
    b_csvt_sfdw = b_csvt_sfdw * jet.scaleFactorCSVv2(cat::Jet::BTAGCSV_TIGHT,-1);

    seljets.push_back(jet);
  }
  return seljets;
}

cat::JetCollection TtbarBbbarDiLeptonAnalyzer::selectBJets(const JetCollection& jets, double workingpoint) const
{
  //https://twiki.cern.ch/twiki/bin/view/CMS/TopBTV
  // 25ns pfCombinedInclusiveSecondaryVertexV2BJetTags :L,M,T 0.605, 0.890, 0.970
  cat::JetCollection selBjets;
  for (auto& jet : jets) {
    if (jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") < workingpoint) continue;
    //if (jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTagsAK4PFPuppi") < workingpoint) continue;
    //printf("b jet with pt %4.1f\n", jet.pt());
    selBjets.push_back(jet);
  }
  return selBjets;
}
void TtbarBbbarDiLeptonAnalyzer::resetBr()
{

  b_nvertex = 0;b_step = -1;b_channel = 0;
  b_njet30 = 0; b_nbjetL30=0, b_nbjetM30 = 0; b_nbjetT30 = 0;
  b_step1 = 0;b_step2 = 0;b_step3 = 0;b_step4 = 0;b_step5 = 0;b_step6 = 0;b_tri = 0;b_filtered = 0;
  b_met = -9; b_metphi = -9;

  b_weight = 1; b_weightQ = 1; b_puweight = 1; b_puweightUp = 1; b_puweightDown =1;
  b_lepweight = 1;
  b_pdfWeights.clear();

  ///////
  b_lep1_pt = -9;b_lep1_eta = -9;b_lep1_phi = -9; b_lep1_RelIso = -9; b_lep1_q=0;
  b_lep2_pt = -9;b_lep2_eta = -9;b_lep2_phi = -9; b_lep2_RelIso = -9; b_lep2_q=0;
  b_ll_pt = -9;b_ll_eta = -9;b_ll_phi = -9;b_ll_m = -9;
  resetBrJets();
  resetBrGEN();
  //////
}
void TtbarBbbarDiLeptonAnalyzer::resetBrJets()
{

  b_jets_pt.clear();
  b_jets_eta.clear();
  b_jets_phi.clear();
  b_jets_flavor.clear();
  b_jets_bDiscriminatorCSV.clear();
  b_csvd_jetid.clear();

  b_csvweight = 1;
  b_csvweight_JES_Up = 1;
  b_csvweight_JES_Down = 1;
  b_csvweight_LF_Up = 1;
  b_csvweight_LF_Down = 1;
  b_csvweight_HF_Up = 1;
  b_csvweight_HF_Down = 1;
  b_csvweight_HF_Stats1_Up = 1;
  b_csvweight_HF_Stats1_Down = 1;
  b_csvweight_HF_Stats2_Up = 1;
  b_csvweight_HF_Stats2_Down = 1;
  b_csvweight_LF_Stats1_Up = 1;
  b_csvweight_LF_Stats1_Down = 1;
  b_csvweight_LF_Stats2_Up = 1;
  b_csvweight_LF_Stats2_Down = 1;
  b_csvweight_Charm_Err1_Up = 1;
  b_csvweight_Charm_Err1_Down = 1;
  b_csvweight_Charm_Err2_Up = 1;
  b_csvweight_Charm_Err2_Down = 1;


  b_csvl_sf = 1;  b_csvl_sfup = 1;  b_csvl_sfdw = 1;
  b_csvm_sf = 1;  b_csvm_sfup = 1;  b_csvm_sfdw = 1;
  b_csvt_sf = 1;  b_csvt_sfup = 1;  b_csvt_sfdw = 1;
}
void TtbarBbbarDiLeptonAnalyzer::resetBrGEN()
{
  b_genTtbarId=0; b_genTtbarId30=0; b_genTtbarId40=0;
  b_NgenJet=0; b_NgenJet30=0; b_NgenJet40=0;

  b_partonChannel = -1; b_partonMode1 = -1; b_partonMode2 = -1;
  b_partonlep1_pt = -9; b_partonlep1_eta = -9;
  b_partonlep2_pt = -9; b_partonlep2_eta = -9;
  b_partonInPhase = 0; b_partonInPhaseLep = false; b_partonInPhaseJet = false;

  lepton1_pt  =0.0      ;//  cms.string("lepton1().Pt()"),
  lepton1_eta =-9.0     ;//  cms.string("lepton1().Eta()"),
  lepton1_phi =-9.0     ;//  cms.string("lepton1().Phi()"),
  lepton2_pt  =0.0      ;//  cms.string("lepton2().Pt()"),
  lepton2_eta =-9.0     ;//  cms.string("lepton2().Eta()"),
  lepton2_phi =-9.0     ;//  cms.string("lepton2().Phi()"),

  allHadronic      =false;//  cms.string("allHadronic"),
  semiLeptonicM1     =false;
  semiLeptonic0     =false;
  semiLeptonicP1     =false;


  diLeptonicM1 =false;
  diLeptonic0 =false;
  diLeptonicP1 =false;

  diLeptonicMuoMuo =false;//  cms.string("diLeptonicMuoMuo"),
  diLeptonicMuoEle =false;//  cms.string("diLeptonicMuoEle"),
  diLeptonicEleEle =false;//  cms.string("diLeptonicEleEle"),
  diLeptonicTauMuo =false;//  cms.string("diLeptonicTauMuo"),
  diLeptonicTauEle =false;//  cms.string("diLeptonicTauEle"),
  diLeptonicTauTau =false;//  cms.string("diLeptonicTauTau"),

  NbJets1           =0;//  cms.string("NbJets(1)"),
  NbJets201         =0;//  cms.string("NbJets20(1)"),
  NbJets251         =0;//  cms.string("NbJets25(1)"),
  NbJets301         =0;//  cms.string("NbJets30(1)"),
  NbJets401         =0;//  cms.string("NbJets40(1)"),
  NaddbJets1        =0;//  cms.string("NaddbJets(1)"),
  NaddbJets201      =0;//  cms.string("NaddbJets20(1)"),
  NaddbJets401      =0;//  cms.string("NaddbJets40(1)"),
  NcJets1           =0;//  cms.string("NcJets(1)"),
  NcJets101         =0;//  cms.string("NcJets10(1)"),
  NcJets151         =0;//  cms.string("NcJets15(1)"),
  NcJets201         =0;//  cms.string("NcJets20(1)"),
  NcJets251         =0;//  cms.string("NcJets25(1)"),
  NcJets301         =0;//  cms.string("NcJets30(1)"),
  NcJets401         =0;//  cms.string("NcJets40(1)"),
  NbJets           =0;//  cms.string("NbJets(0)"),
  NbJets20         =0;//  cms.string("NbJets20(0)"),
  NbJets25         =0;//  cms.string("NbJets25(0)"),
  NbJets30         =0;//  cms.string("NbJets30(0)"),
  NbJets40         =0;//  cms.string("NbJets40(0)"),
  NaddbJets        =0;//  cms.string("NaddbJets(0)"),
  NaddbJets20      =0;//  cms.string("NaddbJets20(0)"),
  NaddbJets40      =0;//  cms.string("NaddbJets40(0)"),
  NcJets           =0;//  cms.string("NcJets(0)"),
  NcJets10         =0;//  cms.string("NcJets10(0)"),
  NcJets15         =0;//  cms.string("NcJets15(0)"),
  NcJets20         =0;//  cms.string("NcJets20(0)"),
  NcJets25         =0;//  cms.string("NcJets25(0)"),
  NcJets30         =0;//  cms.string("NcJets30(0)"),
  NcJets40         =0;//  cms.string("NcJets40(0)"),
  NJets            =0;//  cms.string("NJets"),
  NJets10          =0;//  cms.string("NJets10"),
  NJets20          =0;//  cms.string("NJets20"),
  NJets25          =0;//  cms.string("NJets25"),
  NJets30          =0;//  cms.string("NJets30"),
  NJets40          =0;//  cms.string("NJets40"),
  NaddJets20       =0;//  cms.string("NaddJets20"),
  NaddJets40       =0;//  cms.string("NaddJets40"),
  NbQuarksTop      =0;//  cms.string("NbQuarksTop"),
  NbQuarksNoTop    =0;//  cms.string("NbQuarksNoTop"),
  NbQuarks         =0;//  cms.string("NbQuarks"),
  NbQuarks20       =0;//  cms.string("NbQuarks20"),
  NbQuarks40       =0;//  cms.string("NbQuarks40"),
  NaddbQuarks20    =0;//  cms.string("NaddbQuarks20"),
  NaddbQuarks40    =0;//  cms.string("NaddbQuarks40"),
  NcQuarks         =0;//  cms.string("NcQuarks"),

  b_is3lep = -9;
}
float TtbarBbbarDiLeptonAnalyzer::MuonSF(float pt, float eta)
{
  if (eta >=-2.4 && eta <-2.1){
    if     (pt > 20.0 && pt <=25.0 )  return (float) 0.96652;
    else if(pt > 25.0 && pt <=30.0 )  return (float) 0.97249;
    else if(pt > 30.0 && pt <=40.0 )  return (float) 0.98056;
    else if(pt > 40.0 && pt <=50.0 )  return (float) 0.98035;
    else if(pt > 50.0 && pt <=60.0 )  return (float) 0.98217;
    else if(pt > 60.0 )  return (float) 0.93115;
    else return (float) 1.0;
  }
  else if (eta >=-2.1 && eta <-1.2){
    if(pt > 20.0 && pt <=25.0 )  return (float) 0.99707;
    else if(pt > 25.0 && pt <=30.0 )  return (float) 0.9941;
    else if(pt > 30.0 && pt <=40.0 )  return (float) 0.9938;
    else if(pt > 40.0 && pt <=50.0 )  return (float) 0.9935;
    else if(pt > 50.0 && pt <=60.0 )  return (float) 0.9876;
    else if(pt > 60.0 )  return (float) 0.98751;
    else return (float) 1.0;
  }
  else if (eta >=-1.2 && eta <-0.9){
    if(pt > 20.0 && pt <=25.0 )  return (float) 0.9680;
    else if(pt > 25.0 && pt <=30.0 )  return (float) 0.9795;
    else if(pt > 30.0 && pt <=40.0 )  return (float) 0.9802;
    else if(pt > 40.0 && pt <=50.0 )  return (float) 0.9835;
    else if(pt > 50.0 && pt <=60.0 )  return (float) 0.9806;
    else if(pt > 60.0 )  return (float) 0.98020;
    else return (float) 1.0;
  }
  else if (eta >=-0.9 && eta <-0.0){
    if(pt > 20.0 && pt <=25.0 )  return (float) 0.97835;
    else if(pt > 25.0 && pt <=30.0 )  return (float) 0.9804;
    else if(pt > 30.0 && pt <=40.0 )  return (float) 0.9875;
    else if(pt > 40.0 && pt <=50.0 )  return (float) 0.9877;
    else if(pt > 50.0 && pt <=60.0 )  return (float) 0.9808;
    else if(pt > 60.0)  return (float) 0.98220;
    else return (float) 1.0;
  }
  else if (eta >=0.0 && eta <0.9){
    if(pt > 20.0 && pt <=25.0 )  return (float) 0.9783;
    else if(pt > 25.0 && pt <=30.0 )  return (float) 0.9804;
    else if(pt > 30.0 && pt <=40.0 )  return (float) 0.9875;
    else if(pt > 40.0 && pt <=50.0 )  return (float) 0.9877;
    else if(pt > 50.0 && pt <=60.0 )  return (float) 0.9808;
    else if(pt > 60.0 )  return (float) 0.98220;
    else return (float) 1.0;
  }
  else if (eta >=0.9 && eta <1.2){
    if(pt > 20.0 && pt <=25.0 )  return (float) 0.9680;
    else if(pt > 25.0 && pt <=30.0 )  return (float) 0.9795;
    else if(pt > 30.0 && pt <=40.0 )  return (float) 0.9802;
    else if(pt > 40.0 && pt <=50.0 )  return (float) 0.9835;
    else if(pt > 50.0 && pt <=60.0 )  return (float) 0.9806;
    else if(pt > 60.0 )  return (float) 0.98020;
    else return (float) 1.0;
  }
  else if (eta >=1.2 && eta <2.1){
    if     (pt > 20.0 && pt <=25.0 )  return (float) 0.99707;
    else if(pt > 25.0 && pt <=30.0 )  return (float) 0.99410;
    else if(pt > 30.0 && pt <=40.0 )  return (float) 0.99382;
    else if(pt > 40.0 && pt <=50.0 )  return (float) 0.99353;
    else if(pt > 50.0 && pt <=60.0 )  return (float) 0.98761;
    else if(pt > 60.0)  return (float) 0.98751;
    else return (float) 1.0;
  }
  else if (eta >=2.1 && eta <=2.4){
    if     (pt > 20.0 && pt <=25.0 )  return (float) 0.96652;
    else if(pt > 25.0 && pt <=30.0 )  return (float) 0.9725;
    else if(pt > 30.0 && pt <=40.0 )  return (float) 0.98057;
    else if(pt > 40.0 && pt <=50.0 )  return (float) 0.98035;
    else if(pt > 50.0 && pt <=60.0 )  return (float) 0.98217;
    else if(pt > 60.0 )  return (float) 0.9311;
    else return (float) 1.;
  }
  else return (float) 1.;

};

//define this as a plug-in
DEFINE_FWK_MODULE(TtbarBbbarDiLeptonAnalyzer);
