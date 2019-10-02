#ifndef RecoEgamma_EgammaTools_EGMiniAODTypeIMETCorrector_h
#define RecoEgamma_EgammaTools_EGMiniAODTypeIMETCorrector_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/METReco/interface/CorrMETData.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

class EGMiniAODTypeIMETCorrector{
public:
  explicit EGMiniAODTypeIMETCorrector(const edm::ParameterSet& cfg);

  CorrMETData operator()(const std::vector<pat::Electron>& eles,
		       const std::vector<pat::Jet>& jets)const;

  static edm::ParameterSetDescription makePSetDescription();

private:
  void correctForPFFailingEle(const pat::Electron& ele,const std::vector<pat::Jet>& jets,
			      CorrMETData& metCorr)const;
  const pat::Jet* matchJet(const pat::Electron& ele,const std::vector<pat::Jet>& jets)const;
  bool passType1Sel(const pat::Jet& jet)const;
  void unCorrectType1MET(CorrMETData& metCorr,const pat::Jet& corrJet)const;

  float minJetPt_;
  float maxJetEMFrac_;
  
  StringCutObjectSelector<pat::Electron> isGoodEle_;

};

#endif
