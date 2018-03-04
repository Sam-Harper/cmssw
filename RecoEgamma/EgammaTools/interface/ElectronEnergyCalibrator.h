#ifndef RecoEgamma_EgammaTools_ElectronEnergyCalibrator_h
#define RecoEgamma_EgammaTools_ElectronEnergyCalibrator_h


#include "FWCore/Utilities/interface/StreamID.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "RecoEgamma/EgammaTools/interface/EnergyScaleCorrection.h"
#include "RecoEgamma/EgammaTools/interface/EpCombinationToolSemi.h"

#include <TRandom.h>

#include <vector>

class ElectronEnergyCalibrator
{
public:
  ElectronEnergyCalibrator() {}
  ElectronEnergyCalibrator(EpCombinationToolSemi &combinator, bool isMC, 
			   bool synchronization, const std::string& );
  ~ElectronEnergyCalibrator() ;
  
  /// Initialize with a random number generator (if not done, it will use the CMSSW service)
  /// Caller code owns the TRandom.
  void initPrivateRng(TRandom *rnd) ;

  //set the minimum et to apply the correction to
  void setMinEt(float val){minEt_=val;}
  
  /// Correct this electron.
  /// StreamID is needed when used with CMSSW Random Number Generator
  std::vector<float> calibrate(reco::GsfElectron &ele, const unsigned int runNumber, 
			       const EcalRecHitCollection* recHits, edm::StreamID const & id = edm::StreamID::invalidStreamID(), const int eventIsMC = -1) const ;
  std::vector<float> calibrate(reco::GsfElectron &ele, const unsigned int runNumber, 
			       const EcalRecHitCollection* recHits, const float smearNrSigma, const int eventIsMC = -1) const ;

private:
  void setEcalEnergy(reco::GsfElectron& ele,const float scale,const float smear)const;
  std::pair<float,float> calCombinedMom(reco::GsfElectron& ele,const float scale,const float smear)const;
  /// Return a number distributed as a unit gaussian, drawn from the private RNG if initPrivateRng was called,
  /// or from the CMSSW RandomNumberGenerator service
  /// If synchronization is set to true, it returns a fixed number (1.0)
  double gauss(edm::StreamID const& id) const ;
  

  // whatever data will be needed
  EnergyScaleCorrection correctionRetriever_;
  EpCombinationToolSemi *epCombinationTool_; //this is not owned
  bool isMC_;
  bool synchronization_;
  TRandom *rng_; //this is not owned
  float minEt_;

  

};

#endif
