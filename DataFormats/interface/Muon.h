#ifndef CATTools_Muon_H
#define CATTools_Muon_H

#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "CATTools/DataFormats/interface/Lepton.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

// Define typedefs for convenience
namespace cat {
  class Muon;
  typedef std::vector<Muon>              MuonCollection;
  typedef edm::Ref<MuonCollection>       MuonRef;
  typedef edm::RefVector<MuonCollection> MuonRefVector;
}

namespace cat {

  class Muon : public Lepton{
  public:
    Muon();
    Muon(const reco::LeafCandidate & aMuon);
    virtual ~Muon();
    
    bool isGlobalMuon() const { return isGlobalMuon_; }
    bool isSoftMuon() const { return isSoftMuon_; }
    bool isPFMuon() const { return this->isPF(); }
    bool isTightMuon() const { return this->isTight(); }
    bool isHighPtMuon() const { return isHighPtMuon_; }
    bool isMediumMuon() const { return this->isMedium(); }
    bool isLooseMuon() const { return this->isLoose(); }

    float normalizedChi2() const { return normalizedChi2_; }
    int numberOfValidHits() const { return numberOfValidHits_; }
    int numberOfValidMuonHits() const { return numberOfValidMuonHits_; }
    int numberOfMatchedStations() const { return numberOfMatchedStations_; }
    int numberOfValidPixelHits() const { return numberOfValidPixelHits_; }
    int trackerLayersWithMeasurement() const { return trackerLayersWithMeasurement_; }

    float ip2Dsignificance() const { return ip2Dsig_;}
    float ip3Dsignificance() const { return ip3Dsig_;}
    float trackIso() const {return trkIso_;}

    float shiftedEn() const { if (this->pt() < 100) return 0.002; else return 0.05; }
    float shiftedEnDown() const {return 1-shiftedEn();}
    float shiftedEnUp() const {return  1+shiftedEn();}

    float EcalIso() const {return ecalIso_;}
    float HcalIso() const {return hcalIso_;}
    float PtError() const {return pterror_;}
    
    void setIsGlobalMuon(bool d) { isGlobalMuon_ = d; }
    void setIsSoftMuon(bool d) { isSoftMuon_ = d; }
    void setisHighPtMuon(bool d) { isHighPtMuon_ = d;}

    void  setEcalIso(float d) { ecalIso_ = d;}
    void  setHcalIso(float d) { hcalIso_ = d;}
    void  setPtError(float d) { pterror_ = d;}

    void setNormalizedChi2(float d) { normalizedChi2_ = d; }
    void setNumberOfValidHits(int i) { numberOfValidHits_ = i; }
    void setNumberOfValidMuonHits(int i) { numberOfValidMuonHits_ = i; }
    void setNumberOfMatchedStations(int i) { numberOfMatchedStations_ = i; }
    void setNumberOfValidPixelHits(int i) { numberOfValidPixelHits_ = i; }
    void setTackerLayersWithMeasurement(int i) { trackerLayersWithMeasurement_ = i; }

    void setIp2DSignficance(float ip2Dsig) {ip2Dsig_ = ip2Dsig;}
    void setIp3DSignficance(float ip3Dsig) {ip3Dsig_ = ip3Dsig;}
    void settrkIso(float trkIso) {trkIso_ = trkIso;}
    float scaleFactor(const std::string& name, int sign = 0) const;
    
  private:

    bool isGlobalMuon_;
    bool isSoftMuon_;
    bool isHighPtMuon_;

    float normalizedChi2_;
    float ip2Dsig_;
    float ip3Dsig_;
    float trkIso_;
    float ecalIso_;
    float hcalIso_;
    float pterror_;

    int numberOfValidHits_;
    int numberOfValidMuonHits_;
    int numberOfMatchedStations_;
    int numberOfValidPixelHits_;
    int trackerLayersWithMeasurement_;
  };
}

#endif

