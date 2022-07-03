#include "DQMOffline/Trigger/interface/FunctionDefs.h"

template <>
std::function<float(const reco::GsfElectron&)> hltdqm::getUnaryFuncExtraFloat<reco::GsfElectron>(
    const std::string& varName) {
  std::function<float(const reco::GsfElectron&)> varFunc;
  if (varName == "scEta")
    varFunc = scEtaFunc<reco::GsfElectron>;
  else if (varName == "hOverE")
    varFunc = [](const reco::GsfElectron& ele) -> float { return ele.hcalOverEcal(); };
  return varFunc;
}

template <>
std::function<float(const reco::Photon&)> hltdqm::getUnaryFuncExtraFloat<reco::Photon>(const std::string& varName) {
  std::function<float(const reco::Photon&)> varFunc;
  if (varName == "scEta")
    varFunc = scEtaFunc<reco::Photon>;
  else if (varName == "hOverE")
    varFunc = [](const reco::Photon& pho) -> float { return pho.hadTowOverEm(); };
  return varFunc;
}

//hack to put the function definitions in the non-plugin libary
//so they can be used by other packages
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/MuonReco/interface/Muon.h"
auto testPho = hltdqm::getUnaryFuncFloat<reco::Photon>("et");
auto testEle = hltdqm::getUnaryFuncFloat<reco::GsfElectron>("et");
auto testMu = hltdqm::getUnaryFuncFloat<reco::Muon>("et");
