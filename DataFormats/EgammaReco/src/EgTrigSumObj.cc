#include "DataFormats/EgammaReco/interface/EgTrigSumObj.h"

#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/ElectronSeed.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"

reco::EgTrigSumObj::EgTrigSumObj(float energy, float pt, float eta, float phi)
    : energy_(energy), pt_(pt), eta_(eta), phi_(phi), hasPixelMatch_(false) {}

reco::EgTrigSumObj::EgTrigSumObj(const reco::RecoEcalCandidate& ecalCand)
    : energy_(ecalCand.energy()),
      pt_(ecalCand.pt()),
      eta_(ecalCand.eta()),
      phi_(ecalCand.phi()),
      hasPixelMatch_(false),
      superCluster_(ecalCand.superCluster()) {}

void reco::EgTrigSumObj::setSeeds(reco::ElectronSeedRefVector seeds) {
  seeds_ = std::move(seeds);
  hasPixelMatch_ = false;
  for (const auto& seed : seeds_) {
    if (!seed->hitInfo().empty()) {
      hasPixelMatch_ = true;
      break;
    }
  }
}

bool reco::EgTrigSumObj::hasVar(const std::string& varName) const {
  return std::binary_search(vars_.begin(), vars_.end(), varName, VarComparer());
}

float reco::EgTrigSumObj::var(const std::string& varName, const bool raiseExcept) const {
  //here we have a guaranteed sorted vector with unique entries
  auto varIt = std::equal_range(vars_.begin(), vars_.end(), varName, VarComparer());
  if (varIt.first != varIt.second)
    return varIt.first->second;
  else if (raiseExcept) {
    cms::Exception ex("AttributeError");
    ex << " error variable " << varName << " is not present, variables present are " << varNamesStr();
    throw ex;
  } else {
    return std::numeric_limits<float>::max();
  }
}

std::vector<std::string> reco::EgTrigSumObj::varNames() const {
  std::vector<std::string> names;
  for (const auto& var : vars_) {
    names.push_back(var.first);
  }
  return names;
}

std::string reco::EgTrigSumObj::varNamesStr() const {
  std::string retVal;
  auto names = varNames();
  for (const auto& name : names) {
    if (!retVal.empty())
      retVal += " ";
    retVal += name;
  }
  return retVal;
}

void reco::EgTrigSumObj::setVars(std::vector<std::pair<std::string, float>> vars) {
  vars_ = std::move(vars);
  std::sort(vars_.begin(), vars_.end(), [](auto& lhs, auto& rhs) { return lhs.first < rhs.first; });
}
