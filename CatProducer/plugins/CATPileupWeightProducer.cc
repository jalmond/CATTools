#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/Common/interface/View.h"

#include <memory>
#include <vector>
#include <string>

using namespace std;

class CATPileupWeightProducer : public edm::stream::EDProducer<>
{
public:
  CATPileupWeightProducer(const edm::ParameterSet& pset);
  ~CATPileupWeightProducer() {};

  void produce(edm::Event& event, const edm::EventSetup& eventSetup) override;

private:
  edm::LumiReWeighting lumiWeights_, lumiWeightsUp_, lumiWeightsDn_;

  edm::LumiReWeighting lumiWeights_periodB_, lumiWeights_periodC_, lumiWeights_periodD_, lumiWeights_periodE_,lumiWeights_periodF_,lumiWeights_periodG_,lumiWeights_periodH_;


  enum class WeightingMethod { Standard, RedoWeight, NVertex };
  WeightingMethod weightingMethod_;

  std::vector<double> simpleWeights_;

  typedef std::vector<PileupSummaryInfo> PUInfos;
  edm::EDGetTokenT<PUInfos> puToken_;
  edm::EDGetTokenT<reco::VertexCollection> vertexToken_;

  edm::EDGetTokenT<int> nTrueIntrToken_;

  bool doPeriodWeights_;
};
CATPileupWeightProducer::CATPileupWeightProducer(const edm::ParameterSet& pset)
{
  
  doPeriodWeights_ = pset.getParameter<bool>("doPeriodWeights");

  const string methodName = pset.getParameter<string>("weightingMethod");
  if      ( methodName == "Standard" ) weightingMethod_ = WeightingMethod::Standard;
  else if ( methodName == "RedoWeight" ) weightingMethod_ = WeightingMethod::RedoWeight;
  else if ( methodName == "NVertex"  ) weightingMethod_ = WeightingMethod::NVertex;
  else throw cms::Exception("ConfigError") << "Cannot find weighting method \""
                                           << methodName << "\", should be Standard, RedoWeight or NVertex\n";

  if ( weightingMethod_ == WeightingMethod::NVertex )
  {
    std::cerr << "!!CATPileupWeightProducer!! We are using NON STANDARD method for the pileup reweight.\n"
              << "                         This weight values are directly from reco vertex\n";
    vertexToken_ = consumes<reco::VertexCollection>(pset.getParameter<edm::InputTag>("vertex"));
    simpleWeights_ = pset.getParameter<std::vector<double> >("simpleWeights");
    const double sumW = std::accumulate(simpleWeights_.begin(), simpleWeights_.end(), 0.);
    if ( sumW > 0 ) { for ( auto& w : simpleWeights_ ) { w /= sumW; } }
  }
  else
  {
    if ( weightingMethod_ == WeightingMethod::Standard )
    {
      puToken_ = consumes<PUInfos>(pset.getParameter<edm::InputTag>("pileupInfo"));
      vertexToken_ = consumes<reco::VertexCollection>(pset.getParameter<edm::InputTag>("vertex"));
    }
    else if ( weightingMethod_ == WeightingMethod::RedoWeight )
    {
      nTrueIntrToken_ = consumes<int>(pset.getParameter<edm::InputTag>("nTrueIntr"));
    }
    std::vector<double> pileupMC = pset.getParameter<std::vector<double> >("pileupMC");
    std::vector<double> pileupRD = pset.getParameter<std::vector<double> >("pileupRD");
    std::vector<double> pileupUp = pset.getParameter<std::vector<double> >("pileupUp");
    std::vector<double> pileupDn = pset.getParameter<std::vector<double> >("pileupDn");

    
    std::vector<double> pileupRD_B = pset.getParameter<std::vector<double> >("pileupRD_B");
    std::vector<double> pileupRD_C = pset.getParameter<std::vector<double> >("pileupRD_C");
    std::vector<double> pileupRD_D = pset.getParameter<std::vector<double> >("pileupRD_D");
    std::vector<double> pileupRD_E = pset.getParameter<std::vector<double> >("pileupRD_E");
    std::vector<double> pileupRD_F = pset.getParameter<std::vector<double> >("pileupRD_F");
    std::vector<double> pileupRD_G = pset.getParameter<std::vector<double> >("pileupRD_G");
    std::vector<double> pileupRD_H = pset.getParameter<std::vector<double> >("pileupRD_H");

    const double sumWMC = std::accumulate(pileupMC.begin(), pileupMC.end(), 0.);
    const double sumWRD = std::accumulate(pileupRD.begin(), pileupRD.end(), 0.);
    const double sumWUp = std::accumulate(pileupUp.begin(), pileupUp.end(), 0.);
    const double sumWDn = std::accumulate(pileupDn.begin(), pileupDn.end(), 0.);
    
    const double sumWRD_B = std::accumulate(pileupRD_B.begin(), pileupRD_B.end(), 0.);
    const double sumWRD_C = std::accumulate(pileupRD_C.begin(), pileupRD_C.end(), 0.);
    const double sumWRD_D = std::accumulate(pileupRD_D.begin(), pileupRD_D.end(), 0.);
    const double sumWRD_E = std::accumulate(pileupRD_E.begin(), pileupRD_E.end(), 0.);
    const double sumWRD_F = std::accumulate(pileupRD_F.begin(), pileupRD_F.end(), 0.);
    const double sumWRD_G = std::accumulate(pileupRD_G.begin(), pileupRD_G.end(), 0.);
    const double sumWRD_H = std::accumulate(pileupRD_H.begin(), pileupRD_H.end(), 0.);


    std::vector<float> pileupMCTmp;
    std::vector<float> pileupRDTmp;
    std::vector<float> pileupUpTmp, pileupDnTmp;
    std::vector<float> pileupRDTmp_B;
    std::vector<float> pileupRDTmp_C;
    std::vector<float> pileupRDTmp_D;
    std::vector<float> pileupRDTmp_E;
    std::vector<float> pileupRDTmp_F;
    std::vector<float> pileupRDTmp_G;
    std::vector<float> pileupRDTmp_H;

    for ( int i=0, n=min(pileupMC.size(), pileupRD.size()); i<n; ++i )
    {
      pileupMCTmp.push_back(pileupMC[i]/sumWMC);
      pileupRDTmp.push_back(pileupRD[i]/sumWRD);
      pileupUpTmp.push_back(pileupUp[i]/sumWUp);
      pileupDnTmp.push_back(pileupDn[i]/sumWDn);

      if(doPeriodWeights_){
	pileupRDTmp_B.push_back(pileupRD_B[i]/sumWRD_B);
	pileupRDTmp_C.push_back(pileupRD_C[i]/sumWRD_C);
	pileupRDTmp_D.push_back(pileupRD_D[i]/sumWRD_D);
	pileupRDTmp_E.push_back(pileupRD_E[i]/sumWRD_E);
	pileupRDTmp_F.push_back(pileupRD_F[i]/sumWRD_F);
	pileupRDTmp_G.push_back(pileupRD_G[i]/sumWRD_G);
	pileupRDTmp_H.push_back(pileupRD_H[i]/sumWRD_H);
      }

    }
    lumiWeights_ = edm::LumiReWeighting(pileupMCTmp, pileupRDTmp);
    lumiWeightsUp_ = edm::LumiReWeighting(pileupMCTmp, pileupUpTmp);
    lumiWeightsDn_ = edm::LumiReWeighting(pileupMCTmp, pileupDnTmp);
    if(doPeriodWeights_){
      lumiWeights_periodB_ = edm::LumiReWeighting(pileupMCTmp, pileupRDTmp_B);
      lumiWeights_periodC_ = edm::LumiReWeighting(pileupMCTmp, pileupRDTmp_C);
      lumiWeights_periodD_ = edm::LumiReWeighting(pileupMCTmp, pileupRDTmp_D);
      lumiWeights_periodE_ = edm::LumiReWeighting(pileupMCTmp, pileupRDTmp_E);
      lumiWeights_periodF_ = edm::LumiReWeighting(pileupMCTmp, pileupRDTmp_F);
      lumiWeights_periodG_ = edm::LumiReWeighting(pileupMCTmp, pileupRDTmp_G);
      lumiWeights_periodH_ = edm::LumiReWeighting(pileupMCTmp, pileupRDTmp_H);
    }
  }

  produces<int>("nTrueInteraction");
  produces<float>("");
  produces<float>("up");
  produces<float>("dn");

  produces<float>("periodB");
  produces<float>("periodC");
  produces<float>("periodD");
  produces<float>("periodE");
  produces<float>("periodF");
  produces<float>("periodG");
  produces<float>("periodH");


}

void CATPileupWeightProducer::produce(edm::Event& event, const edm::EventSetup& eventSetup)
{
  std::auto_ptr<int> nTrueIntr(new int(-1));
  std::auto_ptr<float> weight(new float(1.));
  std::auto_ptr<float> weightUp(new float(1.));
  std::auto_ptr<float> weightDn(new float(1.));

  std::auto_ptr<float> weight_B(new float(1.));
  std::auto_ptr<float> weight_C(new float(1.));
  std::auto_ptr<float> weight_D(new float(1.));
  std::auto_ptr<float> weight_E(new float(1.));
  std::auto_ptr<float> weight_F(new float(1.));
  std::auto_ptr<float> weight_G(new float(1.));
  std::auto_ptr<float> weight_H(new float(1.));


  if ( !event.isRealData() ){
    if ( weightingMethod_ == WeightingMethod::NVertex) {
      edm::Handle<reco::VertexCollection> vertexHandle;
      event.getByToken(vertexToken_, vertexHandle);

      const int nPVBin = std::min(simpleWeights_.size(), vertexHandle->size()) - 1;
      if ( nPVBin >= 0 ){
        *weight   = simpleWeights_[nPVBin];
        *weightUp = simpleWeights_[nPVBin];
        *weightDn = simpleWeights_[nPVBin];
      }
    }
    else {
      if ( weightingMethod_ == WeightingMethod::Standard ) {
        edm::Handle<std::vector<PileupSummaryInfo> > puHandle;
        event.getByToken(puToken_, puHandle);

        for ( auto& puInfo : *puHandle ){
          //const int nIntr = puInfo.getPU_NumInteractions();
          const int bx = puInfo.getBunchCrossing();

          if ( bx == 0 ){
            *nTrueIntr = puInfo.getTrueNumInteractions();
            break;
          }
        }
      }
      else if ( weightingMethod_ == WeightingMethod::RedoWeight ) {
        edm::Handle<int> nTrueIntrHandle;
        event.getByToken(nTrueIntrToken_, nTrueIntrHandle);
        *nTrueIntr = *nTrueIntrHandle;
      }

      if ( *nTrueIntr > 0 ) {
        *weight   = lumiWeights_.weight(*nTrueIntr);
        *weightUp = lumiWeightsUp_.weight(*nTrueIntr);
        *weightDn = lumiWeightsDn_.weight(*nTrueIntr);

	if(doPeriodWeights_){
	  *weight_B   = lumiWeights_periodB_.weight(*nTrueIntr);
	  *weight_C   = lumiWeights_periodC_.weight(*nTrueIntr);
	  *weight_D   = lumiWeights_periodD_.weight(*nTrueIntr);
	  *weight_E   = lumiWeights_periodE_.weight(*nTrueIntr);
	  *weight_F   = lumiWeights_periodF_.weight(*nTrueIntr);
	  *weight_G   = lumiWeights_periodG_.weight(*nTrueIntr);
	  *weight_H   = lumiWeights_periodH_.weight(*nTrueIntr);
	}
      }
    }
  }

  event.put(nTrueIntr, "nTrueInteraction");
  event.put(weight  , "");
  event.put(weightUp, "up");
  event.put(weightDn, "dn");


  
  if(doPeriodWeights_){
    event.put(weight_B  , "periodB");
    event.put(weight_C  , "periodC");
    event.put(weight_D  , "periodD");
    event.put(weight_E  , "periodE");
    event.put(weight_F  , "periodF");
    event.put(weight_G  , "periodG");
    event.put(weight_H  , "periodH");
  }
}


DEFINE_FWK_MODULE(CATPileupWeightProducer);
