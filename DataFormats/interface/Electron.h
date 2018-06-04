#ifndef CATTools_Electron_H
#define CATTools_Electron_H

#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "CATTools/DataFormats/interface/Lepton.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"

// Define typedefs for convenience
namespace cat {
  class Electron;
  typedef std::vector<Electron>              ElectronCollection;
  typedef edm::Ref<ElectronCollection>       ElectronRef;
  typedef edm::RefVector<ElectronCollection> ElectronRefVector;
}

namespace cat {

  class Electron : public Lepton{
  public:
    Electron();
    Electron(const reco::LeafCandidate & aElectron);
    virtual ~Electron();

    float electronID(const std::string& name) const;
    float electronID(const char* name) const { return electronID( std::string(name) );}
    bool isVeto() const {return electronID("veto");}
    bool isMediumMVA() const {return electronID("wp90");}
    bool isTightMVA() const {return electronID("wp80");}
    
    int MissingHits() const{return nhitsmiss_;}
    float EcalEnergy() const{return ecal_energy_;}
    float SCEnergy() const{return sc_energy_;}
    float SCRawEnergy() const{return scraw_energy_;}
    float PTErr() const{return pterr_;}
    float R9() const{return r9_;}
    float UnsmearedEnergy() const {return unsmear_energy_;}
    


    float relIso(float dR=0.3) const {
      if( dR < 0.35) return relIso03_;
      else return relIso04_;
    }
    
    float mva() const {return mva_;}
    float zzmva() const {return zzmva_;}

    float scEta() const { return scEta_; }
    bool passConversionVeto() const { return passConversionVeto_; }
    bool isGsfCtfScPixChargeConsistent() const{ return isGsfCtfScPixChargeConsistent_; }
    bool isGsfScPixChargeConsistent() const{ return isGsfScPixChargeConsistent_;}
    bool isGsfCtfChargeConsistent()  const{ return isGsfCtfChargeConsistent_;}

    bool isEB() const{ return isEB_; }

    float ip2Dsignificance() const { return ip2Dsig_;}
    float ip3Dsignificance() const { return ip3Dsig_;}

    float smearedScale() const { return smearedScale_; }
    float shiftedEn() const { if (this->isEB()) return 0.006; else return 0.015; }
    float shiftedEnDown() const {return 1-shiftedEn();}
    float shiftedEnUp() const {return  1+shiftedEn();}
    
    int snuID() const {return snuID_;}
    bool isTrigMVAValid() const { return isTrigMVAValid_; }
    
    void setR9(float r9){r9_ = r9;}
    void setEcalEnergy(float e){ecal_energy_ = e;}
    void setSCEnergy(float e){sc_energy_ = e;}
    void setSCRawEnergy(float e){scraw_energy_ = e;}
    void setPTErr(float e){pterr_ = e;}
    void setUnSmearedEnergy(float e) {unsmear_energy_ = e;}
    void setNMissingHit(int i) {nhitsmiss_=i;}
    void setElectronIDs(const std::vector<pat::Electron::IdPair> & ids) { electronIDs_ = ids; }
    void setElectronID(pat::Electron::IdPair ids) { electronIDs_.push_back(ids); }

    void setrelIso(double dR, double chIso, double nhIso, double phIso, double AEff, double rhoIso, double ecalpt)
    {
      float relIso = ( chIso + std::max(0.0, nhIso + phIso - rhoIso*AEff) )/ ecalpt;
      if( dR < 0.35) relIso03_ = relIso;
      else  relIso04_ = relIso;
    }
    
    void setscEta(float i) { scEta_ = i; }
    void setPassConversionVeto(bool i) {  passConversionVeto_ = i; }
    void setIsGsfCtfScPixChargeConsistent(bool d) { isGsfCtfScPixChargeConsistent_ = d ; }
    void setIsGsfScPixChargeConsistent(bool d) {isGsfScPixChargeConsistent_ = d; }
    void setIsGsfCtfChargeConsistent(bool d) {isGsfCtfChargeConsistent_ = d;}
    void setIsEB(bool d) { isEB_ = d ; }

    void setSNUID(int id) {snuID_ = id;}
    void setTrigMVAValid(bool val) { isTrigMVAValid_ = val; }
    void setIp2DSignficance(float ip2Dsig) {ip2Dsig_ = ip2Dsig;}
    void setIp3DSignficance(float ip3Dsig) {ip3Dsig_ = ip3Dsig;}

    void setSmearedScale(const float scale) { smearedScale_ = scale; }

    float scaleFactor(const std::string& name, int sign = 0) const;
    
    void setMVA(float f){ mva_=f;}
    void setZZMVA(float f){ zzmva_=f;}

  private:

    std::vector<pat::Electron::IdPair> electronIDs_;

    float smearedScale_; // smearedScale = (pt_smeared)/(pt_original). Undo smearing by pt()/smearedScale()
    float relIso03_;
    float relIso04_;
    float ip2Dsig_;
    float ip3Dsig_;
    float scEta_;
    bool passConversionVeto_;
    bool isGsfCtfScPixChargeConsistent_;
    bool isGsfScPixChargeConsistent_;
    bool isGsfCtfChargeConsistent_;
    bool isEB_;
    
    int snuID_;
    int nhitsmiss_;
    bool isTrigMVAValid_;
    float mva_;
    float zzmva_;
    float ecal_energy_;
    float r9_;
    float sc_energy_;
    float scraw_energy_;
    float pterr_;
    float unsmear_energy_;
  };
}

#endif
