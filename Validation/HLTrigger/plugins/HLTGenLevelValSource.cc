#include "DQMServices/Core/interface/DQMStore.h"

#include "DQMServices/Core/interface/DQMEDAnalyzer.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"

#include "Validation/HLTrigger/interface/HLTGenLevelEff.h"

#include <vector>
#include <string>

template <typename ObjType, typename ObjCollType>
class HLTGenLevelValSource : public DQMEDAnalyzer {
public:
  explicit HLTGenLevelValSource(const edm::ParameterSet&);
  ~HLTGenLevelValSource() override = default;
  HLTGenLevelValSource(const HLTGenLevelValSource&) = delete;
  HLTGenLevelValSource& operator=(const HLTGenLevelValSource&) = delete;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void bookHistograms(DQMStore::IBooker&, edm::Run const& run, edm::EventSetup const& c) override;

private:
  // HLTGenLevelEff objects, each for a certain Filter
  std::vector<HLTGenLevelEff<ObjType, ObjCollType>> effs_;
};

template <typename ObjType, typename ObjCollType>
HLTGenLevelValSource<ObjType, ObjCollType>::HLTGenLevelValSource(
    const edm::ParameterSet& config) {
  auto histCollConfigs = config.getParameter<std::vector<edm::ParameterSet> >("collections");
  for (auto& histCollConfig : histCollConfigs) {
    effs_.emplace_back(
        HLTGenLevelEff<ObjType, ObjCollType>(histCollConfig, consumesCollector()));
  }
}


//all python parameters used to configure the module must be specified here
template <typename ObjType, typename ObjCollType>
void HLTGenLevelValSource<ObjType, ObjCollType>::fillDescriptions(
    edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("objs", edm::InputTag(""));
  desc.addVPSet("collections",
                HLTGenLevelEff<ObjType, ObjCollType>::makePSetDescription(),
                std::vector<edm::ParameterSet>());
  descriptions.addDefault(desc);
}

template <typename ObjType, typename ObjCollType>
void HLTGenLevelValSource<ObjType, ObjCollType>::bookHistograms(
    DQMStore::IBooker& iBooker, const edm::Run& run, const edm::EventSetup& setup) {
  for (auto& eff : effs_)
      eff.bookHists(iBooker);
}

template <typename ObjType, typename ObjCollType>
void HLTGenLevelValSource<ObjType, ObjCollType>::analyze(
    const edm::Event& event, const edm::EventSetup& setup) {
  for (auto& eff : effs_)
    eff.fill(event, setup);
}

#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
using HLTEleGenLevelValSource = HLTGenLevelValSource<reco::GsfElectron, reco::GsfElectronCollection>;
using HLTPhoGenLevelValSource = HLTGenLevelValSource<reco::Photon, reco::PhotonCollection>;
using HLTMuGenLevelValSource = HLTGenLevelValSource<reco::Muon, reco::MuonCollection>;
DEFINE_FWK_MODULE(HLTEleGenLevelValSource);
DEFINE_FWK_MODULE(HLTPhoGenLevelValSource);
DEFINE_FWK_MODULE(HLTMuGenLevelValSource);
