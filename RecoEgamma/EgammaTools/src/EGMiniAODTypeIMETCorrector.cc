#include "RecoEgamma/EgammaTools/interface/EGMiniAODTypeIMETCorrector.h"

#include "DataFormats/Math/interface/deltaR.h"

EGMiniAODTypeIMETCorrector::EGMiniAODTypeIMETCorrector(const edm::ParameterSet& cfg):
  minJetPt_(cfg.getParameter<double>("minJetPt")),
  maxJetEMFrac_(cfg.getParameter<double>("maxJetEMFrac")),
  isGoodEle_(cfg.getParameter<std::string>("eleSelection"))
{
  
}

edm::ParameterSetDescription EGMiniAODTypeIMETCorrector::makePSetDescription()
{
  edm::ParameterSetDescription desc;
  desc.add<double>("minJetPt",15);
  desc.add<double>("maxJetEMFrac",0.9);
  desc.add<std::string>("eleSelection","pt()>20 && userFloat(\"heepTrkPtIso\")<5");
  return desc;
}
CorrMETData EGMiniAODTypeIMETCorrector::operator()(const std::vector<pat::Electron>& eles,
						   const std::vector<pat::Jet>& jets)const
{
  CorrMETData metCorr;
  for(const auto& ele : eles){
    if(isGoodEle_(ele) && !ele.passingPflowPreselection()){
      correctForPFFailingEle(ele,jets,metCorr);
    }  
  }
  return metCorr;
}

void EGMiniAODTypeIMETCorrector::correctForPFFailingEle(const pat::Electron& ele,
			    const std::vector<pat::Jet>& jets,
			    CorrMETData& metCorr)const
{
  //first undo the type1 MET correction from the jet
  const pat::Jet* jet = matchJet(ele,jets);
  if(jet) unCorrectType1MET(metCorr,*jet);
  
  //now sub off all the PF candidates associated with the electron
  for(const auto& pfCand : ele.associatedPackedPFCandidates()){
    metCorr.mex += pfCand->px();
    metCorr.mey += pfCand->py();
    metCorr.sumet -= pfCand->et();
  }
  
  //now add in the electron
  metCorr.mex -= ele.px();
  metCorr.mey -= ele.py();
  metCorr.sumet += ele.et();
  
}

const pat::Jet* EGMiniAODTypeIMETCorrector::matchJet(const pat::Electron& ele,const std::vector<pat::Jet>& jets)const
{
  //so we could do some fancy particle match but DR probably works well enough
  float dR2Max = 0.3*0.3;
  const pat::Jet* bestJet=nullptr;

  float eleEta = ele.eta();
  float elePhi = ele.phi();

  for(auto& jet : jets){
    float dR2 = reco::deltaR2(eleEta,elePhi,jet.eta(),jet.phi());
    if(dR2<dR2Max){
      dR2Max =dR2;
      bestJet = &jet;
    }
  }
  return bestJet;
}

bool EGMiniAODTypeIMETCorrector::passType1Sel(const pat::Jet& jet)const
{
  return jet.pt() > minJetPt_ && jet.chargedEmEnergyFraction() + jet.neutralEmEnergyFraction() < maxJetEMFrac_;
}

void EGMiniAODTypeIMETCorrector::unCorrectType1MET(CorrMETData& metCorr,const pat::Jet& corrJet)const
{
  if(passType1Sel(corrJet)){
    //following reverse of JetMETCorrections/Type1MET/interface/PFJetMETcorrInputProducerT.h
    auto l1CorrJet = corrJet.correctedJet("L1FastJet");
    
    metCorr.mex += (corrJet.px() - l1CorrJet.px());
    metCorr.mey += (corrJet.py() - l1CorrJet.py());
    metCorr.sumet -= (corrJet.et() - l1CorrJet.et());
  }
}
